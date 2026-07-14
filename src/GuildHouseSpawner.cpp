#include "GuildHouseSpawner.h"

#include "GuildHouseMgr.h"
#include "GuildHouseCatalogMgr.h"
#include "GuildHouseDefines.h"
#include "GuildHouseTypes.h"

#include "DatabaseEnv.h"
#include "ObjectMgr.h"
#include "MapMgr.h"
#include "Map.h"
#include "Creature.h"
#include "GameObject.h"
#include "Log.h"

#include <sstream>

GuildHouseSpawner&
GuildHouseSpawner::Instance()
{
    static GuildHouseSpawner instance;
    return instance;
}

// =====================================================
// Startup loader
//
// Only loads assets that have already been placed.
// Purchased assets remain stored.
// =====================================================

void GuildHouseSpawner::LoadPlacedAssets()
{
    LOG_INFO("server.loading", "GuildHouseSpawner permanent spawn mode enabled.");
}

bool GuildHouseSpawner::HasExistingSpawn(uint32 instanceId, uint32 assetId)
{
    QueryResult result = CharacterDatabase.Query("SELECT COUNT(*) FROM guildhouse_spawn WHERE instanceId={} AND assetId={} AND enabled=1", instanceId, assetId);

    if (!result)
        return false;

    return result->Fetch()[0].Get<uint32>() > 0;
}

// =====================================================
// Spawn one placed asset
// =====================================================

bool GuildHouseSpawner::SpawnAsset(uint32 guildId, uint32 assetId)
{
    QueryResult result = CharacterDatabase.Query("SELECT catalogId, status, instanceId, positionX, positionY, positionZ, orientation FROM guildhouse_asset WHERE assetId={} AND guildId={}", assetId, guildId);
    if (!result)
        return false;

    Field* fields = result->Fetch();

    uint32 instanceId = fields[2].Get<uint32>();
    if (HasExistingSpawn(instanceId, assetId))
    {
        LOG_INFO("server.loading", "GuildHouse asset {} already spawned.", assetId);
        return true;
    }

    uint32 catalogId = fields[0].Get<uint32>();
    uint8 status = fields[1].Get<uint8>();
    if (status != GH_ASSET_PLACED)
        return false;
    
    float baseX = fields[3].Get<float>();
    float baseY = fields[4].Get<float>();
    float baseZ = fields[5].Get<float>();
    float baseO = fields[6].Get<float>();

    const GHCatalog* catalog = sGuildHouseCatalogMgr.GetCatalog(catalogId);
    if (!catalog)
        return false;

    for (const GHCatalogAsset& component : catalog->Components)
    {
        float x = baseX + component.XOffset;
        float y = baseY + component.YOffset;
        float z = baseZ + component.ZOffset;
        float o = baseO + component.OOffset;

        if (GuildHouseUtil::HasFlag(component.SpawnFlags, GH_SPAWN_CREATURE))
            SpawnCreature(guildId, instanceId, assetId, component.Entry, x, y, z, o);

        if (GuildHouseUtil::HasFlag(component.SpawnFlags, GH_SPAWN_GAMEOBJECT))
            SpawnGameObject(guildId, instanceId, assetId, component.Entry, x, y, z, o);
    }

    return true;
}

// =====================================================
// Spawn Creature
//
// Creates permanent AzerothCore creature spawn
// and records it in guildhouse_spawn.
// =====================================================

bool GuildHouseSpawner::SpawnCreature(uint32 guildId, uint32 instanceId, uint32 assetId, uint32 entry, float x, float y, float z, float o)
{
    ObjectGuid::LowType spawnGuid = sObjectMgr->GenerateCreatureSpawnId();

    std::ostringstream creatureSql;
    creatureSql << "INSERT INTO creature (guid,id,map,position_x,position_y,position_z,orientation) VALUES ("
        << spawnGuid << "," << entry << "," << GH_MAP << "," << x << "," << y << "," << z << "," << o << ")";
    WorldDatabase.Execute(creatureSql.str());

    std::ostringstream trackingSql;
    trackingSql << "INSERT INTO guildhouse_spawn (assetId,guildId,instanceId,spawnGuid,spawnType,mapId,positionX,positionY,positionZ,orientation) VALUES ("
        << assetId << "," << guildId << "," << instanceId << "," << spawnGuid << "," << 0 << "," << GH_MAP << "," << x << "," << y << "," << z << "," << o << ")";
    CharacterDatabase.Execute(trackingSql.str());

    LOG_INFO("server.loading", "GuildHouse creature {} spawned (asset {})", entry, assetId);

    return true;
}

// =====================================================
// Spawn GameObject
//
// Creates permanent AzerothCore gameobject spawn
// and records it in guildhouse_spawn.
// =====================================================

bool GuildHouseSpawner::SpawnGameObject(uint32 guildId, uint32 instanceId, uint32 assetId, uint32 entry, float x, float y, float z, float o)
{
    ObjectGuid::LowType spawnGuid = sObjectMgr->GenerateGameObjectSpawnId();

    std::ostringstream objectSql;
    objectSql << "INSERT INTO gameobject (guid,id,map,position_x,position_y,position_z,orientation) VALUES ("
        << spawnGuid << "," << entry << "," << GH_MAP << "," << x << "," << y << "," << z << "," << o << ")";
    WorldDatabase.Execute(objectSql.str());

    std::ostringstream trackingSql;
    trackingSql << "INSERT INTO guildhouse_spawn (assetId,guildId,instanceId,spawnGuid,spawnType,mapId,positionX,positionY,positionZ,orientation) VALUES ("
        << assetId << "," << guildId << "," << instanceId << "," << spawnGuid << "," << 1 << "," << GH_MAP << "," << x << "," << y << "," << z << "," << o << ")";
    CharacterDatabase.Execute(trackingSql.str());

    LOG_INFO("server.loading", "GuildHouse gameobject {} spawned (asset {})", entry, assetId);

    return true;
}

// =====================================================
// Remove Asset
//
// Used later by:
// .gh store
// .gh sell
// .gh move
//
// Uses guildhouse_spawn tracking table.
// =====================================================

bool GuildHouseSpawner::RemoveAsset(uint32 guildId, uint32 instanceId, uint32 assetId)
{
    QueryResult result = CharacterDatabase.Query("SELECT spawnGuid, spawnType FROM guildhouse_spawn WHERE guildId={} AND instanceId={} AND assetId={} AND enabled=1", guildId, instanceId, assetId);

    if (!result)
        return false;

    do
    {
        Field* fields = result->Fetch();

        uint32 spawnGuid = fields[0].Get<uint32>();
        uint8 spawnType = fields[1].Get<uint8>();

        if (spawnType == 0)
            WorldDatabase.Execute("DELETE FROM creature WHERE guid={}", spawnGuid);
        else
            WorldDatabase.Execute("DELETE FROM gameobject WHERE guid={}", spawnGuid);

    } while (result->NextRow());

    CharacterDatabase.Execute("UPDATE guildhouse_spawn SET enabled=0 WHERE guildId={} AND instanceId={} AND assetId={}", guildId, instanceId, assetId);

    return true;
}
