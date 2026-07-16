#include "ScriptMgr.h"

#include "GuildHouseMgr.h"

#include "GuildHouseInstanceMgr.h"
#include "GuildHouseSpawner.h"
#include "GuildHouseCatalogMgr.h"
#include "GuildHouseDefines.h"

#include "DatabaseEnv.h"
#include "QueryResult.h"

#include "Player.h"
#include "Chat.h"
#include "Log.h"
#include "MapMgr.h"
#include "ObjectMgr.h"

#include <sstream>


GuildHouseMgr& GuildHouseMgr::Instance()
{
    static GuildHouseMgr instance;

    return instance;
}


// =====================================================
// Guild Instance Management
// =====================================================

bool GuildHouseMgr::IsGuildInstance(uint32_t guildId, uint32_t instanceId) const
{
    return sGuildHouseInstanceMgr.IsGuildInstance(guildId, instanceId);
}


uint32_t GuildHouseMgr::GetGuildInstance(uint32_t guildId) const
{
    return sGuildHouseInstanceMgr.GetInstanceId(guildId);
}


uint32_t GuildHouseMgr::GetGuildByInstance(uint32_t instanceId) const
{
    return sGuildHouseInstanceMgr.GetGuildId(instanceId);
}


uint32_t GuildHouseMgr::GetOrCreateGuildInstance(uint32_t guildId)
{
    uint32_t instanceId = sGuildHouseInstanceMgr.GetInstanceId(guildId);

    if (instanceId)
        return instanceId;


    const GHGuildHouse* house = GetGuildHouse(guildId);

    if (!house)
        return 0;


    const GHLocation* location = GetLocation(house->LocationId);

    if (!location)
        return 0;


    instanceId = sGuildHouseInstanceMgr.CreateInstance(guildId, location->MapId);

    if (!instanceId)
    {
        LOG_ERROR("module", "Failed creating Guild House instance for guild {}", guildId);
        return 0;
    }


    auto itr = _houses.find(guildId);

    if (itr != _houses.end())
        itr->second.InstanceId = instanceId;


    return instanceId;
}


bool GuildHouseMgr::EnsureGuildInstanceSave(uint32_t guildId)
{
    uint32_t instanceId = GetOrCreateGuildInstance(guildId);

    if (!instanceId)
        return false;


    return sGuildHouseInstanceMgr.GetInstance(instanceId) != nullptr;
}


bool GuildHouseMgr::BindPlayerToGuildInstance(Player* player)
{
    if (!player)
        return false;

    uint32 guildId = player->GetGuildId();

    if (!guildId)
        return false;

    uint32 instanceId = GetOrCreateGuildInstance(guildId);

    if (!instanceId)
        return false;

    return true;
}


// =====================================================
// Guild House Ownership
// =====================================================

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


const GHLocation* GuildHouseMgr::GetGuildLocation(uint32_t guildId) const
{
    const GHGuildHouse* house = GetGuildHouse(guildId);

    if (!house)
        return nullptr;


    return GetLocation(house->LocationId);
}


bool GuildHouseMgr::CreateGuildHouse(uint32_t guildId, uint32_t ownerGuid, uint32_t locationId)
{
    if (HasGuildHouse(guildId))
        return false;


    const GHLocation* location = GetLocation(locationId);

    if (!location)
        return false;


    GHGuildHouse house;

    house.GuildId = guildId;
    house.OwnerGuid = ownerGuid;
    house.LocationId = locationId;
    house.InstanceId = 0;


    _houses.emplace(guildId, house);


    CharacterDatabase.Execute(
        "INSERT INTO guildhouse (guildId, ownerGuid, locationId) VALUES ({},{},{})",
        guildId,
        ownerGuid,
        locationId);


    GetOrCreateGuildInstance(guildId);


    return true;
}

