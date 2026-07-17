#include "ScriptMgr.h"

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

#include <algorithm>



GuildHouseMgr&
GuildHouseMgr::Instance()
{
    static GuildHouseMgr instance;

    return instance;
}



// =====================================================
// Phase Management
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
    uint32_t locationId)
{
    return sGuildHousePhaseMgr.CreatePhase(
        guildId,
        locationId);
}



bool GuildHouseMgr::EnterPhase(
    Player* player,
    uint32_t guildId)
{
    return sGuildHousePhaseMgr.EnterPhase(
        player,
        guildId);
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




// =====================================================
// Create Guild House
// =====================================================


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



    house.PhaseMask =
        0;



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



    uint32_t phaseMask =
        CreatePhase(
            guildId,
            locationId);



    if (!phaseMask)
    {
        CharacterDatabase.Execute(
            "DELETE FROM guildhouse "
            "WHERE guildId={}",
            guildId);



        _houses.erase(
            guildId);



        return false;
    }



    _houses[guildId].PhaseMask =
        phaseMask;



    return true;
}






// =====================================================
// Sell Guild House
// =====================================================


bool GuildHouseMgr::SellGuildHouse(
    uint32_t guildId)
{
    auto itr =
        _houses.find(guildId);



    if (itr == _houses.end())
        return false;



    uint32_t phaseMask =
        GetPhaseMask(guildId);



    if (phaseMask)
    {
        sGuildHouseSpawner.RemoveAllAssets(
            guildId);
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
// Teleport
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
        GetGuildLocation(
            guildId);



    if (!location)
        return false;




    if (!HasPhase(guildId))
    {
        if (!CreatePhase(
            guildId,
            house->LocationId))
        {
            return false;
        }
    }



    return EnterPhase(
        player,
        guildId);
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



    for(auto const& [id, location] :
        _locations)
    {
        if(location.Enabled)
            result.push_back(
                &location);
    }



    return result;
}





// =====================================================
// Boundary
// =====================================================


bool GuildHouseMgr::IsInsideGuildHouseBoundary(
    uint32_t guildId,
    float x,
    float y) const
{
    const GHLocation* location =
        GetGuildLocation(
            guildId);



    if(!location)
        return false;



    return
        x >= location->MinX &&
        x <= location->MaxX &&
        y >= location->MinY &&
        y <= location->MaxY;
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




    // -------------------------------------------------
    // Locations
    // -------------------------------------------------

    if(QueryResult result =
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
                fields[0].Get<uint32>();



            location.Name =
                fields[1].Get<std::string>();



            location.MapId =
                fields[2].Get<uint32>();



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



        }while(result->NextRow());
    }




    // -------------------------------------------------
    // Guild Houses
    // -------------------------------------------------

    if(QueryResult result =
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
                fields[0].Get<uint32>();



            house.OwnerGuid =
                fields[1].Get<uint32>();



            house.LocationId =
                fields[2].Get<uint32>();



            house.PhaseMask =
                0;



            _houses.emplace(
                house.GuildId,
                house);



        }while(result->NextRow());
    }




    // -------------------------------------------------
    // Load phases
    // -------------------------------------------------

    sGuildHousePhaseMgr.Load();



    for(auto& [guildId, house] :
        _houses)
    {
        house.PhaseMask =
            GetPhaseMask(
                guildId);
    }




    // -------------------------------------------------
    // Assets
    // -------------------------------------------------

    if(QueryResult result =
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



            uint32 guildId =
                fields[1].Get<uint32>();



            auto itr =
                _houses.find(
                    guildId);



            if(itr == _houses.end())
                continue;



            GHGuildAsset asset;



            asset.AssetId =
                fields[0].Get<uint32>();



            asset.GuildId =
                guildId;



            asset.CatalogId =
                fields[2].Get<uint32>();



            asset.Status =
                static_cast<GHAssetStatus>(
                    fields[3].Get<uint8>());



            asset.X =
                fields[4].Get<float>();

            asset.Y =
                fields[5].Get<float>();

            asset.Z =
                fields[6].Get<float>();

            asset.O =
                fields[7].Get<float>();



            itr->second.Assets.push_back(
                asset);



        }while(result->NextRow());
    }



    LOG_INFO(
        "server.loading",
        "GuildHouseMgr loaded {} houses and {} locations",
        _houses.size(),
        _locations.size());
}

