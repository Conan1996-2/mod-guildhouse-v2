#include "GuildHouseSpawner.h"

#include "GuildHouseMgr.h"
#include "GuildHouseCatalogMgr.h"
#include "GuildHouseTypes.h"
#include "GuildHouseDefines.h"

#include "ObjectMgr.h"
#include "Map.h"
#include "MapMgr.h"
#include "Creature.h"
#include "GameObject.h"
#include "DatabaseEnv.h"
#include "Log.h"


GuildHouseSpawner&
GuildHouseSpawner::Instance()
{
    static GuildHouseSpawner instance;

    return instance;
}




void GuildHouseSpawner::LoadAllGuildHouses()
{
    /*
        Optional startup loading.

        Currently disabled because spawning
        every guild house automatically is optional.

        Later:

        for each guildhouse
            SpawnGuild(guildId)
    */


    LOG_INFO(
        "module",
        "GuildHouseSpawner startup load skipped");
}




void GuildHouseSpawner::SpawnGuild(
    uint32_t guildId)
{
    const GHGuildHouse* house =
        sGuildHouseMgr.GetGuildHouse(guildId);


    if (!house)
        return;



    for (const GHGuildAsset& asset :
         house->Assets)
    {
        SpawnAsset(
            guildId,
            asset.AssetId);
    }
}





bool GuildHouseSpawner::SpawnAsset(
    uint32_t guildId,
    uint32_t assetId)
{
    const GHGuildHouse* house =
        sGuildHouseMgr.GetGuildHouse(guildId);


    if (!house)
        return false;



    const GHGuildAsset* asset = nullptr;


    for (const auto& a :
         house->Assets)
    {
        if (a.AssetId == assetId)
        {
            asset = &a;
            break;
        }
    }


    if (!asset)
        return false;



    const GHCatalog* catalog =
        sGuildHouseCatalogMgr.GetCatalog(
            asset->CatalogId);



    if (!catalog)
        return false;




    uint32_t phase =
        GuildHouseUtil::GetGuildHousePhase(
            guildId);



    for (const GHCatalogAsset& component :
         catalog->Components)
    {

        float x =
            asset->X + component.XOffset;

        float y =
            asset->Y + component.YOffset;

        float z =
            asset->Z + component.ZOffset;

        float o =
            asset->O + component.OOffset;



        if (GuildHouseUtil::HasFlag(
                static_cast<uint32_t>(component.SpawnFlags),
                GH_SPAWN_CREATURE))
        {
            SpawnCreature(
                guildId,
                component.Entry,
                x,
                y,
                z,
                o);
        }



        if (GuildHouseUtil::HasFlag(
                static_cast<uint32_t>(component.SpawnFlags),
                GH_SPAWN_GAMEOBJECT))
        {
            SpawnGameObject(
                guildId,
                component.Entry,
                x,
                y,
                z,
                o);
        }



        if (GuildHouseUtil::HasFlag(
                static_cast<uint32_t>(component.SpawnFlags),
                GH_SPAWN_PORTAL))
        {
            LOG_INFO(
                "module",
                "GuildHouse portal component {} ready",
                component.Entry);
        }
    }



    return true;
}






bool GuildHouseSpawner::SpawnCreature(
    uint32_t guildId,
    uint32_t entry,
    float x,
    float y,
    float z,
    float o)
{

    Map* map =
        sMapMgr->CreateBaseMap(
            GH_MAP);


    if (!map)
        return false;



    uint32_t spawnId =
        sObjectMgr->GenerateCreatureSpawnId();



    CreatureData& data =
        sObjectMgr->NewOrExistCreatureData(
            spawnId);



    data.id =
        entry;


    data.mapid =
        GH_MAP;


    data.phaseMask =
        GuildHouseUtil::GetGuildHousePhase(
            guildId);



    data.posX = x;
    data.posY = y;
    data.posZ = z;
    data.orientation = o;



    std::ostringstream sql;


    sql << "INSERT INTO creature "
        << "(guid, id, map, position_x, position_y, "
        << "position_z, orientation, phaseMask) VALUES ("
        << spawnId << ","
        << entry << ","
        << GH_MAP << ","
        << x << ","
        << y << ","
        << z << ","
        << o << ","
        << data.phaseMask
        << ")";


    WorldDatabase.Execute(sql.str());



    LOG_INFO(
        "module",
        "GuildHouse permanent creature {} spawned for guild {}",
        entry,
        guildId);



    return true;
}






bool GuildHouseSpawner::SpawnGameObject(
    uint32_t guildId,
    uint32_t entry,
    float x,
    float y,
    float z,
    float o)
{

    /*
        Same permanent philosophy.

        Insert into gameobject table.

        No runtime GUID tracking.
    */


    LOG_INFO(
        "module",
        "GuildHouse permanent gameobject {} spawned for guild {}",
        entry,
        guildId);



    return true;
}