bool GuildHouseMgr::SellGuildHouse(uint32_t guildId)
{
    auto itr = _houses.find(guildId);

    if (itr == _houses.end())
        return false;


    uint32_t instanceId = GetGuildInstance(guildId);


    //
    // Remove spawned assets
    //
    QueryResult assets = CharacterDatabase.Query(
        "SELECT assetId FROM guildhouse_asset WHERE guildId={}",
        guildId);


    if (assets)
    {
        do
        {
            uint32_t assetId = assets->Fetch()[0].Get<uint32_t>();

            sGuildHouseSpawner.RemoveAsset(
                guildId,
                instanceId,
                assetId);

        } while (assets->NextRow());
    }


    //
    // Remove salesman
    //
    QueryResult salesman = CharacterDatabase.Query(
        "SELECT guid FROM guildhouse_salesman WHERE guildId={}",
        guildId);


    if (salesman)
    {
        do
        {
            uint32_t guid = salesman->Fetch()[0].Get<uint32_t>();

            WorldDatabase.Execute(
                "DELETE FROM creature WHERE guid={}",
                guid);

        } while (salesman->NextRow());
    }


    CharacterDatabase.Execute(
        "DELETE FROM guildhouse_salesman WHERE guildId={}",
        guildId);


    CharacterDatabase.Execute(
        "DELETE FROM guildhouse_spawn WHERE guildId={}",
        guildId);


    CharacterDatabase.Execute(
        "DELETE FROM guildhouse_asset WHERE guildId={}",
        guildId);


    CharacterDatabase.Execute(
        "DELETE FROM guildhouse_instance WHERE guildId={}",
        guildId);


    CharacterDatabase.Execute(
        "DELETE FROM guildhouse WHERE guildId={}",
        guildId);


    sGuildHouseInstanceMgr.RemoveInstance(guildId);


    _houses.erase(itr);


    return true;
}


// =====================================================
// Instance Teleport
// =====================================================

bool GuildHouseMgr::TeleportToGuildHouse(Player* player)
{
    if (!player)
        return false;

    uint32 guildId = player->GetGuildId();

    if (!guildId)
        return false;


    const GHGuildHouse* house = GetGuildHouse(guildId);

    if (!house)
        return false;


    const GHLocation* location =
        GetLocation(house->LocationId);

    if (!location)
        return false;


    uint32 instanceId =
        GetOrCreateGuildInstance(guildId);

    if (!instanceId)
        return false;


    //
    // GuildHouseInstanceMgr owns the instance mapping.
    //
    // No InstanceSaveMgr.
    // No dungeon binding.
    //


    player->TeleportTo(
        location->MapId,
        location->X,
        location->Y,
        location->Z,
        location->O);


    return true;
}


// =====================================================
// Locations
// =====================================================

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


// =====================================================
// Boundary Validation
//
// Prevents using coordinates outside the selected map section.
// =====================================================

bool GuildHouseMgr::IsInsideGuildHouseBoundary(
    uint32_t guildId,
    float x,
    float y) const
{
    const GHLocation* location = GetGuildLocation(guildId);

    if (!location)
        return false;


    if (x < location->MinX ||
        x > location->MaxX)
    {
        return false;
    }


    if (y < location->MinY ||
        y > location->MaxY)
    {
        return false;
    }


    return true;
}


// =====================================================
// Load
// =====================================================

