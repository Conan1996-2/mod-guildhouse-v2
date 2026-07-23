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

    for (auto const& [guildId, house] : sGuildHouseMgr.GetHouses())
    {
        for (auto const& [assetId, asset] : house.Assets)
        {
            if (asset.Status != GH_ASSET_PLACED)
                continue;

            SpawnAsset(guildId, asset.AssetId, asset.CatalogId, asset.X, asset.Y, asset.Z, asset.O);
        }
    }
}

// =====================================================
// Existing spawn
// =====================================================
bool GuildHouseSpawner::HasExistingSpawn(uint32_t guildId, uint32_t assetId)
{
    GHGuildHouse const* house = sGuildHouseMgr.GetGuildHouse(guildId);

    if (!house)
        return false;

    for (GHGuildSpawn const& spawn : house->Spawns)
    {
        if (spawn.AssetId == assetId && spawn.SpawnGuid != 0)
            return true;
    }

    return false;
}

// =====================================================
// Spawn Asset
// =====================================================
bool GuildHouseSpawner::SpawnAsset(uint32_t guildId, uint32_t assetId, uint32_t catalogId, float x, float y, float z, float o, bool checkExists)
{
    LOG_INFO("server.loading", "In SpawnAsset {}, {}", guildId, assetId);
    
    if(checkExists && HasExistingSpawn(guildId, assetId))
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
        "VALUES ({},{},{},{},{},{},{},{},{},{})", guildId, assetId, spawnId, GH_SPAWN_CREATURE, mapId, phaseMask, x, y, z, o);

    GHGuildHouse* house = sGuildHouseMgr.GetGuildHouse(guildId);
    if (house)
    {
        GHGuildSpawn spawn;
        spawn.GuildId = guildId;
        spawn.AssetId = assetId;
        spawn.SpawnGuid = spawnId;
        spawn.SpawnType = GH_SPAWN_CREATURE;
        spawn.PhaseMask = phaseMask;

        spawn.MapId = mapId;
        spawn.X = x;
        spawn.Y = y;
        spawn.Z = z;
        spawn.O = o;

        house->Spawns.push_back(std::move(spawn));
    }
    
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

    CharacterDatabase.Execute("INSERT INTO guildhouse_spawn (guildId,assetId,spawnGuid,spawnType,mapId,phaseMask,x,y,z,o) "
        "VALUES ({},{},{},{},{},{},{},{},{},{})", guildId, assetId, guid, 1, mapId, phaseMask, x, y, z, o);

    return true;
}

// =====================================================
// Remove asset
// =====================================================
bool GuildHouseSpawner::RemoveAsset(uint32_t guildId, uint32_t assetId)
{
    GHGuildHouse* house = sGuildHouseMgr.GetGuildHouse(guildId);
    if (!house)
        return false;

    bool removed = false;
    for (auto itr = house->Spawns.begin(); itr != house->Spawns.end(); )
    {
        if (itr->AssetId != assetId)
        {
            ++itr;
            continue;
        }

        uint32_t spawnGuid = itr->SpawnGuid;
        bool success = false;

        switch (itr->SpawnType)
        {
            case GH_SPAWN_CREATURE:
                success = RemoveCreatureSpawn(spawnGuid);
                break;
            case GH_SPAWN_GAMEOBJECT:
                success = RemoveGameObjectSpawn(spawnGuid);
                break;
            default:
                break;
        }

        if (success)
        {
            CharacterDatabase.Execute("DELETE FROM guildhouse_spawn WHERE guildId={} AND assetId={} AND spawnGuid={}", guildId, assetId, spawnGuid);
            itr = house->Spawns.erase(itr);
            removed = true;
        }
        else
        {
            ++itr;
        }
    }

    return removed;
}

// =====================================================
// Remove all
// =====================================================
bool GuildHouseSpawner::RemoveAllAssets(uint32_t guildId)
{
    GHGuildHouse* house = sGuildHouseMgr.GetGuildHouse(guildId);
    if (!house)
        return false;

    std::unordered_set<uint32_t> assetIds;

    for (GHGuildSpawn const& spawn : house->Spawns)
        assetIds.insert(spawn.AssetId);

    for (uint32_t assetId : assetIds)
        RemoveAsset(guildId, assetId);

    return true;
}