bool GuildHouseMgr::PlaceAsset(
    Player* player,
    uint32_t assetId)
{
    if (!player)
        return false;


    uint32_t guildId =
        player->GetGuildId();


    if (!guildId)
        return false;


    GHGuildHouse const* house =
        GetGuildHouse(guildId);


    if (!house)
        return false;



    //
    // Must be inside guild phase
    //

    if (!sGuildHousePhaseMgr.IsMember(player))
        return false;



    CharacterDatabase.Execute(
        "UPDATE guildhouse_asset "
        "SET status={} "
        "WHERE guildId={} "
        "AND assetId={}",
        GH_ASSET_PLACED,
        guildId,
        assetId);



    return sGuildHouseSpawner.SpawnAsset(
        guildId,
        assetId);
}

bool GuildHouseMgr::MoveAsset(
    Player* player,
    uint32_t assetId)
{
    if (!player)
        return false;


    uint32 guildId =
        player->GetGuildId();


    if (!guildId)
        return false;



    if (!sGuildHousePhaseMgr.IsMember(player))
        return false;



    //
    // Later we can add mover mode:
    // target position capture
    //

    return true;
}

bool GuildHouseMgr::StoreAsset(
    Player* player,
    uint32_t assetId)
{
    if (!player)
        return false;


    uint32 guildId =
        player->GetGuildId();



    if (!guildId)
        return false;



    if (!sGuildHousePhaseMgr.IsMember(player))
        return false;



    sGuildHouseSpawner.RemoveAsset(
        guildId,
        assetId);



    CharacterDatabase.Execute(
        "UPDATE guildhouse_asset "
        "SET status={} "
        "WHERE guildId={} "
        "AND assetId={}",
        GH_ASSET_STORED,
        guildId,
        assetId);



    return true;
}

bool GuildHouseMgr::SellAsset(
    Player* player,
    uint32_t assetId)
{
    if (!player)
        return false;


    uint32 guildId =
        player->GetGuildId();



    if (!guildId)
        return false;



    if (!sGuildHousePhaseMgr.IsMember(player))
        return false;



    sGuildHouseSpawner.RemoveAsset(
        guildId,
        assetId);



    CharacterDatabase.Execute(
        "DELETE FROM guildhouse_asset "
        "WHERE guildId={} "
        "AND assetId={}",
        guildId,
        assetId);



    return true;
}


// =====================================================
// Catalog Purchase
// =====================================================


bool GuildHouseMgr::PurchaseCatalogItem(
    Player* player,
    uint32_t catalogId)
{
    if(!player)
        return false;



    uint32_t guildId =
        player->GetGuildId();



    if(!guildId)
        return false;



    if(!HasGuildHouse(guildId))
        return false;



    if(!GuildHouseUtil::IsGuildMaster(player))
        return false;



    const GHCatalog* catalog =
        sGuildHouseCatalogMgr.GetCatalog(
            catalogId);



    if(!catalog ||
       !catalog->Enabled)
    {
        return false;
    }



    CharacterDatabase.Execute(
        "INSERT INTO guildhouse_asset "
        "(guildId,catalogId,status,"
        "positionX,positionY,positionZ,orientation,createdBy)"
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



    if(!result)
        return false;



    return result->Fetch()[0]
        .Get<uint32>() > 0;
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
        "(guildId,guid,mapId,phaseMask,"
        "positionX,positionY,positionZ,orientation)"
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
    uint32_t entry)
{
    if(!player)
        return false;



    uint32_t guildId =
        player->GetGuildId();



    uint32_t phaseMask =
        GetPhaseMask(
            guildId);



    if(!phaseMask)
        return false;



    WorldDatabase.Execute(
        "INSERT INTO creature "
        "(id,map,phaseMask,"
        "position_x,position_y,position_z,orientation)"
        "VALUES ({},{},{},{},{},{},{})",

        entry,
        player->GetMapId(),
        phaseMask,
        player->GetPositionX(),
        player->GetPositionY(),
        player->GetPositionZ(),
        player->GetOrientation());



    return true;
}

// =====================================================
// Script Registration
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
        sGuildHousePhaseMgr.Load();

        sGuildHouseMgr.Load();

        sGuildHouseSpawner.LoadPlacedAssets();
    }
};


void AddSC_GuildHouseMgr()
{
    new GuildHouseWorldScript();
}
