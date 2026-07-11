#include "GuildHouseMgr.h"

#include "DatabaseEnv.h"
#include "QueryResult.h"

#include "Player.h"
#include "Chat.h"
#include "Log.h"
#include "ObjectMgr.h"

#include "GuildHouseDefines.h"
#include "GuildHouseCatalogMgr.h"
#include "GuildHouseSpawner.h"

#include <sstream>

GuildHouseMgr& GuildHouseMgr::Instance()
{
    static GuildHouseMgr instance;

    return instance;
}

bool GuildHouseMgr::IsGuildInstance(uint32 guildId, uint32 instanceId) const
{
    auto itr = _guildInstances.find(guildId);
    if (itr == _guildInstances.end())
        return false;

    return itr->second == instanceId;
}

uint32 GuildHouseMgr::GetGuildInstance(uint32 guildId) const
{
    auto itr = _guildInstances.find(guildId);
    if (itr == _guildInstances.end())
        return 0;

    return itr->second;
}

void GuildHouseMgr::SetGuildInstance(uint32 guildId, uint32 instanceId)
{
    _guildInstances[guildId] = instanceId;
}

void GuildHouseMgr::RemoveGuildInstance(uint32 guildId)
{
    _guildInstances.erase(guildId);
}

bool GuildHouseUtil::IsGuildHouseInstance(uint32 guildId, uint32 instanceId)
{
    return sGuildHouseMgr.IsGuildInstance(guildId, instanceId);
}

void GuildHouseMgr::Load()
{
    _houses.clear();
    _guildInstances.clear();

    //
    // Load guild instance ownership
    //
    if (QueryResult result = CharacterDatabase.Query("SELECT guildId, instanceId FROM guildhouse_instance"))
    {
        do
        {
            Field* fields = result->Fetch();    
            uint32 guildId = fields[0].Get<uint32>();
            _guildInstances[guildId] = fields[1].Get<uint32>();
        } while (result->NextRow());
    }
    
    //
    // Load guild ownership
    //
    if (QueryResult result = CharacterDatabase.Query("SELECT guildId, ownerGuid FROM guildhouse"))
    {
        do
        {
            Field* fields = result->Fetch();
            uint32_t guildId = fields[0].Get<uint32_t>();
            uint32_t ownerGuid = fields[1].Get<uint32_t>();

            GHGuildHouse house;
            house.GuildId = guildId;
            house.OwnerGuid = ownerGuid;
            house.InstanceId = GetGuildInstance(guildId);
            _houses.emplace(guildId, house);

        } while(result->NextRow());
    }

    //
    // Load permanent placed assets
    //
    if (QueryResult result = CharacterDatabase.Query("SELECT assetId, guildId, catalogId, status, positionX, positionY, positionZ, orientation FROM guildhouse_asset"))
    {
        do
        {
            Field* fields = result->Fetch();
            uint32_t guildId = fields[1].Get<uint32_t>();
            auto itr = _houses.find(guildId);

            if (itr == _houses.end())
                continue;

            GHGuildAsset asset;
            asset.AssetId = fields[0].Get<uint32_t>();
            asset.GuildId = guildId;
            asset.CatalogId = fields[2].Get<uint32_t>();
            asset.Status = static_cast<GHAssetStatus>(fields[3].Get<uint8_t>());

            asset.X = fields[4].Get<float>();
            asset.Y = fields[5].Get<float>();
            asset.Z = fields[6].Get<float>();
            asset.O = fields[7].Get<float>();

            itr->second.Assets.push_back(asset);

        } while(result->NextRow());
    }

    LOG_INFO("module", "GuildHouseMgr loaded {} guild houses", _houses.size());
}

bool GuildHouseMgr::HasGuildHouse(uint32_t guildId) const
{
    return _houses.find(guildId) != _houses.end();
}

const GHGuildHouse* GuildHouseMgr::GetGuildHouse(uint32_t guildId) const
{
    auto itr = _houses.find(guildId);

    if (itr == _houses.end())
        return nullptr;

    return &itr->second;
}