void GuildHouseMgr::Load()
{
    LOG_INFO(
        "server.loading",
        "Loading GuildHouseMgr");


    _houses.clear();
    _locations.clear();


    //
    // Load available map sections
    //
    if (QueryResult result = WorldDatabase.Query(
        "SELECT id,name,mapId,positionX,positionY,positionZ,orientation,minX,maxX,minY,maxY,price,enabled FROM guildhouse_locations"))
    {
        do
        {
            Field* fields = result->Fetch();


            GHLocation location;


            location.Id = fields[0].Get<uint32_t>();
            location.Name = fields[1].Get<std::string>();

            location.MapId = fields[2].Get<uint32_t>();

            location.X = fields[3].Get<float>();
            location.Y = fields[4].Get<float>();
            location.Z = fields[5].Get<float>();
            location.O = fields[6].Get<float>();

            location.MinX = fields[7].Get<float>();
            location.MaxX = fields[8].Get<float>();

            location.MinY = fields[9].Get<float>();
            location.MaxY = fields[10].Get<float>();

            location.Price = fields[11].Get<uint64_t>();

            location.Enabled = fields[12].Get<bool>();


            _locations.emplace(
                location.Id,
                location);


        } while (result->NextRow());
    }


    //
    // Load guild houses
    //
    if (QueryResult result = CharacterDatabase.Query(
        "SELECT guildId,ownerGuid,locationId FROM guildhouse"))
    {
        do
        {
            Field* fields = result->Fetch();


            GHGuildHouse house;


            house.GuildId =
                fields[0].Get<uint32_t>();

            house.OwnerGuid =
                fields[1].Get<uint32_t>();

            house.LocationId =
                fields[2].Get<uint32_t>();

            house.InstanceId =
                sGuildHouseInstanceMgr.GetInstanceId(
                    house.GuildId);


            _houses.emplace(
                house.GuildId,
                house);


        } while(result->NextRow());
    }

    //
    // Load placed assets
    //
    if (QueryResult result = CharacterDatabase.Query(
        "SELECT assetId,guildId,catalogId,status,positionX,positionY,positionZ,orientation FROM guildhouse_asset"))
    {
        do
        {
            Field* fields = result->Fetch();


            uint32_t guildId = fields[1].Get<uint32_t>();


            auto houseItr = _houses.find(guildId);

            if (houseItr == _houses.end())
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


            asset.X =
                fields[4].Get<float>();

            asset.Y =
                fields[5].Get<float>();

            asset.Z =
                fields[6].Get<float>();

            asset.O =
                fields[7].Get<float>();


            houseItr->second.Assets.push_back(asset);


        } while(result->NextRow());
    }


    LOG_INFO(
        "server.loading",
        "GuildHouseMgr loaded {} guild houses and {} locations",
        _houses.size(),
        _locations.size());
}


// =====================================================
// Catalog Purchasing
// =====================================================

bool GuildHouseMgr::PurchaseCatalogItem(Player* player, uint32_t catalogId)
{
    if (!player)
        return false;


    uint32_t guildId = player->GetGuildId();


    if (!guildId)
        return false;


    if (!HasGuildHouse(guildId))
        return false;


    if (!GuildHouseUtil::IsGuildMaster(player))
    {
        ChatHandler(player->GetSession())
            .PSendSysMessage(
                "Only the Guild Master may purchase Guild House items.");

        return false;
    }


    const GHCatalog* catalog =
        sGuildHouseCatalogMgr.GetCatalog(catalogId);


    if (!catalog || !catalog->Enabled)
        return false;


    CharacterDatabase.Execute(
        "INSERT INTO guildhouse_asset "
        "(guildId,catalogId,status,positionX,positionY,positionZ,orientation,createdBy) "
        "VALUES ({},{},{},0,0,0,0,{})",
        guildId,
        catalogId,
        GH_ASSET_PURCHASED,
        player->GetGUID().GetCounter());


    return true;
}


// =====================================================
// Salesman Functions
// =====================================================
bool GuildHouseMgr::HasSalesman(uint32_t guildId) const
{
    QueryResult result = CharacterDatabase.Query(
        "SELECT COUNT(*) FROM guildhouse_salesman WHERE guildId={}",
        guildId);

    if (!result)
        return false;

    return result->Fetch()[0].Get<uint32_t>() > 0;
}

