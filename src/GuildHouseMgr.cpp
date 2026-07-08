#include "GuildHouseMgr.h"

#include "DatabaseEnv.h"
#include "QueryResult.h"

#include "GuildHouseDefines.h"
#include "GuildHouseCatalogMgr.h"
#include "GuildHouseSpawner.h"

#include "Player.h"
#include "Chat.h"
#include "Log.h"

GuildHouseMgr&
GuildHouseMgr::Instance()
{
    static GuildHouseMgr instance;

    return instance;
}

void GuildHouseMgr::Load()
{
    _houses.clear();

    //
    // Load guild ownership
    //
    if (QueryResult result =
        CharacterDatabase.Query(
            "SELECT guildId, ownerGuid "
            "FROM guildhouse"))
    {
        do
        {
            Field* fields =
                result->Fetch();

            uint32_t guildId =
                fields[0].Get<uint32_t>();

            uint32_t ownerGuid =
                fields[1].Get<uint32_t>();

            GHGuildHouse house;

            house.GuildId =
                guildId;

            house.OwnerGuid =
                ownerGuid;

            house.Phase =
                GetPhase(guildId);

            _houses.emplace(
                guildId,
                house);


        } while(result->NextRow());
    }

    //
    // Load permanent placed assets
    //
    if (QueryResult result =
        CharacterDatabase.Query(
            "SELECT assetId, guildId, catalogId, "
            "status, phase, "
            "positionX, positionY, positionZ, orientation "
            "FROM guildhouse_asset"))
    {
        do
        {
            Field* fields =
                result->Fetch();

            uint32_t guildId =
                fields[1].Get<uint32_t>();

            auto itr =
                _houses.find(guildId);

            if (itr == _houses.end())
                continue;

            GHGuildAsset asset;

            asset.AssetId =
                fields[0].Get<uint32_t>();

            asset.GuildId =
                guildId;

            asset.CatalogId =
                fields[2].Get<uint32_t>();

            asset.Status =
                static_cast<GHAssetStatus>(
                    fields[3].Get<uint8_t>());

            asset.Phase =
                fields[4].Get<uint32_t>();

            asset.X =
                fields[5].Get<float>();

            asset.Y =
                fields[6].Get<float>();

            asset.Z =
                fields[7].Get<float>();

            asset.O =
                fields[8].Get<float>();

            itr->second.Assets.push_back(asset);

        } while(result->NextRow());
    }

    LOG_INFO(
        "module",
        "GuildHouseMgr loaded {} guild houses",
        _houses.size());
}

bool GuildHouseMgr::HasGuildHouse(
    uint32_t guildId) const
{
    return
        _houses.find(guildId)
        != _houses.end();
}

uint32_t GuildHouseMgr::GetPhase(
    uint32_t guildId) const
{
    return guildId + GH_PHASE_OFFSET;
}

const GHGuildHouse*
GuildHouseMgr::GetGuildHouse(
    uint32_t guildId) const
{

    auto itr =
        _houses.find(guildId);

    if (itr == _houses.end())
        return nullptr;

    return &itr->second;
}

bool GuildHouseMgr::CreateGuildHouse(
    uint32_t guildId,
    uint32_t ownerGuid)
{
    if (HasGuildHouse(guildId))
        return false;

    GHGuildHouse house;

    house.GuildId =
        guildId;

    house.OwnerGuid =
        ownerGuid;

    house.Phase =
        GetPhase(guildId);

    _houses.emplace(
        guildId,
        house);

    std::ostringstream sql;

    sql <<
    "INSERT INTO guildhouse "
    "(guildId, ownerGuid) VALUES ("
    << guildId
    << ","
    << ownerGuid
    << ")";

    CharacterDatabase.Execute(
        sql.str());

    return true;
}