bool GuildHouseMgr::CreateGuildHouse(uint32_t guildId, uint32_t ownerGuid)
{
    if (HasGuildHouse(guildId))
        return false;

    GHGuildHouse house;
    house.GuildId = guildId;
    house.OwnerGuid = ownerGuid;
    house.InstanceId = 0;

    _houses.emplace(guildId, house);

    std::ostringstream sql;
    sql << "INSERT INTO guildhouse (guildId, ownerGuid) VALUES (" << guildId << "," << ownerGuid << ")";
    CharacterDatabase.Execute(sql.str());

    return true;
}

bool GuildHouseMgr::PurchaseCatalogItem(Player* player, uint32_t catalogId)
{
    if (!player)
        return false;

    uint32 guildId = player->GetGuildId();

    if (!guildId)
    {
        ChatHandler(player->GetSession()).PSendSysMessage("You are not in a guild.");
        return false;
    }

    if (!HasGuildHouse(guildId))
    {
        ChatHandler(player->GetSession()).PSendSysMessage("Your guild does not own a Guild House.");
        return false;
    }

    //
    // Only Guild Master can purchase
    //
    if (!GuildHouseUtil::IsGuildMaster(player))
    {
        ChatHandler(player->GetSession()).PSendSysMessage("Only the Guild Master may purchase Guild House items.");
        return false;
    }

    const GHCatalog* catalog = sGuildHouseCatalogMgr.GetCatalog(catalogId);
    if (!catalog || !catalog->Enabled)
    {
        ChatHandler(player->GetSession()).PSendSysMessage("Invalid Guild House item.");
        return false;
    }

    //
    // Purchased but NOT placed.
    //
    // Position values remain 0 until .gh place
    //
    std::ostringstream ss;
    ss << "INSERT INTO guildhouse_asset (guildId, layoutId, catalogId, status, positionX, positionY, positionZ, orientation, createdBy)"
    " VALUES (" << guildId << "," << 1 << "," << catalogId << "," << GH_ASSET_PURCHASED << "," << 0 << "," << 0 << "," << 0 << "," << 0 << "," << player->GetGUID().GetCounter() << ")";
    CharacterDatabase.Execute(ss.str());
    QueryResult result = CharacterDatabase.Query("SELECT LAST_INSERT_ID()");
    if (!result)
        return false;

    uint32 assetId = result->Fetch()[0].Get<uint32>();

    ChatHandler(player->GetSession()).PSendSysMessage("Guild House item purchased. Asset ID: {}. Use .gh place {} to place it.", assetId, assetId);

    return true;
}

bool GuildHouseMgr::HasSalesman(uint32_t guildId) const
{
    std::ostringstream ss;
    ss << "SELECT COUNT(*) FROM guildhouse_salesman WHERE guildId=" << guildId;
    QueryResult result = CharacterDatabase.Query(ss.str());
    if (!result)
        return false;

    return result->Fetch()[0].Get<uint32_t>() > 0;
}

void GuildHouseMgr::RecordSalesmanSpawn(uint32_t guildId, uint32_t spawnId, uint32_t mapId, uint32_t instanceId, float x, float y, float z, float o)
{
    std::ostringstream ss;
    ss << "INSERT INTO guildhouse_salesman (guildId,guid,mapId,instanceId,positionX,positionY,positionZ,orientation)"
    " VALUES (" << guildId << "," << spawnId << "," << mapId << "," << instanceId << "," << x << "," << y << "," << z << "," << o << ")";
    CharacterDatabase.Execute(ss.str());
}

