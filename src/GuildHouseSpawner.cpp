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
    LOG_INFO(
        "module",
        "GuildHouseSpawner permanent spawn mode enabled.");
}

bool GuildHouseSpawner::HasExistingSpawn(uint32 assetId)
{
    QueryResult result = CharacterDatabase.Query(
        "SELECT COUNT(*) FROM guildhouse_spawn WHERE assetId=%u AND enabled=1",
        assetId);

    if (!result)
        return false;

    return result->Fetch()[0].Get<uint32>() > 0;
}

// =====================================================
// Spawn one placed asset
// =====================================================

bool GuildHouseSpawner::SpawnAsset(
    uint32 guildId,
    uint32 assetId)
{
    if (HasExistingSpawn(assetId))
    {
        LOG_INFO(
            "module",
            "GuildHouse asset {} already spawned.",
            assetId);

        return true;
    }


    QueryResult result =
        CharacterDatabase.Query(
            "SELECT catalogId, status, "
            "positionX, positionY, positionZ, orientation "
            "FROM guildhouse_asset "
            "WHERE assetId=%u "
            "AND guildId=%u",
            assetId,
            guildId);



    if (!result)
        return false;



    Field* fields =
        result->Fetch();



    uint32 catalogId =
        fields[0].Get<uint32>();


    uint8 status =
        fields[1].Get<uint8>();



    if (status != GH_ASSET_PLACED)
        return false;



    float baseX =
        fields[2].Get<float>();

    float baseY =
        fields[3].Get<float>();

    float baseZ =
        fields[4].Get<float>();

    float baseO =
        fields[5].Get<float>();



    const GHCatalog* catalog =
        sGuildHouseCatalogMgr.GetCatalog(
            catalogId);



    if (!catalog)
        return false;



    for (const GHCatalogAsset& component :
         catalog->Components)
    {

        float x =
            baseX + component.XOffset;

        float y =
            baseY + component.YOffset;

        float z =
            baseZ + component.ZOffset;

        float o =
            baseO + component.OOffset;



        if (GuildHouseUtil::HasFlag(
                component.SpawnFlags,
                GH_SPAWN_CREATURE))
        {
            SpawnCreature(
                guildId,
                assetId,
                component.Entry,
                x,
                y,
                z,
                o);
        }



        if (GuildHouseUtil::HasFlag(
                component.SpawnFlags,
                GH_SPAWN_GAMEOBJECT))
        {
            SpawnGameObject(
                guildId,
                assetId,
                component.Entry,
                x,
                y,
                z,
                o);
        }
    }


    return true;
}



// =====================================================
// Spawn Creature
//
// Creates permanent AzerothCore creature spawn
// and records it in guildhouse_spawn.
// =====================================================

bool GuildHouseSpawner::SpawnCreature(
    uint32 guildId,
    uint32 assetId,
    uint32 entry,
    float x,
    float y,
    float z,
    float o)
{

    ObjectGuid::LowType spawnGuid =
        sObjectMgr->GenerateCreatureSpawnId();



    std::ostringstream creatureSql;


    creatureSql
        << "INSERT INTO creature "
        << "(guid,id,map,phaseMask,"
        << "position_x,position_y,position_z,"
        << "orientation) VALUES ("
        << spawnGuid << ","
        << entry << ","
        << GH_MAP << ","
        << GuildHouseUtil::GetGuildHousePhase(guildId)
        << ","
        << x << ","
        << y << ","
        << z << ","
        << o
        << ")";


    WorldDatabase.Execute(
        creatureSql.str());



    std::ostringstream trackingSql;


    trackingSql
        << "INSERT INTO guildhouse_spawn "
        << "(assetId,guildId,spawnGuid,"
        << "spawnType,mapId,phase,"
        << "positionX,positionY,positionZ,orientation)"
        << " VALUES ("
        << assetId << ","
        << guildId << ","
        << spawnGuid << ","
        << 0 << ","
        << GH_MAP << ","
        << GuildHouseUtil::GetGuildHousePhase(guildId)
        << ","
        << x << ","
        << y << ","
        << z << ","
        << o
        << ")";


    CharacterDatabase.Execute(
        trackingSql.str());



    LOG_INFO(
        "module",
        "GuildHouse creature {} spawned (asset {})",
        entry,
        assetId);


    return true;
}



// =====================================================
// Spawn GameObject
//
// Creates permanent AzerothCore gameobject spawn
// and records it in guildhouse_spawn.
// =====================================================

bool GuildHouseSpawner::SpawnGameObject(
    uint32 guildId,
    uint32 assetId,
    uint32 entry,
    float x,
    float y,
    float z,
    float o)
{

    ObjectGuid::LowType spawnGuid =
        sObjectMgr->GenerateGameObjectSpawnId();



    std::ostringstream objectSql;


    objectSql
        << "INSERT INTO gameobject "
        << "(guid,id,map,phaseMask,"
        << "position_x,position_y,position_z,"
        << "orientation) VALUES ("
        << spawnGuid << ","
        << entry << ","
        << GH_MAP << ","
        << GuildHouseUtil::GetGuildHousePhase(guildId)
        << ","
        << x << ","
        << y << ","
        << z << ","
        << o
        << ")";


    WorldDatabase.Execute(
        objectSql.str());



    std::ostringstream trackingSql;


    trackingSql
        << "INSERT INTO guildhouse_spawn "
        << "(assetId,guildId,spawnGuid,"
        << "spawnType,mapId,phase,"
        << "positionX,positionY,positionZ,orientation)"
        << " VALUES ("
        << assetId << ","
        << guildId << ","
        << spawnGuid << ","
        << 1 << ","
        << GH_MAP << ","
        << GuildHouseUtil::GetGuildHousePhase(guildId)
        << ","
        << x << ","
        << y << ","
        << z << ","
        << o
        << ")";


    CharacterDatabase.Execute(
        trackingSql.str());



    LOG_INFO(
        "module",
        "GuildHouse gameobject {} spawned (asset {})",
        entry,
        assetId);


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

bool GuildHouseSpawner::RemoveAsset(
    uint32 guildId,
    uint32 assetId)
{
    QueryResult result =
        CharacterDatabase.Query(
            "SELECT spawnGuid, spawnType "
            "FROM guildhouse_spawn "
            "WHERE guildId=%u "
            "AND assetId=%u "
            "AND enabled=1",
            guildId,
            assetId);



    if (!result)
        return false;



    do
    {
        Field* fields =
            result->Fetch();


        uint32 spawnGuid =
            fields[0].Get<uint32>();


        uint8 spawnType =
            fields[1].Get<uint8>();


        if (spawnType == 0)
        {
            WorldDatabase.Execute(
                "DELETE FROM creature "
                "WHERE guid=%u",
                spawnGuid);
        }
        else
        {
            WorldDatabase.Execute(
                "DELETE FROM gameobject "
                "WHERE guid=%u",
                spawnGuid);
        }


    } while (result->NextRow());



    CharacterDatabase.Execute(
        "UPDATE guildhouse_spawn "
        "SET enabled=0 "
        "WHERE guildId=%u "
        "AND assetId=%u",
        guildId,
        assetId);



    return true;
}