bool GuildHouseMgr::PurchaseCatalogItem(
    Player* player,
    uint32_t catalogId)
{
    if (!player)
        return false;

    uint32 guildId =
        player->GetGuildId();

    if (!guildId)
    {
        ChatHandler(player->GetSession())
            .PSendSysMessage(
                "You are not in a guild.");

        return false;
    }

    if (!HasGuildHouse(guildId))
    {
        ChatHandler(player->GetSession())
            .PSendSysMessage(
                "Your guild does not own a Guild House.");

        return false;
    }

    //
    // Only Guild Master can purchase
    //
    if (!GuildHouseUtil::IsGuildMaster(player))
    {
        ChatHandler(player->GetSession())
            .PSendSysMessage(
                "Only the Guild Master may purchase Guild House items.");

        return false;
    }

    const GHCatalog* catalog =
        sGuildHouseCatalogMgr.GetCatalog(
            catalogId);

    if (!catalog || !catalog->Enabled)
    {
        ChatHandler(player->GetSession())
            .PSendSysMessage(
                "Invalid Guild House item.");

        return false;
    }

    uint32 phase =
        GetPhase(guildId);

    //
    // Purchased but NOT placed.
    //
    // Position values remain 0 until .gh place
    //
    std::ostringstream ss;

    ss <<
    "INSERT INTO guildhouse_asset "
    "(guildId, layoutId, catalogId, status, phase,"
    "positionX, positionY, positionZ, orientation, createdBy)"
    " VALUES ("
    << guildId << ","
    << 1 << ","
    << catalogId << ","
    << GH_ASSET_PURCHASED << ","
    << phase << ","
    << 0 << ","
    << 0 << ","
    << 0 << ","
    << 0 << ","
    << player->GetGUID().GetCounter()
    << ")";

    CharacterDatabase.Execute(
        ss.str());

    QueryResult result =
        CharacterDatabase.Query(
            "SELECT LAST_INSERT_ID()");

    if (!result)
        return false;

    uint32 assetId =
        result->Fetch()[0]
        .Get<uint32>();

    ChatHandler(player->GetSession())
        .PSendSysMessage(
            "Guild House item purchased. Asset ID: %u. Use .gh place %u to place it.",
            assetId,
            assetId);

    return true;
}

bool GuildHouseMgr::PlaceAsset(
    Player* player,
    uint32_t assetId)
{
    if (!player)
        return false;

    if (!GuildHouseUtil::IsGuildMaster(player))
        return false;

    uint32 guildId =
        player->GetGuildId();

    if (!guildId)
        return false;

    if (!GuildHouseUtil::IsOnGMIsland(player))
    {
        ChatHandler(player->GetSession())
            .PSendSysMessage(
                "Guild House items must be placed on GM Island.");

        return false;
    }

    // Load asset from database

    QueryResult result =
        CharacterDatabase.Query(
            "SELECT catalogId,status "
            "FROM guildhouse_asset "
            "WHERE assetId=%u "
            "AND guildId=%u",
            assetId,
            guildId);

    if (!result)
        return false;

    Field* f =
        result->Fetch();

    uint32 catalogId =
        f[0].Get<uint32>();

    uint8 status =
        f[1].Get<uint8>();

    if (status != GH_ASSET_PURCHASED)
    {
        ChatHandler(player->GetSession())
            .PSendSysMessage(
                "This item is already placed.");

        return false;
    }

    //
    // Update location
    //

    CharacterDatabase.Execute(
        "UPDATE guildhouse_asset SET "
        "status=%u,"
        "phase=%u,"
        "positionX=%f,"
        "positionY=%f,"
        "positionZ=%f,"
        "orientation=%f "
        "WHERE assetId=%u",
        GH_ASSET_PLACED,
        GuildHouseUtil::GetGuildHousePhase(guildId),
        player->GetPositionX(),
        player->GetPositionY(),
        player->GetPositionZ(),
        player->GetOrientation(),
        assetId);

    sGuildHouseSpawner.SpawnAsset(
        guildId,
        assetId);

    ChatHandler(player->GetSession())
        .PSendSysMessage(
            "Placed Guild House asset ID %u",
            assetId);

    return true;
}