bool GuildHouseMgr::CreatePermanentSalesman(Player* player, uint32 entry)
{
    if (!player)
        return false;

    uint32 guildId = player->GetGuildId();

    if (!guildId)
        return false;

    if (HasSalesman(guildId))
    {
        ChatHandler(player->GetSession()).PSendSysMessage("Your guild already has a Guild House salesman.");
        return false;
    }

    if (!GuildHouseUtil::CanManageGuildHouse(player))
    {
        ChatHandler(player->GetSession()).PSendSysMessage("Only someone with the proper rank can place the Salesman on the Island.");
        return false;
    }

    uint32 instanceId = player->GetInstanceId();

    //
    // Create permanent creature spawn
    //

    std::ostringstream sql;
    sql << "INSERT INTO creature (id,map,position_x,position_y,position_z,orientation,spawntimesecs) VALUES ("
    << entry << "," << player->GetMapId() << "," << player->GetPositionX() << "," << player->GetPositionY() << "," << player->GetPositionZ() << "," << player->GetOrientation() << "," << 300 << ")";
    WorldDatabase.Execute(sql.str());

    //
    // Get generated creature GUID
    //

    QueryResult result = WorldDatabase.Query("SELECT MAX(guid) FROM creature");

    if (!result)
        return false;

    uint32 spawnGuid = result->Fetch()[0].Get<uint32>();

    //
    // Store guild ownership
    //

    RecordSalesmanSpawn(guildId, spawnGuid, player->GetMapId(), instanceId, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation());

    LOG_INFO("module", "GuildHouse salesman spawned. Guild {}, Spawn {}, Entry {}", guildId, spawnGuid, entry);

    return true;
}

const GHGuildAsset* GuildHouseMgr::GetAsset(uint32_t guildId, uint32_t assetId) const
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

GHGuildAsset* GuildHouseMgr::GetAsset(uint32_t guildId, uint32_t assetId)
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

bool GuildHouseMgr::PlaceAsset(Player* player, uint32 assetId)
{
    if (!player)
        return false;

    //
    // Must be GM and inside guild instance
    //
    if (!GuildHouseUtil::CanManageGuildHouse(player))
    {
        ChatHandler(player->GetSession()).PSendSysMessage("Only someone with the proper rank can place the asset on the Island.");
        return false;
    }

    uint32 guildId = player->GetGuildId();
    if (!HasGuildHouse(guildId))
        return false;

    GHGuildAsset* asset = GetAsset(guildId, assetId);
    if (!asset)
    {
        ChatHandler(player->GetSession()).PSendSysMessage("Unknown asset.");
        return false;
    }

    if (asset->Status != GH_ASSET_PURCHASED)
    {
        ChatHandler(player->GetSession()).PSendSysMessage("That item has already been placed.");
        return false;
    }

    asset->Status = GH_ASSET_PLACED;
    asset->X = player->GetPositionX();
    asset->Y = player->GetPositionY();
    asset->Z = player->GetPositionZ();
    asset->O = player->GetOrientation();

    std::ostringstream ss;
    ss << "UPDATE guildhouse_asset SET status=" << GH_ASSET_PLACED << ",positionX=" << asset->X << ",positionY=" << asset->Y << ",positionZ=" << asset->Z << ",orientation=" << asset->O << " WHERE assetId=" << assetId;
    CharacterDatabase.Execute(ss.str());

    sGuildHouseSpawner.SpawnAsset(guildId, assetId);

    return true;
}

