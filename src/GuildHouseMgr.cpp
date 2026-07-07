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


    uint32 guildId = player->GetGuildId();


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



    const GHCatalog* catalog =
        sGuildHouseCatalogMgr.GetCatalog(catalogId);



    if (!catalog || !catalog->Enabled)
    {
        ChatHandler(player->GetSession())
            .PSendSysMessage(
                "Invalid Guild House item.");

        return false;
    }



    uint32 phase =
        GetPhase(guildId);



    float x =
        player->GetPositionX();

    float y =
        player->GetPositionY();

    float z =
        player->GetPositionZ();

    float o =
        player->GetOrientation();



    std::ostringstream ss;


    ss <<
    "INSERT INTO guildhouse_asset "
    "(guildId, layoutId, catalogId, status, phase,"
    "positionX, positionY, positionZ, orientation, createdBy)"
    " VALUES ("
    << guildId << ","
    << 1 << ","
    << catalogId << ","
    << GH_ASSET_PLACED << ","
    << phase << ","
    << x << ","
    << y << ","
    << z << ","
    << o << ","
    << player->GetGUID().GetCounter()
    << ")";


    CharacterDatabase.Execute(ss.str());



    QueryResult result =
        CharacterDatabase.Query(
            "SELECT LAST_INSERT_ID()");


    if (!result)
        return false;



    uint32 assetId =
        result->Fetch()[0]
        .Get<uint32>();



    if (!sGuildHouseSpawner.SpawnAsset(
            guildId,
            assetId))
    {
        ChatHandler(player->GetSession())
            .PSendSysMessage(
                "Failed to place Guild House item.");
    
        return false;
    }



    ChatHandler(player->GetSession())
        .PSendSysMessage(
            "Guild House item placed.");



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
