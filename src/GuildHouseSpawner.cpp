#include "GuildHouseSpawner.h"

#include "GuildHouseMgr.h"
#include "GuildHousePhaseMgr.h"
#include "GuildHouseCatalogMgr.h"
#include "GuildHouseTypes.h"

#include "MapMgr.h"
#include "DatabaseEnv.h"
#include "ObjectMgr.h"
#include "Log.h"

#include <sstream>

GuildHouseSpawner& GuildHouseSpawner::Instance()
{
    static GuildHouseSpawner instance;
    return instance;
}

// =====================================================
// Startup loader
// =====================================================
void GuildHouseSpawner::LoadPlacedAssets()
{
    LOG_INFO("server.loading", "Loading Guild House phase spawns");

    QueryResult result = CharacterDatabase.Query("SELECT guildId,assetId,catalogId,status,positionX,positionY,positionZ,orientation FROM guildhouse_asset WHERE status={}", GH_ASSET_PLACED);
    if(!result)
        return;

    do
    {
        Field* fields = result->Fetch();
        if(fields[3].Get<uint8_t>() == GH_ASSET_PLACED)
            SpawnAsset(fields[0].Get<uint32>(), fields[1].Get<uint32>(), fields[2].Get<uint32>(), fields[4].Get<float>(), fields[5].Get<float>(), fields[6].Get<float>(), fields[7].Get<float>());
    } while(result->NextRow());
}

// =====================================================
// Existing spawn
// =====================================================
bool GuildHouseSpawner::HasExistingSpawn(uint32_t guildId, uint32_t assetId)
{
   // QueryResult result = CharacterDatabase.Query("SELECT COUNT(*) FROM guildhouse_spawn WHERE guildId={} AND assetId={} AND enabled=1", guildId, assetId);
     QueryResult result = CharacterDatabase.Query("SELECT COUNT(*) FROM guildhouse_spawn WHERE guildId={} AND assetId={}", guildId, assetId);
    if(!result)
        return false;

    return result->Fetch()[0].Get<uint32>() > 0;
}

// =====================================================
// Spawn Asset
// =====================================================
bool GuildHouseSpawner::SpawnAsset(uint32_t guildId, uint32_t assetId, uint32_t catalogId, float x, float y, float z, float o)
{
    LOG_INFO("server.loading", "In SpawnAsset");
    
    if(HasExistingSpawn(guildId, assetId))
        return false;

    LOG_INFO("server.loading", "No Duplicate");
    
    const GHPhaseRecord* phase = sGuildHousePhaseMgr.GetPhase(guildId);
    if(!phase)
        return false;

    LOG_INFO("server.loading", "In correct Phase");
    
    const GHCatalog* catalog = sGuildHouseCatalogMgr.GetCatalog(catalogId);
    if(!catalog)
        return false;

    LOG_INFO("server.loading", "Have Catalog: Spawning");

    for(auto const& component : catalog->Components)
    {
        float cx = x + component.XOffset;
        float cy = y + component.YOffset;
        float cz = z + component.ZOffset;
        float co = o + component.OOffset;

        if(GuildHouseUtil::HasFlag(component.SpawnFlags, GH_SPAWN_CREATURE))
            SpawnCreature(guildId, assetId, phase->PhaseMask, phase->MapId, component.Entry, cx, cy, cz, co);

        if(GuildHouseUtil::HasFlag(component.SpawnFlags, GH_SPAWN_GAMEOBJECT))
            SpawnGameObject(guildId, assetId, phase->PhaseMask, phase->MapId, component.Entry, cx, cy, cz, co);
    }

    return true;
}

// =====================================================
// Creature
// =====================================================
bool GuildHouseSpawner::SpawnCreature(uint32_t guildId, uint32_t assetId, uint32_t phaseMask, uint32_t mapId, uint32_t entry, float x, float y, float z, float o)
{
   Map* map = sMapMgr->CreateBaseMap(mapId);
    if (!map)
        return false;
    
    LOG_INFO("server.loading", "Have Map");

    Creature* creature = new Creature();
    if (!creature->Create(map->GenerateLowGuid<HighGuid::Unit>(), map, phaseMask, entry, 0, x, y, z, o))
    {
        delete creature;
        return false;
    }

    LOG_INFO("server.loading", "Created first creature");

    creature->SaveToDB(mapId, (1 << map->GetSpawnMode()), phaseMask);

    uint32 spawnId = creature->GetSpawnId();

    creature->CleanupsBeforeDelete();
    delete creature;

    creature = new Creature();
    if (!creature->LoadCreatureFromDB(spawnId, map))
    {
        delete creature;
        return false;
    }

    LOG_INFO("server.loading", "Can load creature from DB, saving to guildhouse_spawn");

    sObjectMgr->AddCreatureToGrid(spawnId, sObjectMgr->GetCreatureData(spawnId));
    
    CharacterDatabase.Execute("INSERT INTO guildhouse_spawn (guildId,assetId,spawnGuid,spawnType,mapId,phaseMask,x,y,z,o) "
        "VALUES ({},{},{},{},{},{},{},{},{},{})", guildId, assetId, spawnId, 0, mapId, phaseMask, x, y, z, o);

    return true;
}

