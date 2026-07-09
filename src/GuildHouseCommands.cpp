#include "ScriptMgr.h"
#include "Chat.h"
#include "ChatCommand.h"
#include "Player.h"
#include "Creature.h"
#include "ObjectMgr.h"
#include "Map.h"
#include "Guild.h"

#include "GuildHouseMgr.h"
#include "GuildHouseCatalogMgr.h"
#include "GuildHouseCommands.h"


// =====================================================
// Permission Check
// =====================================================

static bool GuildHouseCommandScript::CanManageGuildHouse(Player* player)
{
    if (!player)
        return false;

    if (!GuildHouseUtil::IsGuildMaster(player))
        return false;

    if (!GuildHouseUtil::IsOnGMIsland(player))
        return false;

    return true;
}

// =====================================================
// BROKER
//
// Global faction NPC.
//
// Multiple brokers allowed.
// =====================================================

static bool GuildHouseCommandScript::HandleAddBroker(ChatHandler* handler)
{
    Player* player = handler->GetSession()->GetPlayer();

    uint32 entry = (player->GetTeamId() == TEAM_ALLIANCE) ? 900000 : 900001;

    ObjectGuid::LowType spawnId = SpawnPermanentCreature(player, entry, player->GetPhaseMaskForSpawn()); 
    if (!spawnId)
    {
        handler->PSendSysMessage("Failed to spawn Guild House Broker.");

        return false;
    }

    handler->PSendSysMessage("Guild House Broker spawned.");

    return true;
}

// =====================================================
// SALESMAN
//
// Guild Master only.
//
// Permanent guild phased NPC.
// =====================================================

static bool GuildHouseCommandScript::HandleAddSalesman(ChatHandler* handler)
{
    Player* player =
        handler->GetSession()->GetPlayer();


    if (!GuildHouseUtil::IsOnGMIsland(player))
    {
        handler->PSendSysMessage(
            "The Guild House salesman can only be placed on GM Island.");

        return false;
    }


    if (!GuildHouseUtil::IsGuildMaster(player))
    {
        handler->PSendSysMessage(
            "Only the Guild Master may place a Guild House salesman.");

        return false;
    }


    uint32 entry =
        (player->GetTeamId() == TEAM_ALLIANCE)
            ? 900002
            : 900003;


    if (!sGuildHouseMgr.CreatePermanentSalesman(
            player,
            entry))
    {
        handler->PSendSysMessage(
            "Failed to place Guild House salesman.");

        return false;
    }


    handler->PSendSysMessage(
        "Guild House salesman permanently placed.");

    return true;
}

static bool GuildHouseCommandScript::HandleListAssets(ChatHandler* handler, char const*)
{
    Player* player = handler->GetSession()->GetPlayer();
   
    if (!player)
        return false;
   
    uint32 guildId = player->GetGuildId();
    
    if (!guildId)
    {
        handler->PSendSysMessage("You must belong to a guild.");
        
        return false;
    }
    
    const GHGuildHouse* house = sGuildHouseMgr.GetGuildHouse(guildId);
    
    if (!house)
    {
        handler->PSendSysMessage("Your guild does not own a Guild House.");

        return false;
    }
    
    handler->PSendSysMessage("==== Guild House Assets ====");
    
    for (const GHGuildAsset& asset : house->Assets)
    {
        const GHCatalog* catalog = sGuildHouseCatalogMgr.GetCatalog(asset.CatalogId);
        char const* status = "Unknown";
    
        switch(asset.Status)
        {
            case GH_ASSET_PURCHASED:
                status = "Purchased";
                break;

            case GH_ASSET_PLACED:
                status = "Placed";
                break;

            case GH_ASSET_STORED:
                status = "Stored";
                break;

            case GH_ASSET_DISABLED:
                status = "Disabled";
                break;
        }
   
        handler->PSendSysMessage("ID: %u | %s | %s", asset.AssetId, catalog ? catalog->Name.c_str() : "Unknown", status);
    }
    
    return true;
}

static bool GuildHouseCommandScript::HandlePlaceAsset(ChatHandler* handler, char const* args)
{
    Player* player = handler->GetSession()->GetPlayer();
    
    if (!player)
        return false;
   
    if (!args || !*args)
    {
        handler->PSendSysMessage("Usage: .gh asset place <assetId>");

        return false;
    }
    
    uint32 assetId = atoi(args);
   
    if (!assetId)
    {
        handler->PSendSysMessage("Invalid asset id.");

        return false;
    }

    if (!CanManageGuildHouse(player))
    {
        handler->PSendSysMessage("Only the Guild Master may place Guild House assets on GM Island.");
    
        return false;
    }
    
    if (!sGuildHouseMgr.PlaceAsset(player, assetId))
    {
        handler->PSendSysMessage("Failed to place asset.");

        return false;
    }
    
    handler->PSendSysMessage("Guild House asset %u placed.", assetId);
   
    return true;
}

static bool GuildHouseCommandScript::HandleMoveAsset(
    ChatHandler* handler,
    char const* args)
{
    Player* player =
        handler->GetSession()->GetPlayer();


    if (!player)
        return false;


    if (!args || !*args)
    {
        handler->PSendSysMessage(
            "Usage: .gh asset move <assetId>");

        return false;
    }


    uint32 assetId =
        atoi(args);


    if (!assetId)
    {
        handler->PSendSysMessage(
            "Invalid asset id.");

        return false;
    }


    if (!sGuildHouseMgr.MoveAsset(
            player,
            assetId))
    {
        handler->PSendSysMessage(
            "Failed to move Guild House asset.");

        return false;
    }


    handler->PSendSysMessage(
        "Guild House asset %u moved.",
        assetId);


    return true;
}

static bool GuildHouseCommandScript::HandleStoreAsset(
    ChatHandler* handler,
    char const* args)
{
    Player* player =
        handler->GetSession()->GetPlayer();


    if (!player)
        return false;


    if (!args || !*args)
    {
        handler->PSendSysMessage(
            "Usage: .gh asset store <assetId>");

        return false;
    }


    uint32 assetId =
        atoi(args);


    if (!assetId)
    {
        handler->PSendSysMessage(
            "Invalid asset id.");

        return false;
    }


    if (!sGuildHouseMgr.StoreAsset(
            player,
            assetId))
    {
        handler->PSendSysMessage(
            "Failed to store Guild House asset.");

        return false;
    }


    handler->PSendSysMessage(
        "Guild House asset %u stored.",
        assetId);


    return true;
}

static bool GuildHouseCommandScript::HandleSellAsset(
    ChatHandler* handler,
    char const* args)
{
    Player* player =
        handler->GetSession()->GetPlayer();


    if (!player)
        return false;


    if (!args || !*args)
    {
        handler->PSendSysMessage(
            "Usage: .gh asset sell <assetId>");

        return false;
    }


    uint32 assetId =
        atoi(args);


    if (!assetId)
    {
        handler->PSendSysMessage(
            "Invalid asset id.");

        return false;
    }


    if (!sGuildHouseMgr.SellAsset(
            player,
            assetId))
    {
        handler->PSendSysMessage(
            "Failed to sell Guild House asset.");

        return false;
    }


    handler->PSendSysMessage(
        "Guild House asset %u sold.",
        assetId);


    return true;
}


void AddSC_GuildHouseCommands()
{
    new GuildHouseCommandScript();
}
