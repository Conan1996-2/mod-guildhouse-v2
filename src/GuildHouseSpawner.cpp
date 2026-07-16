#include "GuildHouseSpawner.h"

#include "GuildHouseMgr.h"
#include "GuildHouseInstanceMgr.h"
#include "GuildHouseCatalogMgr.h"
#include "GuildHouseDefines.h"
#include "GuildHouseTypes.h"

#include "DatabaseEnv.h"
#include "ObjectMgr.h"
#include "Log.h"

#include <cmath>
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
// Loads already purchased and placed assets.
// Missing spawns are recreated.
// =====================================================

void GuildHouseSpawner::LoadPlacedAssets()
{
    LOG_INFO(
        "server.loading",
        "GuildHouseSpawner loading placed guild assets.");


    QueryResult result = CharacterDatabase.Query(
        "SELECT guildId, assetId FROM guildhouse_asset WHERE status={}",
        GH_ASSET_PLACED);


    if (!result)
        return;


    do
    {
        Field* fields = result->Fetch();


        uint32 guildId = fields[0].Get<uint32>();

        uint32 assetId = fields[1].Get<uint32>();


        SpawnAsset(
            guildId,
            assetId);


    } while(result->NextRow());
}



// =====================================================
// Check existing spawn
// =====================================================

bool GuildHouseSpawner::HasExistingSpawn(
    uint32_t instanceId,
    uint32_t assetId)
{
    QueryResult result = CharacterDatabase.Query(
        "SELECT COUNT(*) FROM guildhouse_spawn "
        "WHERE instanceId={} AND assetId={} AND enabled=1",
        instanceId,
        assetId);


    if (!result)
        return false;


    return result->Fetch()[0].Get<uint32>() > 0;
}



// =====================================================
// Spawn one placed asset
// =====================================================

bool GuildHouseSpawner::SpawnAsset(
    uint32_t guildId,
    uint32_t assetId)
{
    uint32 instanceId =
        sGuildHouseInstanceMgr.GetInstanceId(guildId);


    if (!instanceId)
        return false;



    const GHInstanceRecord* instance =
        sGuildHouseInstanceMgr.GetInstance(instanceId);


    if (!instance)
        return false;



    if (HasExistingSpawn(instanceId, assetId))
        return true;



    QueryResult result = CharacterDatabase.Query(
        "SELECT catalogId,status,positionX,positionY,positionZ,orientation "
        "FROM guildhouse_asset "
        "WHERE guildId={} AND assetId={}",
        guildId,
        assetId);


    if (!result)
        return false;



    Field* fields = result->Fetch();


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



    const GHLocation* location =
        sGuildHouseMgr.GetGuildLocation(guildId);



    if (!location)
        return false;



    if (baseX < location->MinX ||
        baseX > location->MaxX ||
        baseY < location->MinY ||
        baseY > location->MaxY)
    {
        LOG_ERROR(
            "module",
            "GuildHouse asset {} outside guild boundary.",
            assetId);

        return false;
    }



    const GHCatalog* catalog =
        sGuildHouseCatalogMgr.GetCatalog(catalogId);



    if (!catalog)
        return false;



    for (const GHCatalogAsset& component : catalog->Components)
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
                instanceId,
                assetId,
                instance->MapId,
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
                instanceId,
                assetId,
                instance->MapId,
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
// =====================================================

bool GuildHouseSpawner::SpawnCreature(
    uint32_t guildId,
    uint32_t instanceId,
    uint32_t assetId,
    uint32_t mapId,
    uint32_t entry,
    float x,
    float y,
    float z,
    float o)
{
    ObjectGuid::LowType spawnGuid =
        sObjectMgr->GenerateCreatureSpawnId();



    std::ostringstream sql;

    sql <<
        "INSERT INTO creature "
        "(guid,id,map,position_x,position_y,position_z,orientation) VALUES ("
        << spawnGuid << ","
        << entry << ","
        << mapId << ","
        << x << ","
        << y << ","
        << z << ","
        << o << ")";


    WorldDatabase.Execute(
        sql.str());



    CharacterDatabase.Execute(
        "INSERT INTO guildhouse_spawn "
        "(assetId,guildId,instanceId,spawnGuid,spawnType,mapId,positionX,positionY,positionZ,orientation) "
        "VALUES ({},{},{},{},{},{},{},{},{},{})",
        assetId,
        guildId,
        instanceId,
        spawnGuid,
        0,
        mapId,
        x,
        y,
        z,
        o);



    return true;
}



// =====================================================
// Spawn GameObject
// =====================================================

bool GuildHouseSpawner::SpawnGameObject(
    uint32_t guildId,
    uint32_t instanceId,
    uint32_t assetId,
    uint32_t mapId,
    uint32_t entry,
    float x,
    float y,
    float z,
    float o)
{
    ObjectGuid::LowType spawnGuid =
        sObjectMgr->GenerateGameObjectSpawnId();



    std::ostringstream sql;

    sql <<
        "INSERT INTO gameobject "
        "(guid,id,map,position_x,position_y,position_z,orientation) VALUES ("
        << spawnGuid << ","
        << entry << ","
        << mapId << ","
        << x << ","
        << y << ","
        << z << ","
        << o << ")";


    WorldDatabase.Execute(
        sql.str());



    CharacterDatabase.Execute(
        "INSERT INTO guildhouse_spawn "
        "(assetId,guildId,instanceId,spawnGuid,spawnType,mapId,positionX,positionY,positionZ,orientation) "
        "VALUES ({},{},{},{},{},{},{},{},{},{})",
        assetId,
        guildId,
        instanceId,
        spawnGuid,
        1,
        mapId,
        x,
        y,
        z,
        o);



    return true;
}



// =====================================================
// Remove Asset
// =====================================================

bool GuildHouseSpawner::RemoveAsset(
    uint32_t guildId,
    uint32_t instanceId,
    uint32_t assetId)
{
    QueryResult result = CharacterDatabase.Query(
        "SELECT spawnGuid,spawnType "
        "FROM guildhouse_spawn "
        "WHERE guildId={} AND instanceId={} AND assetId={} AND enabled=1",
        guildId,
        instanceId,
        assetId);



    if (!result)
        return false;



    do
    {
        Field* fields = result->Fetch();


        uint32 spawnGuid =
            fields[0].Get<uint32>();


        uint8 spawnType =
            fields[1].Get<uint8>();



        if (spawnType == 0)
            WorldDatabase.Execute(
                "DELETE FROM creature WHERE guid={}",
                spawnGuid);

        else
            WorldDatabase.Execute(
                "DELETE FROM gameobject WHERE guid={}",
                spawnGuid);



    } while(result->NextRow());



    CharacterDatabase.Execute(
        "UPDATE guildhouse_spawn "
        "SET enabled=0 "
        "WHERE guildId={} AND instanceId={} AND assetId={}",
        guildId,
        instanceId,
        assetId);



    return true;
}



// =====================================================
// Remove all guild assets
// =====================================================

bool GuildHouseSpawner::RemoveAllAssets(
    uint32_t guildId,
    uint32_t instanceId)
{
    QueryResult result = CharacterDatabase.Query(
        "SELECT assetId "
        "FROM guildhouse_spawn "
        "WHERE guildId={} AND instanceId={} AND enabled=1",
        guildId,
        instanceId);



    if (!result)
        return false;



    do
    {
        uint32 assetId =
            result->Fetch()[0].Get<uint32>();


        RemoveAsset(
            guildId,
            instanceId,
            assetId);


    } while(result->NextRow());



    return true;
}
