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

GuildHouseCommandScript::GuildHouseCommandScript() : CommandScript("GuildHouseCommandScript")
{
}

    ChatCommandTable GuildHouseCommandScript::GetCommands() const
    {
        static ChatCommandTable npcTable =
        {
            { "broker",   GuildHouseCommandScript::HandleAddBroker,   SEC_GAMEMASTER, Console::No },
            { "salesman", GuildHouseCommandScript::HandleAddSalesman, SEC_PLAYER, Console::No }
        };
                
        static ChatCommandTable assetTable =
        {
            { "list",  GuildHouseCommandScript::HandleListAssets,  SEC_PLAYER, Console::No },
            { "place", GuildHouseCommandScript::HandlePlaceAsset,  SEC_PLAYER, Console::No },
            { "move",  GuildHouseCommandScript::HandleMoveAsset,   SEC_PLAYER, Console::No },
            { "store", GuildHouseCommandScript::HandleStoreAsset,  SEC_PLAYER, Console::No },
            { "sell",  GuildHouseCommandScript::HandleSellAsset,   SEC_PLAYER, Console::No }
        };
                
        static ChatCommandTable guildHouseTable =
        {
            { "add",      npcTable },
            { "asset",    assetTable },
            { "tele",     GuildHouseCommandScript::HandleTeleportAsset,   SEC_PLAYER, Console::No },
            { "teleport", GuildHouseCommandScript::HandleTeleportAsset,   SEC_PLAYER, Console::No }
        };
                
        static ChatCommandTable root =
        {
            { "gh",          guildHouseTable },
            { "guildhouse",  guildHouseTable }
        };

        return root;
    }

// =====================================================
// BROKER
//
// Global faction NPC.
//
// Multiple brokers allowed.
// =====================================================

bool GuildHouseCommandScript::HandleAddBroker(ChatHandler* handler)
{
    Player* player = handler->GetSession()->GetPlayer();
    if (!player)
        return false;

    uint32 entry = (player->GetTeamId() == TEAM_ALLIANCE) ? 900000 : 900001;

    Creature* creature = new Creature();

    if (!creature->Create(player->GetMap()->GenerateLowGuid<HighGuid::Unit>(), player->GetMap(),  player->GetPhaseMaskForSpawn(), entry, 0,  player->GetPositionX(),  player->GetPositionY(),  player->GetPositionZ(), player->GetOrientation()))
    {
        delete creature;
        return false;
    }
    creature->SaveToDB(player->GetMapId(), (1 << player->GetMap()->GetSpawnMode()),  player->GetPhaseMaskForSpawn());
    uint32 db_guid = creature->GetSpawnId();

    creature->CleanupsBeforeDelete();
    delete creature;
    creature = new Creature();
    if (!creature->LoadCreatureFromDB(db_guid, player->GetMap()))
    {
        delete creature;
        return false;
    }

    sObjectMgr->AddCreatureToGrid(db_guid, sObjectMgr->GetCreatureData(db_guid));
    
    handler->PSendSysMessage("Guild House Broker permanently spawned (GUID {}).", db_guid);

    return true;
}

// =====================================================
// SALESMAN
//
// Guild Master only.
//
// Permanent guild phased NPC.
// =====================================================

bool GuildHouseCommandScript::HandleAddSalesman(ChatHandler* handler)
{
    Player* player = handler->GetSession()->GetPlayer();

    if (!GuildHouseUtil::CanManageGuildHouse(player))
    {
        handler->PSendSysMessage("Only the Guild Master may place Guild House Salesman on GM Island.");
        return false;
    }

    uint32 entry = (player->GetTeamId() == TEAM_ALLIANCE) ? 900002 : 900003;
    if (!sGuildHouseMgr.CreatePermanentSalesman(player, entry))
    {
        handler->PSendSysMessage("Failed to place Guild House salesman: Error creating.");
        return false;
    }

    handler->PSendSysMessage("Guild House salesman permanently placed.");

    return true;
}

bool GuildHouseCommandScript::HandleListAssets(ChatHandler* handler, char const*)
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
   
        handler->PSendSysMessage("ID: {} | {} | {}", asset.AssetId, catalog ? catalog->Name.c_str() : "Unknown", status);
    }
    
    return true;
}

bool GuildHouseCommandScript::HandlePlaceAsset(ChatHandler* handler, char const* args)
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

    if (!GuildHouseUtil::CanManageGuildHouse(player))
    {
        handler->PSendSysMessage("Only the Guild Master may place Guild House assets on GM Island.");
            return false;
    }
    
    if (!sGuildHouseMgr.PlaceAsset(player, assetId))
    {
        handler->PSendSysMessage("Failed to place asset.");
        return false;
    }
    
    handler->PSendSysMessage("Guild House asset {} placed.", assetId);
   
    return true;
}

bool GuildHouseCommandScript::HandleMoveAsset(ChatHandler* handler, char const* args)
{
    Player* player = handler->GetSession()->GetPlayer();
    if (!player)
        return false;

    if (!args || !*args)
    {
        handler->PSendSysMessage("Usage: .gh asset move <assetId>");
        return false;
    }

    uint32 assetId = atoi(args);
    if (!assetId)
    {
        handler->PSendSysMessage("Invalid asset id.");
        return false;
    }

    if (!sGuildHouseMgr.MoveAsset(player, assetId))
    {
        handler->PSendSysMessage("Failed to move Guild House asset.");
        return false;
    }

    handler->PSendSysMessage("Guild House asset {} moved.", assetId);

    return true;
}

bool GuildHouseCommandScript::HandleStoreAsset(ChatHandler* handler, char const* args)
{
    Player* player = handler->GetSession()->GetPlayer();
    if (!player)
        return false;

    if (!args || !*args)
    {
        handler->PSendSysMessage("Usage: .gh asset store <assetId>");
        return false;
    }

    uint32 assetId = atoi(args);
    if (!assetId)
    {
        handler->PSendSysMessage("Invalid asset id.");
        return false;
    }

    if (!sGuildHouseMgr.StoreAsset(player, assetId))
    {
        handler->PSendSysMessage("Failed to store Guild House asset.");
        return false;
    }

    handler->PSendSysMessage("Guild House asset {} stored.", assetId);

    return true;
}

bool GuildHouseCommandScript::HandleTeleportAsset(ChatHandler* handler, char const* args)
{
    Player* player = handler->GetSession()->GetPlayer();
    if (!player)
        return false;
    
    if (!sGuildHouseMgr.TeleportToGuildHouse(player))
    {
        ChatHandler(player->GetSession()).PSendSysMessage("Unable to teleport to Guild House.");
        return false;
    }
    return true;
}

bool GuildHouseCommandScript::HandleSellAsset(ChatHandler* handler, char const* args)
{
    Player* player = handler->GetSession()->GetPlayer();
    if (!player)
        return false;

    if (!args || !*args)
    {
        handler->PSendSysMessage("Usage: .gh asset sell <assetId>");
        return false;
    }

    uint32 assetId = atoi(args);
    if (!assetId)
    {
        handler->PSendSysMessage("Invalid asset id.");
        return false;
    }

    if (!sGuildHouseMgr.SellAsset(player, assetId))
    {
        handler->PSendSysMessage("Failed to sell Guild House asset.");
        return false;
    }

    handler->PSendSysMessage("Guild House asset {} sold.", assetId);

    return true;
}


void AddSC_GuildHouseCommands()
{
    new GuildHouseCommandScript();
}