bool GuildHouseMgr::MoveAsset(Player* player, uint32 assetId)
{
    if (!player)
        return false;

    if (!GuildHouseUtil::CanManageGuildHouse(player))
    {
        ChatHandler(player->GetSession()).PSendSysMessage("Only someone with the proper rank can move the asset on the Island.");
        return false;
    }

    uint32 guildId = player->GetGuildId();
    GHGuildAsset* asset = GetAsset(guildId, assetId);

    if (!asset)
    {
        ChatHandler(player->GetSession()).PSendSysMessage("Unknown asset.");
        return false;
    }

    if (asset->Status != GH_ASSET_PLACED)
    {
        ChatHandler(player->GetSession()).PSendSysMessage("Only placed assets can be moved.");
        return false;
    }

    float oldX = asset->X;
    float oldY = asset->Y;
    float oldZ = asset->Z;

    float newX = player->GetPositionX();
    float newY = player->GetPositionY();
    float newZ = player->GetPositionZ();
    float newO = player->GetOrientation();

    float deltaX = newX - oldX;
    float deltaY = newY - oldY;
    float deltaZ = newZ - oldZ;

    //
    // Update asset anchor
    //
    asset->X = newX;
    asset->Y = newY;
    asset->Z = newZ;
    asset->O = newO;

    CharacterDatabase.Execute("UPDATE guildhouse_asset SET positionX=%f,positionY=%f,positionZ=%f,orientation=%f WHERE assetId=%u AND guildId=%u", newX, newY, newZ, newO, assetId, guildId);

    //
    // Move permanent spawn records
    //
    uint32 instanceId = player->GetInstanceId();
    QueryResult result = CharacterDatabase.Query("SELECT spawnGuid,spawnType FROM guildhouse_spawn WHERE guildId=%u AND assetId=%u AND instanceId=%u AND enabled=1", guildId, assetId, instanceId);
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 spawnGuid = fields[0].Get<uint32>();
            uint8 spawnType = fields[1].Get<uint8>();

            if (spawnType == 0)
                WorldDatabase.Execute("UPDATE creature SET position_x=position_x+%f,position_y=position_y+%f,position_z=position_z+%f,orientation=%f WHERE guid=%u", deltaX, deltaY, deltaZ, newO, spawnGuid);
            else
                WorldDatabase.Execute("UPDATE gameobject SET position_x=position_x+%f,position_y=position_y+%f,position_z=position_z+%f,orientation=%f WHERE guid=%u", deltaX, deltaY, deltaZ, newO, spawnGuid);

        } while(result->NextRow());
    }

    //
    // Update tracking table
    //

    CharacterDatabase.Execute("UPDATE guildhouse_spawn SET positionX=positionX+%f,positionY=positionY+%f,positionZ=positionZ+%f WHERE guildId=%u AND assetId=%u AND instanceId=%u", deltaX, deltaY, deltaZ, guildId, assetId, instanceId);

    return true;
}

bool GuildHouseMgr::StoreAsset(Player* player, uint32 assetId)
{
    if (!player)
        return false;

    if (!GuildHouseUtil::IsGuildMaster(player))
        return false;

    uint32 guildId = player->GetGuildId();

    if (!sGuildHouseSpawner.RemoveAsset(guildId, player->GetInstanceId(), assetId))
        return false;

    GHGuildAsset* asset = GetAsset(guildId, assetId);

    if (!asset)
        return false;

    asset->Status = GH_ASSET_STORED;

    CharacterDatabase.Execute("UPDATE guildhouse_asset SET status=%u WHERE assetId=%u", GH_ASSET_STORED, assetId);

    return true;
}

bool GuildHouseMgr::SellAsset(Player* player, uint32 assetId)
{
    if (!player)
        return false;

    if (!GuildHouseUtil::IsGuildMaster(player))
        return false;

    uint32 guildId = player->GetGuildId();
    GHGuildAsset* asset = GetAsset(guildId, assetId);

    if (!asset)
        return false;

    if (asset->Status == GH_ASSET_PLACED)
        sGuildHouseSpawner.RemoveAsset(guildId, player->GetInstanceId(), assetId);

    CharacterDatabase.Execute("DELETE FROM guildhouse_asset WHERE assetId=%u AND guildId=%u", assetId, guildId);

    return true;
}

const GHLocation* GuildHouseMgr::GetLocation(uint32_t locationId) const
{
    auto itr = _locations.find(locationId);

    if (itr == _locations.end())
        return nullptr;

    return &itr->second;
}

std::vector<const GHLocation*> GuildHouseMgr::GetLocations() const
{
    std::vector<const GHLocation*> locations;

    for (auto const& itr : _locations)
    {
        if (itr.second.Enabled)
            locations.push_back(&itr.second);
    }

    return locations;
}