// =====================================================
// GameObject
// =====================================================
bool GuildHouseSpawner::SpawnGameObject(uint32_t guildId, uint32_t assetId, uint32_t phaseMask, uint32_t mapId, uint32_t entry, float x, float y, float z, float o)
{
    uint32 guid = sObjectMgr->GenerateGameObjectSpawnId();

    WorldDatabase.Execute("INSERT INTO gameobject (guid,id,map,phaseMask,position_x,position_y,position_z,orientation)"
        "VALUES ({},{},{},{},{},{},{},{})", guid, entry, mapId, phaseMask, x, y, z, o);

    //CharacterDatabase.Execute("INSERT INTO guildhouse_spawn (guildId,assetId,spawnGuid,spawnType,mapId,phaseMask,positionX,positionY,positionZ,orientation)"
    //    "VALUES ({},{},{},{},{},{},{},{},{},{})", guildId, assetId, guid, 1, mapId, phaseMask, x, y, z, o);
    CharacterDatabase.Execute("INSERT INTO guildhouse_spawn (guildId,assetId,spawnGuid,spawnType,mapId,phaseMask,x,y,z,o) "
        "VALUES ({},{},{},{},{},{},{},{},{},{})", guildId, assetId, guid, 1, mapId, phaseMask, x, y, z, o);

    return true;
}

// =====================================================
// Remove asset
// =====================================================
bool GuildHouseSpawner::RemoveAsset(uint32_t guildId, uint32_t assetId)
{
    //QueryResult result = CharacterDatabase.Query("SELECT spawnGuid,spawnType FROM guildhouse_spawn WHERE guildId={} AND assetId={} AND enabled=1", guildId, assetId);
    QueryResult result = CharacterDatabase.Query("SELECT spawnGuid,spawnType FROM guildhouse_spawn WHERE guildId={} AND assetId={}", guildId, assetId);
    if(!result)
        return false;

    do
    {
        Field* fields = result->Fetch();
        uint32 guid = fields[0].Get<uint32>();
        uint8 type = fields[1].Get<uint8>();

        if(type == 0)
            RemoveCreatureSpawn(guid);
        else
            RemoveGameObjectSpawn(guid);
    }while(result->NextRow());

    //CharacterDatabase.Execute("UPDATE guildhouse_spawn SET enabled=0 WHERE guildId={} AND assetId={}", guildId, assetId);

    return true;
}

// =====================================================
// Remove all
// =====================================================
bool GuildHouseSpawner::RemoveAllAssets(uint32_t guildId)
{
    //QueryResult result = CharacterDatabase.Query("SELECT DISTINCT assetId FROM guildhouse_spawn WHERE guildId={} AND enabled=1", guildId);
    QueryResult result = CharacterDatabase.Query("SELECT DISTINCT assetId FROM guildhouse_spawn WHERE guildId={}", guildId);
    if(!result)
        return false;

    do
    {
        RemoveAsset(guildId, result->Fetch()[0].Get<uint32>());
    }while(result->NextRow());

    return true;
}

// =====================================================
// World removal
// =====================================================
bool GuildHouseSpawner::RemoveCreatureSpawn(uint32 guid)
{
    WorldDatabase.Execute("DELETE FROM creature WHERE guid={}", guid);
    return true;
}

bool GuildHouseSpawner::RemoveGameObjectSpawn(uint32 guid)
{
    WorldDatabase.Execute("DELETE FROM gameobject WHERE guid={}", guid);
    return true;
}

// =====================================================
// Move Asset
// =====================================================
bool GuildHouseSpawner::MoveAsset(uint32_t guildId, uint32_t assetId, float deltaX, float deltaY, float deltaZ, float orientation)
{
    //CharacterDatabase.Execute("UPDATE guildhouse_spawn SET positionX=positionX+{},positionY=positionY+{},positionZ=positionZ+{},orientation={} "
    CharacterDatabase.Execute("UPDATE guildhouse_spawn SET x=x+{},y=y+{},z=z+{},o={} "
        "WHERE guildId={} AND assetId={} AND enabled=1", deltaX, deltaY, deltaZ, orientation, guildId, assetId);

    return true;
}
