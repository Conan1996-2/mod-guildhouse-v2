#include "GuildHouseMgr.h"

#include "GuildHousePhaseMgr.h"
#include "GuildHouseSpawner.h"
#include "GuildHouseCatalogMgr.h"
#include "GuildHouseDefines.h"

#include "DatabaseEnv.h"
#include "QueryResult.h"

#include "Player.h"
#include "Chat.h"
#include "Log.h"
#include "ObjectMgr.h"

#include <algorithm>


GuildHouseMgr& GuildHouseMgr::Instance()
{
    static GuildHouseMgr instance;

    return instance;
}



// =====================================================
// Guild Phase Management
// =====================================================

bool GuildHouseMgr::HasPhase(
    uint32_t guildId) const
{
    return sGuildHousePhaseMgr.HasPhase(
        guildId);
}



const GHPhaseRecord*
GuildHouseMgr::GetPhase(
    uint32_t guildId) const
{
    return sGuildHousePhaseMgr.GetPhase(
        guildId);
}



uint32_t GuildHouseMgr::GetPhaseMask(
    uint32_t guildId) const
{
    return sGuildHousePhaseMgr.GetPhaseMask(
        guildId);
}



uint32_t GuildHouseMgr::CreatePhase(
    uint32_t guildId,
    uint32_t mapId)
{
    return sGuildHousePhaseMgr.CreatePhase(
        guildId,
        mapId);
}



bool GuildHouseMgr::EnterPhase(
    Player* player,
    uint32_t guildId,
    uint32_t mapId,
    float x,
    float y,
    float z,
    float o)
{
    return sGuildHousePhaseMgr.EnterPhase(
        player,
        guildId,
        mapId,
        x,
        y,
        z,
        o);
}



bool GuildHouseMgr::RemovePhase(
    uint32_t guildId)
{
    return sGuildHousePhaseMgr.RemovePhase(
        guildId);
}




// =====================================================
// Guild House Ownership
// =====================================================

