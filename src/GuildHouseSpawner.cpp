#include "GuildHouseSpawner.h"

#include "GuildHouseMgr.h"
#include "GuildHouseCatalogMgr.h"
#include "GuildHouseDefines.h"
#include "GuildHouseTypes.h"

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
        Optional future feature.

        If enabled later:

        for every guildhouse_asset
            SpawnAsset()

        Currently disabled because
        purchases spawn immediately.
    */

    LOG_INFO(
        "module",
        "GuildHouseSpawner startup loading disabled");
}





void GuildHouseSpawner::SpawnGuild(
    uint32_t guildId)
{
    const GHGuildHouse* house =
        sGuildHouseMgr.GetGuildHouse(guildId);


    if (!house)
        return;



    for (const GHGuildAsset& asset : house->Assets)
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


    for (const auto& a : house->Assets)
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
                component.SpawnFlags,
                GH_SPAWN_CREATURE))
        {
            CreatePermanentCreature(
                guildId,
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
            CreatePermanentGameObject(
                guildId,
                component.Entry,
                x,
                y,
                z,
                o);
        }
    }



    return true;
}









bool GuildHouseSpawner::CreatePermanentCreature(
    uint32_t guildId,
    uint32_t entry,
    float x,
    float y,
    float z,
    float o)
{

    uint32_t phase =
        GuildHouseUtil::GetGuildHousePhase(
            guildId);



    std::ostringstream sql;


    sql <<
    "INSERT INTO creature "
    "(id,map,spawnMask,phaseMask,"
    "position_x,position_y,position_z,orientation,"
    "spawntimesecs,MovementType,Comment) VALUES ("

    << entry << ","
    << GH_MAP << ","
    << 1 << ","
    << phase << ","

    << x << ","
    << y << ","
    << z << ","
    << o << ","

    << 300 << ","
    << 0 << ","

    << "'GuildHouse permanent spawn')";



    WorldDatabase.Execute(
        sql.str());



    LOG_INFO(
        "module",
        "GuildHouse creature {} created for guild {}",
        entry,
        guildId);



    return true;
}









bool GuildHouseSpawner::CreatePermanentGameObject(
    uint32_t guildId,
    uint32_t entry,
    float x,
    float y,
    float z,
    float o)
{

    uint32_t phase =
        GuildHouseUtil::GetGuildHousePhase(
            guildId);



    std::ostringstream sql;


    sql <<
    "INSERT INTO gameobject "
    "(id,map,spawnMask,phaseMask,"
    "position_x,position_y,position_z,"
    "orientation,spawntimesecs,Comment) VALUES ("

    << entry << ","
    << GH_MAP << ","
    << 1 << ","
    << phase << ","

    << x << ","
    << y << ","
    << z << ","
    << o << ","

    << 300 << ","

    << "'GuildHouse permanent object')";



    WorldDatabase.Execute(
        sql.str());



    LOG_INFO(
        "module",
        "GuildHouse gameobject {} created for guild {}",
        entry,
        guildId);



    return true;
}