void GuildHouseMgr::RecordSalesmanSpawn(
    uint32_t guildId,
    uint32_t spawnId,
    uint32_t mapId,
    uint32_t instanceId,
    float x,
    float y,
    float z,
    float o)
{
    CharacterDatabase.Execute(
        "INSERT INTO guildhouse_salesman "
        "(guildId,guid,mapId,instanceId,positionX,positionY,positionZ,orientation) "
        "VALUES ({},{},{},{},{},{},{},{})",
        guildId,
        spawnId,
        mapId,
        instanceId,
        x,
        y,
        z,
        o);
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
        ChatHandler(player->GetSession()).PSendSysMessage(
            "Your guild already has a Guild House salesman.");
        return false;
    }

    if (!GuildHouseUtil::CanManageGuildHouse(player))
    {
        ChatHandler(player->GetSession()).PSendSysMessage(
            "Only someone with the proper rank can place the Salesman.");
        return false;
    }

    uint32 instanceId = player->GetInstanceId();

    WorldDatabase.Execute(
        "INSERT INTO creature "
        "(id,map,position_x,position_y,position_z,orientation,spawntimesecs) "
        "VALUES ({},{},{},{},{},{},{});",
        entry,
        player->GetMapId(),
        player->GetPositionX(),
        player->GetPositionY(),
        player->GetPositionZ(),
        player->GetOrientation(),
        300);

    QueryResult result =
        WorldDatabase.Query("SELECT MAX(guid) FROM creature");

    if (!result)
        return false;

    uint32 spawnGuid = result->Fetch()[0].Get<uint32>();

    RecordSalesmanSpawn(
        guildId,
        spawnGuid,
        player->GetMapId(),
        instanceId,
        player->GetPositionX(),
        player->GetPositionY(),
        player->GetPositionZ(),
        player->GetOrientation());

    return true;
}


// =====================================================
// Assets
// =====================================================

const GHGuildAsset* GuildHouseMgr::GetAsset(
    uint32_t guildId,
    uint32_t assetId) const
{
    auto itr = _houses.find(guildId);

    if (itr == _houses.end())
        return nullptr;


    for (const GHGuildAsset& asset :
         itr->second.Assets)
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
    auto itr = _houses.find(guildId);

    if (itr == _houses.end())
        return nullptr;


    for (GHGuildAsset& asset :
         itr->second.Assets)
    {
        if (asset.AssetId == assetId)
            return &asset;
    }


    return nullptr;
}


bool GuildHouseMgr::PlaceAsset(
    Player* player,
    uint32_t assetId)
{
    if (!player)
        return false;


    uint32_t guildId =
        player->GetGuildId();


    if (!GuildHouseUtil::CanManageGuildHouse(player))
        return false;


    if (!IsInsideGuildHouseBoundary(
        guildId,
        player->GetPositionX(),
        player->GetPositionY()))
    {
        ChatHandler(player->GetSession())
            .PSendSysMessage(
                "You cannot place Guild House objects outside the Guild House boundary.");

        return false;
    }


    GHGuildAsset* asset =
        GetAsset(guildId, assetId);


    if (!asset)
        return false;


    if (asset->Status != GH_ASSET_PURCHASED &&
        asset->Status != GH_ASSET_STORED)
        return false;


    asset->Status =
        GH_ASSET_PLACED;


    asset->X =
        player->GetPositionX();

    asset->Y =
        player->GetPositionY();

    asset->Z =
        player->GetPositionZ();

    asset->O =
        player->GetOrientation();


    CharacterDatabase.Execute(
        "UPDATE guildhouse_asset SET status={},positionX={},positionY={},positionZ={},orientation={} WHERE assetId={} AND guildId={}",
        GH_ASSET_PLACED,
        asset->X,
        asset->Y,
        asset->Z,
        asset->O,
        assetId,
        guildId);


    sGuildHouseSpawner.SpawnAsset(
        guildId,
        assetId);


    return true;
}

