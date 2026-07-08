#include "GuildHouseSpawner.h"

#include "GuildHouseMgr.h"
#include "GuildHouseCatalogMgr.h"
#include "GuildHouseDefines.h"
#include "GuildHouseTypes.h"

#include "DatabaseEnv.h"
#include "MapMgr.h"
#include "Map.h"
#include "ObjectMgr.h"
#include "Creature.h"
#include "GameObject.h"
#include "Log.h"

GuildHouseSpawner& GuildHouseSpawner::Instance()
{
    static GuildHouseSpawner instance;
    return instance;
}

// =====================================================
// Startup loader
//
// Only respawns assets that are already placed.
// Purchased items remain stored.
// =====================================================

void GuildHouseSpawner::LoadPlacedAssets()
{
    if (QueryResult result =
        CharacterDatabase.Query(
            "SELECT guildId, assetId "
            "FROM guildhouse_asset "
            "WHERE status = 1"))
    {
        do
        {
            Field* fields = result->Fetch();

            uint32 guildId =
                fields[0].Get<uint32>();

            uint32 assetId =
                fields[1].Get<uint32>();

            SpawnAsset(
                guildId,
                assetId);

        } while (result->NextRow());
    }

    LOG_INFO(
        "module",
        "GuildHouseSpawner loaded placed assets");
}

// =====================================================
// Spawn one asset
// =====================================================

bool GuildHouseSpawner::SpawnAsset(
    uint32 guildId,
    uint32 assetId)
{
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
// Creature permanent spawn
// =====================================================

bool GuildHouseSpawner::SpawnCreature(
    uint32 guildId,
    uint32 entry,
    float x,
    float y,
    float z,
    float o)
{
    std::ostringstream sql;

    sql <<
    "INSERT INTO creature "
    "(id,map,phaseMask,"
    "position_x,position_y,position_z,"
    "orientation)"
    " VALUES ("
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
        sql.str());

    LOG_INFO(
        "module",
        "GuildHouse creature {} spawned for guild {}",
        entry,
        guildId);

    return true;
}

// =====================================================
// GameObject permanent spawn
// =====================================================

bool GuildHouseSpawner::SpawnGameObject(
    uint32 guildId,
    uint32 entry,
    float x,
    float y,
    float z,
    float o)
{
    std::ostringstream sql;

    sql <<
    "INSERT INTO gameobject "
    "(id,map,phaseMask,"
    "position_x,position_y,position_z,"
    "orientation)"
    " VALUES ("
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
        sql.str());

    LOG_INFO(
        "module",
        "GuildHouse gameobject {} spawned for guild {}",
        entry,
        guildId);

    return true;
}

// =====================================================
// Remove asset
//
// Placeholder for .gh store / sell / move
// =====================================================

bool GuildHouseSpawner::RemoveAsset(
    uint32 guildId,
    uint32 assetId)
{
    //
    // Future:
    //
    // Find creature/gameobject rows
    // belonging to assetId
    // remove them
    //

    LOG_INFO(
        "module",
        "GuildHouse remove requested guild {} asset {}",
        guildId,
        assetId);

    return true;
}