bool GuildHouseMgr::HasGuildHouse(
    uint32_t guildId) const
{
    return _houses.find(guildId)
        != _houses.end();
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



const GHLocation*
GuildHouseMgr::GetGuildLocation(
    uint32_t guildId) const
{
    const GHGuildHouse* house =
        GetGuildHouse(guildId);


    if (!house)
        return nullptr;


    return GetLocation(
        house->LocationId);
}




bool GuildHouseMgr::CreateGuildHouse(
    Player* player,
    uint32_t guildId,
    uint32_t ownerGuid,
    uint32_t locationId)
{
    if (HasGuildHouse(guildId))
        return false;



    const GHLocation* location =
        GetLocation(locationId);



    if (!location)
        return false;



    GHGuildHouse house;


    house.GuildId =
        guildId;


    house.OwnerGuid =
        ownerGuid;


    house.LocationId =
        locationId;


    house.InstanceId = 0;



    _houses.emplace(
        guildId,
        house);



    CharacterDatabase.Execute(
        "INSERT INTO guildhouse "
        "(guildId,ownerGuid,locationId) "
        "VALUES ({},{},{})",
        guildId,
        ownerGuid,
        locationId);



    uint32_t phase =
        CreatePhase(
            guildId,
            location->MapId);



    if (!phase)
    {
        CharacterDatabase.Execute(
            "DELETE FROM guildhouse WHERE guildId={}",
            guildId);


        _houses.erase(
            guildId);


        return false;
    }



    return true;
}





bool GuildHouseMgr::SellGuildHouse(
    uint32_t guildId)
{
    auto itr =
        _houses.find(guildId);



    if (itr == _houses.end())
        return false;



    uint32_t phaseMask =
        GetPhaseMask(
            guildId);



    //
    // Remove spawned objects
    //
    if (phaseMask)
    {
        sGuildHouseSpawner.RemoveAllAssets(
            guildId,
            phaseMask);
    }




    CharacterDatabase.Execute(
        "DELETE FROM guildhouse_salesman "
        "WHERE guildId={}",
        guildId);



    CharacterDatabase.Execute(
        "DELETE FROM guildhouse_spawn "
        "WHERE guildId={}",
        guildId);



    CharacterDatabase.Execute(
        "DELETE FROM guildhouse_asset "
        "WHERE guildId={}",
        guildId);



    CharacterDatabase.Execute(
        "DELETE FROM guildhouse "
        "WHERE guildId={}",
        guildId);



    RemovePhase(
        guildId);



    _houses.erase(
        itr);



    return true;
}

// =====================================================
// Phase Teleport
// =====================================================

bool GuildHouseMgr::TeleportToGuildHouse(
    Player* player)
{
    if (!player)
        return false;


    uint32_t guildId =
        player->GetGuildId();


    if (!guildId)
        return false;



    const GHGuildHouse* house =
        GetGuildHouse(
            guildId);



    if (!house)
        return false;



    const GHLocation* location =
        GetLocation(
            house->LocationId);



    if (!location)
        return false;



    if (!HasPhase(guildId))
    {
        if (!CreatePhase(
                guildId,
                location->MapId))
        {
            return false;
        }
    }



    return EnterPhase(
        player,
        guildId,
        location->MapId,
        location->X,
        location->Y,
        location->Z,
        location->O);
}




// =====================================================
// Locations
// =====================================================

const GHLocation*
GuildHouseMgr::GetLocation(
    uint32_t locationId) const
{
    auto itr =
        _locations.find(
            locationId);



    if (itr == _locations.end())
        return nullptr;



    return &itr->second;
}



std::vector<const GHLocation*>
GuildHouseMgr::GetLocations() const
{
    std::vector<const GHLocation*> result;



    for (auto const& [id, location] :
         _locations)
    {
        if (location.Enabled)
            result.push_back(
                &location);
    }



    return result;
}




// =====================================================
// Boundary Validation
// =====================================================

bool GuildHouseMgr::IsInsideGuildHouseBoundary(
    uint32_t guildId,
    float x,
    float y) const
{
    const GHLocation* location =
        GetGuildLocation(
            guildId);



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
    // Locations
    //
    if (QueryResult result =
        WorldDatabase.Query(
        "SELECT "
        "id,"
        "name,"
        "mapId,"
        "positionX,"
        "positionY,"
        "positionZ,"
        "orientation,"
        "minX,"
        "maxX,"
        "minY,"
        "maxY,"
        "price,"
        "enabled "
        "FROM guildhouse_locations"))
    {
        do
        {
            Field* fields =
                result->Fetch();



            GHLocation location;



            location.Id =
                fields[0].Get<uint32_t>();


            location.Name =
                fields[1].Get<std::string>();


            location.MapId =
                fields[2].Get<uint32_t>();


            location.X =
                fields[3].Get<float>();


            location.Y =
                fields[4].Get<float>();


            location.Z =
                fields[5].Get<float>();


            location.O =
                fields[6].Get<float>();


            location.MinX =
                fields[7].Get<float>();


            location.MaxX =
                fields[8].Get<float>();


            location.MinY =
                fields[9].Get<float>();


            location.MaxY =
                fields[10].Get<float>();


            location.Price =
                fields[11].Get<uint64_t>();


            location.Enabled =
                fields[12].Get<bool>();



            _locations.emplace(
                location.Id,
                location);



        } while (result->NextRow());
    }





    //
    // Guild houses
    //
    if (QueryResult result =
        CharacterDatabase.Query(
        "SELECT "
        "guildId,"
        "ownerGuid,"
        "locationId "
        "FROM guildhouse"))
    {
        do
        {
            Field* fields =
                result->Fetch();



            GHGuildHouse house;



            house.GuildId =
                fields[0].Get<uint32_t>();


            house.OwnerGuid =
                fields[1].Get<uint32_t>();


            house.LocationId =
                fields[2].Get<uint32_t>();


            house.InstanceId = 0;



            _houses.emplace(
                house.GuildId,
                house);



        } while (result->NextRow());
    }





    //
    // Load phases
    //
    sGuildHousePhaseMgr.Load();





    //
    // Load assets
    //
    if (QueryResult result =
        CharacterDatabase.Query(
        "SELECT "
        "assetId,"
        "guildId,"
        "catalogId,"
        "status,"
        "positionX,"
        "positionY,"
        "positionZ,"
        "orientation "
        "FROM guildhouse_asset"))
    {
        do
        {
            Field* fields =
                result->Fetch();



            uint32_t guildId =
                fields[1].Get<uint32_t>();



            auto houseItr =
                _houses.find(
                    guildId);



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



            houseItr->second.Assets.push_back(
                asset);



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

bool GuildHouseMgr::PurchaseCatalogItem(
    Player* player,
    uint32_t catalogId)
{
    if (!player)
        return false;


    uint32_t guildId =
        player->GetGuildId();


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
        sGuildHouseCatalogMgr.GetCatalog(
            catalogId);



    if (!catalog ||
        !catalog->Enabled)
    {
        return false;
    }



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
// Salesman
// =====================================================

bool GuildHouseMgr::HasSalesman(
    uint32_t guildId) const
{
    QueryResult result =
        CharacterDatabase.Query(
            "SELECT COUNT(*) "
            "FROM guildhouse_salesman "
            "WHERE guildId={}",
            guildId);



    if (!result)
        return false;



    return result->Fetch()[0]
        .Get<uint32_t>() > 0;
}




void GuildHouseMgr::RecordSalesmanSpawn(
    uint32_t guildId,
    uint32_t spawnId,
    uint32_t mapId,
    uint32_t phaseMask,
    float x,
    float y,
    float z,
    float o)
{
    CharacterDatabase.Execute(
        "INSERT INTO guildhouse_salesman "
        "(guildId,guid,mapId,phaseMask,positionX,positionY,positionZ,orientation) "
        "VALUES ({},{},{},{},{},{},{},{})",
        guildId,
        spawnId,
        mapId,
        phaseMask,
        x,
        y,
        z,
        o);
}





bool GuildHouseMgr::CreatePermanentSalesman(
    Player* player,
    uint32 entry)
{
    if (!player)
        return false;



    uint32_t guildId =
        player->GetGuildId();



    if (!guildId)
        return false;



    if (HasSalesman(guildId))
    {
        ChatHandler(player->GetSession())
            .PSendSysMessage(
                "Your guild already has a Guild House salesman.");

        return false;
    }




    if (!GuildHouseUtil::CanManageGuildHouse(player))
    {
        ChatHandler(player->GetSession())
            .PSendSysMessage(
                "You cannot place this salesman.");

        return false;
    }



    uint32_t phaseMask =
        GetPhaseMask(
            guildId);



    if (!phaseMask)
        return false;



    WorldDatabase.Execute(
        "INSERT INTO creature "
        "(id,map,phaseMask,position_x,position_y,position_z,orientation,spawntimesecs) "
        "VALUES ({},{},{},{},{},{},{},{})",
        entry,
        player->GetMapId(),
        phaseMask,
        player->GetPositionX(),
        player->GetPositionY(),
        player->GetPositionZ(),
        player->GetOrientation(),
        300);



    QueryResult result =
        WorldDatabase.Query(
            "SELECT MAX(guid) FROM creature");



    if (!result)
        return false;



    uint32_t guid =
        result->Fetch()[0]
        .Get<uint32_t>();



    RecordSalesmanSpawn(
        guildId,
        guid,
        player->GetMapId(),
        phaseMask,
        player->GetPositionX(),
        player->GetPositionY(),
        player->GetPositionZ(),
        player->GetOrientation());



    return true;
}





// =====================================================
// Assets
// =====================================================

const GHGuildAsset*
GuildHouseMgr::GetAsset(
    uint32_t guildId,
    uint32_t assetId) const
{
    auto itr =
        _houses.find(guildId);



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





GHGuildAsset*
GuildHouseMgr::GetAsset(
    uint32_t guildId,
    uint32_t assetId)
{
    auto itr =
        _houses.find(guildId);



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





std::vector<const GHGuildAsset*>
GuildHouseMgr::GetPurchasedAssets(
    uint32_t guildId) const
{
    std::vector<const GHGuildAsset*> result;



    auto itr =
        _houses.find(guildId);



    if (itr == _houses.end())
        return result;



    for (const GHGuildAsset& asset :
         itr->second.Assets)
    {
        result.push_back(&asset);
    }



    return result;
}

// =====================================================
// Place Asset
// =====================================================

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
                "You cannot place Guild House objects outside the boundary.");

        return false;
    }



    GHGuildAsset* asset =
        GetAsset(
            guildId,
            assetId);



    if (!asset)
        return false;



    if (asset->Status != GH_ASSET_PURCHASED &&
        asset->Status != GH_ASSET_STORED)
    {
        return false;
    }



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
        "UPDATE guildhouse_asset SET "
        "status={},positionX={},positionY={},positionZ={},orientation={} "
        "WHERE guildId={} AND assetId={}",
        GH_ASSET_PLACED,
        asset->X,
        asset->Y,
        asset->Z,
        asset->O,
        guildId,
        assetId);



    sGuildHouseSpawner.SpawnAsset(
        guildId,
        assetId);



    return true;
}





// =====================================================
// Move Asset
// =====================================================

bool GuildHouseMgr::MoveAsset(
    Player* player,
    uint32_t assetId)
{
    if (!player)
        return false;



    uint32_t guildId =
        player->GetGuildId();



    if (!GuildHouseUtil::CanManageGuildHouse(player))
        return false;



    GHGuildAsset* asset =
        GetAsset(
            guildId,
            assetId);



    if (!asset)
        return false;



    if (asset->Status != GH_ASSET_PLACED)
        return false;



    float deltaX =
        player->GetPositionX() - asset->X;


    float deltaY =
        player->GetPositionY() - asset->Y;


    float deltaZ =
        player->GetPositionZ() - asset->Z;



    asset->X =
        player->GetPositionX();

    asset->Y =
        player->GetPositionY();

    asset->Z =
        player->GetPositionZ();

    asset->O =
        player->GetOrientation();



    CharacterDatabase.Execute(
        "UPDATE guildhouse_asset SET "
        "positionX={},positionY={},positionZ={},orientation={} "
        "WHERE guildId={} AND assetId={}",
        asset->X,
        asset->Y,
        asset->Z,
        asset->O,
        guildId,
        assetId);



    sGuildHouseSpawner.MoveAsset(
        guildId,
        assetId,
        deltaX,
        deltaY,
        deltaZ,
        asset->O);



    return true;
}





// =====================================================
// Store Asset
// =====================================================

bool GuildHouseMgr::StoreAsset(
    Player* player,
    uint32_t assetId)
{
    if (!player)
        return false;



    uint32_t guildId =
        player->GetGuildId();



    if (!GuildHouseUtil::IsGuildMaster(player))
        return false;



    GHGuildAsset* asset =
        GetAsset(
            guildId,
            assetId);



    if (!asset)
        return false;



    if (asset->Status != GH_ASSET_PLACED)
        return false;



    if (!sGuildHouseSpawner.RemoveAsset(
            guildId,
            assetId))
    {
        return false;
    }



    asset->Status =
        GH_ASSET_STORED;



    CharacterDatabase.Execute(
        "UPDATE guildhouse_asset SET status={} "
        "WHERE guildId={} AND assetId={}",
        GH_ASSET_STORED,
        guildId,
        assetId);



    return true;
}





// =====================================================
// Sell Asset
// =====================================================

bool GuildHouseMgr::SellAsset(
    Player* player,
    uint32_t assetId)
{
    if (!player)
        return false;



    uint32_t guildId =
        player->GetGuildId();



    if (!GuildHouseUtil::IsGuildMaster(player))
        return false;



    GHGuildAsset* asset =
        GetAsset(
            guildId,
            assetId);



    if (!asset)
        return false;



    if (asset->Status == GH_ASSET_PLACED)
    {
        sGuildHouseSpawner.RemoveAsset(
            guildId,
            assetId);
    }



    CharacterDatabase.Execute(
        "DELETE FROM guildhouse_asset "
        "WHERE guildId={} AND assetId={}",
        guildId,
        assetId);



    auto itr =
        _houses.find(guildId);



    if (itr != _houses.end())
    {
        auto& assets =
            itr->second.Assets;



        assets.erase(
            std::remove_if(
                assets.begin(),
                assets.end(),
                [assetId](const GHGuildAsset& a)
                {
                    return a.AssetId == assetId;
                }),
            assets.end());
    }



    return true;
}





// =====================================================
// Startup Script
// =====================================================

class GuildHouseWorldScript :
    public WorldScript
{
public:

    GuildHouseWorldScript()
        : WorldScript(
            "GuildHouseWorldScript")
    {
    }



    void OnStartup() override
    {
        sGuildHousePhaseMgr.Load();

        sGuildHouseCatalogMgr.Load();

        sGuildHouseMgr.Load();

        sGuildHouseSpawner.LoadPlacedAssets();
    }
};





void AddSC_GuildHouseMgr()
{
    new GuildHouseWorldScript();
}