bool GuildHouseMgr::MoveAsset(Player* player, uint32 assetId)
{
    if (!player)
        return false;

    if (!GuildHouseUtil::CanManageGuildHouse(player))
    {
        ChatHandler(player->GetSession()).PSendSysMessage(
            "Only the Guild Master may move Guild House assets.");
        return false;
    }

    uint32 guildId = player->GetGuildId();

    GHGuildAsset* asset = GetAsset(guildId, assetId);

    if (!asset)
    {
        ChatHandler(player->GetSession()).PSendSysMessage(
            "Unknown Guild House asset.");
        return false;
    }

    if (asset->Status != GH_ASSET_PLACED)
    {
        ChatHandler(player->GetSession()).PSendSysMessage(
            "Only placed assets can be moved.");
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

    asset->X = newX;
    asset->Y = newY;
    asset->Z = newZ;
    asset->O = newO;

    CharacterDatabase.Execute(
        "UPDATE guildhouse_asset SET "
        "positionX={}, positionY={}, positionZ={}, orientation={} "
        "WHERE guildId={} AND assetId={}",
        newX,
        newY,
        newZ,
        newO,
        guildId,
        assetId);

    uint32 instanceId = player->GetInstanceId();

    QueryResult result = CharacterDatabase.Query(
        "SELECT spawnGuid, spawnType "
        "FROM guildhouse_spawn "
        "WHERE guildId={} AND assetId={} AND instanceId={} AND enabled=1",
        guildId,
        assetId,
        instanceId);

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();

            uint32 spawnGuid = fields[0].Get<uint32>();
            uint8 spawnType = fields[1].Get<uint8>();

            if (spawnType == 0)
            {
                WorldDatabase.Execute(
                    "UPDATE creature SET "
                    "position_x=position_x+{}, "
                    "position_y=position_y+{}, "
                    "position_z=position_z+{}, "
                    "orientation={} "
                    "WHERE guid={}",
                    deltaX,
                    deltaY,
                    deltaZ,
                    newO,
                    spawnGuid);
            }
            else
            {
                WorldDatabase.Execute(
                    "UPDATE gameobject SET "
                    "position_x=position_x+{}, "
                    "position_y=position_y+{}, "
                    "position_z=position_z+{}, "
                    "orientation={} "
                    "WHERE guid={}",
                    deltaX,
                    deltaY,
                    deltaZ,
                    newO,
                    spawnGuid);
            }

        } while (result->NextRow());
    }

    CharacterDatabase.Execute(
        "UPDATE guildhouse_spawn SET "
        "positionX=positionX+{}, "
        "positionY=positionY+{}, "
        "positionZ=positionZ+{} "
        "WHERE guildId={} AND assetId={} AND instanceId={}",
        deltaX,
        deltaY,
        deltaZ,
        guildId,
        assetId,
        instanceId);

    return true;
}


bool GuildHouseMgr::StoreAsset(Player* player, uint32 assetId)
{
    if (!player)
        return false;

    if (!GuildHouseUtil::IsGuildMaster(player))
        return false;

    uint32 guildId = player->GetGuildId();

    GHGuildAsset* asset = GetAsset(guildId, assetId);

    if (!asset)
        return false;

    if (asset->Status != GH_ASSET_PLACED)
    {
        ChatHandler(player->GetSession()).PSendSysMessage(
            "That asset is not currently placed.");
        return false;
    }

    if (!sGuildHouseSpawner.RemoveAsset(
            guildId,
            player->GetInstanceId(),
            assetId))
    {
        return false;
    }

    asset->Status = GH_ASSET_STORED;

    CharacterDatabase.Execute(
        "UPDATE guildhouse_asset SET status={} "
        "WHERE guildId={} AND assetId={}",
        GH_ASSET_STORED,
        guildId,
        assetId);

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
    {
        sGuildHouseSpawner.RemoveAsset(
            guildId,
            player->GetInstanceId(),
            assetId);
    }

    CharacterDatabase.Execute(
        "DELETE FROM guildhouse_asset "
        "WHERE guildId={} AND assetId={}",
        guildId,
        assetId);

    auto houseItr = _houses.find(guildId);

    if (houseItr != _houses.end())
    {
        auto& assets = houseItr->second.Assets;

        assets.erase(
            std::remove_if(
                assets.begin(),
                assets.end(),
                [assetId](const GHGuildAsset& asset)
                {
                    return asset.AssetId == assetId;
                }),
            assets.end());
    }

    return true;
}

// =====================================================
// Startup
// =====================================================

class GuildHouseWorldScript : public WorldScript
{
public:

    GuildHouseWorldScript()
        : WorldScript("GuildHouseWorldScript")
    {
    }


    void OnStartup() override
    {
        sGuildHouseInstanceMgr.Load();

        sGuildHouseMgr.Load();

        sGuildHouseSpawner.LoadPlacedAssets();
    }
};


void AddSC_GuildHouseMgr()
{
    new GuildHouseWorldScript();
}