bool GuildHouseMgr::HasSalesman(uint32_t guildId) const
{
    std::ostringstream ss;

    ss <<
    "SELECT COUNT(*) "
    "FROM guildhouse_salesman "
    "WHERE guildId="
    << guildId;

    QueryResult result =
        CharacterDatabase.Query(ss.str());

    if (!result)
        return false;

    return result->Fetch()[0]
        .Get<uint32_t>() > 0;
}

void GuildHouseMgr::RecordSalesmanSpawn(
    uint32_t guildId,
    uint32_t spawnId,
    uint32_t mapId,
    uint32_t phase,
    float x,
    float y,
    float z,
    float o)
{
    std::ostringstream ss;

    ss <<
    "INSERT INTO guildhouse_salesman "
    "(guildId,guid,mapId,phase,"
    "positionX,positionY,positionZ,orientation)"
    " VALUES ("
    << guildId << ","
    << spawnId << ","
    << mapId << ","
    << phase << ","
    << x << ","
    << y << ","
    << z << ","
    << o
    << ")";

    CharacterDatabase.Execute(ss.str());
}

const GHGuildAsset* GuildHouseMgr::GetAsset(
    uint32_t guildId,
    uint32_t assetId) const
{
    auto houseItr = _houses.find(guildId);

    if (houseItr == _houses.end())
        return nullptr;

    for (const GHGuildAsset& asset : houseItr->second.Assets)
    {
        if (asset.AssetId == assetId)
            return &asset;
    }

    return nullptr;
}

GHGuildAsset* GuildHouseMgr::GetAsset(
    uint32_t guildId,
    uint32_t assetId)
{
    auto houseItr = _houses.find(guildId);

    if (houseItr == _houses.end())
        return nullptr;

    for (GHGuildAsset& asset : houseItr->second.Assets)
    {
        if (asset.AssetId == assetId)
            return &asset;
    }

    return nullptr;
}

bool GuildHouseMgr::PlaceAsset(
    Player* player,
    uint32 assetId)
{
    if (!player)
        return false;

    //
    // Must be on GM Island, inside guild phase
    //
    if (!GuildHouseUtil::IsOnGMIsland(player))
    {
        ChatHandler(player->GetSession())
            .PSendSysMessage(
                "You must be inside your Guild House to place items.");

        return false;
    }

    //
    // Guild Master only
    //
    if (!GuildHouseUtil::IsGuildMaster(player))
    {
        ChatHandler(player->GetSession())
            .PSendSysMessage(
                "Only the Guild Master may place Guild House items.");

        return false;
    }

    uint32 guildId =
        player->GetGuildId();

    if (!HasGuildHouse(guildId))
        return false;

    GHGuildAsset* asset =
        GetAsset(
            guildId,
            assetId);

    if (!asset)
    {
        ChatHandler(player->GetSession())
            .PSendSysMessage(
                "Unknown asset.");

        return false;
    }

    if (asset->Status != GH_ASSET_PURCHASED)
    {
        ChatHandler(player->GetSession())
            .PSendSysMessage(
                "That item has already been placed.");

        return false;
    }

    asset->Status = GH_ASSET_PLACED;

    asset->X = player->GetPositionX();
    asset->Y = player->GetPositionY();
    asset->Z = player->GetPositionZ();
    asset->O = player->GetOrientation();

    std::ostringstream ss;

    ss <<
    "UPDATE guildhouse_asset "
    "SET "
    "status=" << GH_ASSET_PLACED << ","
    "positionX=" << asset->X << ","
    "positionY=" << asset->Y << ","
    "positionZ=" << asset->Z << ","
    "orientation=" << asset->O
    << " WHERE assetId=" << assetId;

    CharacterDatabase.Execute(ss.str());

    sGuildHouseSpawner.SpawnAsset(
        guildId,
        assetId);

    return true;
}
