#include "GuildHouseCommands.h"

#include "GuildHouseMgr.h"
#include "GuildHouseCatalogMgr.h"
#include "GuildHouseDefines.h"
#include "GuildHouseTypes.h"
#include "GuildHouseSpawner.h"

#include "Chat.h"
#include "Player.h"
#include "Creature.h"
#include "ObjectMgr.h"
#include "Map.h"
#include "Guild.h"

#include <cstdlib>

GuildHouseCommandScript::GuildHouseCommandScript() : CommandScript("GuildHouseCommandScript")
{
}

// =====================================================
// Command Registration
// =====================================================
ChatCommandTable GuildHouseCommandScript::GetCommands() const
{
    static ChatCommandTable npcTable =
    {
        { "broker",   HandleAddBroker,   SEC_GAMEMASTER, Console::No },
        { "salesman", HandleAddSalesman, SEC_PLAYER,     Console::No }
    };

    static ChatCommandTable houseTable =
    {
        { "sell",     HandleSellGuildHouse,   SEC_PLAYER, Console::No },
        { "tele",     HandleTeleportGuildHouse, SEC_PLAYER, Console::No },
        { "teleport", HandleTeleportGuildHouse, SEC_PLAYER, Console::No }
    };

    static ChatCommandTable assetTable =
    {
        { "list",  HandleListAssets,  SEC_PLAYER, Console::No },
        { "place", HandlePlaceAsset,  SEC_PLAYER, Console::No },
        { "move",  HandleMoveAsset,   SEC_PLAYER, Console::No },
        { "store", HandleStoreAsset,  SEC_PLAYER, Console::No },
        { "sell",  HandleSellAsset,   SEC_PLAYER, Console::No }
    };

    static ChatCommandTable shopTable =
    {
        { "categories", HandleListCategories, SEC_PLAYER, Console::No },
        { "list",       HandleListCatalog,    SEC_PLAYER, Console::No },
        { "buy",        HandlePurchaseCatalog, SEC_PLAYER, Console::No }
    };

    static ChatCommandTable guildHouseTable =
    {
        { "npc",   npcTable },
        { "house", houseTable },
        { "asset", assetTable },
        { "shop",  shopTable }
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
// Global broker NPC.
// =====================================================
bool GuildHouseCommandScript::HandleAddBroker(ChatHandler* handler)
{
    Player* player = handler->GetSession()->GetPlayer();
    if (!player)
        return false;

    uint32 entry = player->GetTeamId() == TEAM_ALLIANCE ? 900000 : 900001;
    if(sGuildHouseSpawner.SpawnCreature (0, 0,  player->GetPhaseMaskForSpawn(), player->GetMapId(), entry, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation()))
        handler->PSendSysMessage("Guild House Broker permanently spawned.");
    else
        handler->PSendSysMessage("Unable to spawn Guild House Broker");

    return true;
}

// =====================================================
// SALESMAN
//
// Guild instance salesman.
// =====================================================
bool GuildHouseCommandScript::HandleAddSalesman(ChatHandler* handler)
{
    Player* player = handler->GetSession()->GetPlayer();
    if (!player)
        return false;

    if (!GuildHouseUtil::CanManageGuildHouse(player))
    {
        handler->PSendSysMessage("Only the Guild Master may place the Guild House salesman.");
        return true;
    }

    uint32 entry = player->GetTeamId() == TEAM_ALLIANCE ? 900002 : 900003;
    if (!sGuildHouseMgr.CreatePermanentSalesman(player, entry))
        handler->PSendSysMessage("Failed creating Guild House salesman.");
    else
        handler->PSendSysMessage("Guild House salesman created.");

    return true;
}

// =====================================================
// Sell Guild House
// =====================================================
bool GuildHouseCommandScript::HandleSellGuildHouse(ChatHandler* handler, char const*)
{
    Player* player = handler->GetSession()->GetPlayer();
    if (!player)
        return false;

    uint32 guildId = player->GetGuildId();
    if (!guildId)
    {
        handler->PSendSysMessage("You must belong to a guild.");
        return true;
    }

    if (!GuildHouseUtil::IsGuildMaster(player))
    {
        handler->PSendSysMessage("Only the Guild Master may sell the Guild House.");
        return true;
    }

    if (!sGuildHouseMgr.SellGuildHouse(guildId))
    {
        handler->PSendSysMessage("Failed selling Guild House.");
        return true;
    }

    handler->PSendSysMessage("Guild House sold.");

    return true;
}

// =====================================================
// Teleport
// =====================================================
bool GuildHouseCommandScript::HandleTeleportGuildHouse(ChatHandler* handler, char const*)
{
    Player* player = handler->GetSession()->GetPlayer();
    if (!player)
        return false;

    if (!sGuildHouseMgr.TeleportToGuildHouse(player))
    {
        handler->PSendSysMessage("Unable to teleport to Guild House.");
        return true;
    }

    return true;
}

// =====================================================
// List Assets
// =====================================================
bool GuildHouseCommandScript::HandleListAssets(ChatHandler* handler, char const*)
{
    Player* player = handler->GetSession()->GetPlayer();
    if (!player)
        return false;

    uint32 guildId = player->GetGuildId();
    if (!guildId)
    {
        handler->PSendSysMessage("You must belong to a guild.");
        return true;
    }

    const GHGuildHouse* house = sGuildHouseMgr.GetGuildHouse(guildId);
    if (!house)
    {
        handler->PSendSysMessage("Your guild does not own a Guild House.");
        return true;
    }

    handler->PSendSysMessage("==== Guild House Assets ====");

    if (house->Assets.empty())
    {
        handler->PSendSysMessage("No guild assets.");
        return true;
    }
    
    for (auto const& [assetId, asset] : house->Assets)
    {
        const GHCatalog* catalog = sGuildHouseCatalogMgr.GetCatalog(asset.CatalogId);

        char const* statusText = "Unknown";
        switch (asset.Status)
        {
            case GH_ASSET_PURCHASED:  statusText = "Purchased"; break;
            case GH_ASSET_PLACED:     statusText = "Placed";    break;
            case GH_ASSET_STORED:     statusText = "Stored";    break;
            case GH_ASSET_DISABLED:   statusText = "Disabled";  break;
        }

        handler->PSendSysMessage("Asset {} | {} | {}", assetId, catalog ? catalog->Name.c_str() : "Unknown", statusText);
    }
    
    return true;
}

// =====================================================
// Place Asset
// =====================================================
bool GuildHouseCommandScript::HandlePlaceAsset(ChatHandler* handler, char const* args)
{
    Player* player = handler->GetSession()->GetPlayer();
    if (!player)
        return false;

    if (!args || !*args)
    {
        handler->PSendSysMessage("Usage: .gh asset place <assetId>");
        return true;
    }

    uint32 assetId = atoi(args);
    if (!assetId)
    {
        handler->PSendSysMessage("Invalid asset id.");
        return true;
    }

    if (!sGuildHouseMgr.PlaceAsset(player, assetId))
    {
        handler->PSendSysMessage("Failed placing Guild House asset.");
        return true;
    }

    handler->PSendSysMessage("Guild House asset {} placed.", assetId);

    return true;
}

// =====================================================
// Move Asset
// =====================================================
bool GuildHouseCommandScript::HandleMoveAsset(ChatHandler* handler, char const* args)
{
    Player* player = handler->GetSession()->GetPlayer();
    if (!player)
        return false;

    if (!args || !*args)
    {
        handler->PSendSysMessage("Usage: .gh asset move <assetId>");
        return true;
    }

    uint32 assetId = atoi(args);
    if (!assetId)
    {
        handler->PSendSysMessage("Invalid asset id.");
        return true;
    }

    if (!sGuildHouseMgr.MoveAsset(player, assetId))
    {
        handler->PSendSysMessage("Failed moving Guild House asset.");
        return true;
    }

    handler->PSendSysMessage("Guild House asset {} moved.", assetId);

    return true;
}

// =====================================================
// Store Asset
//
// Removes world spawn but keeps ownership.
// =====================================================
bool GuildHouseCommandScript::HandleStoreAsset(ChatHandler* handler, char const* args)
{
    Player* player = handler->GetSession()->GetPlayer();
    if (!player)
        return false;

    if (!args || !*args)
    {
        handler->PSendSysMessage("Usage: .gh asset store <assetId>");
        return true;
    }

    uint32 assetId = atoi(args);
    if (!assetId)
    {
        handler->PSendSysMessage("Invalid asset id.");
        return true;
    }

    if (!sGuildHouseMgr.StoreAsset(player, assetId))
    {
        handler->PSendSysMessage("Failed storing Guild House asset.");
        return true;
    }

    handler->PSendSysMessage("Guild House asset {} stored.", assetId);

    return true;
}

// =====================================================
// Sell Asset
//
// Removes ownership permanently.
// =====================================================
bool GuildHouseCommandScript::HandleSellAsset(ChatHandler* handler, char const* args)
{
    Player* player = handler->GetSession()->GetPlayer();
    if (!player)
        return false;

    if (!args || !*args)
    {
        handler->PSendSysMessage("Usage: .gh asset sell <assetId>");
        return true;
    }

    uint32 assetId = atoi(args);
    if (!assetId)
    {
        handler->PSendSysMessage("Invalid asset id.");
        return true;
    }

    if (!sGuildHouseMgr.SellAsset(player, assetId))
    {
        handler->PSendSysMessage("Failed selling Guild House asset.");
        return true;
    }

    handler->PSendSysMessage("Guild House asset {} sold.", assetId);

    return true;
}

// =====================================================
// List Root Categories
// =====================================================
bool GuildHouseCommandScript::HandleListCategories(ChatHandler* handler, char const*)
{
    std::vector<const GHCategory*> categories = sGuildHouseCatalogMgr.GetRootCategories();
    if (categories.empty())
    {
        handler->PSendSysMessage("No Guild House categories available.");
        return true;
    }

    handler->PSendSysMessage("==== Guild House Categories ====");

    for (const GHCategory* category : categories)
    {
        if (!category)
            continue;

        handler->PSendSysMessage("{} - {}", category->Id, category->Name.c_str());
    }

    return true;
}

// =====================================================
// List Catalog Items
//
// Usage:
// .gh shop list <categoryId>
// =====================================================
bool GuildHouseCommandScript::HandleListCatalog(ChatHandler* handler, char const* args)
{
    Player* player = handler->GetSession()->GetPlayer();
    if (!player)
        return false;
    
    if (!args || !*args)
    {
        handler->PSendSysMessage("Usage: .gh shop list <categoryId>");
        return true;
    }

    uint32 categoryId = atoi(args);
    if (!categoryId)
    {
        handler->PSendSysMessage("Invalid category id.");
        return true;
    }

    std::vector<const GHCatalog*> catalogs = sGuildHouseCatalogMgr.GetCatalogs(categoryId, player->GetTeamId());

    if (catalogs.empty())
    {
        handler->PSendSysMessage("No Guild House items found.");
        return true;
    }

    handler->PSendSysMessage("==== Guild House Catalog ====");

    for (const GHCatalog* catalog : catalogs)
    {
        if (!catalog)
            continue;

        handler->PSendSysMessage("{} - {}", catalog->CatalogId, catalog->Name.c_str());
    }

    return true;
}

// =====================================================
// Purchase Catalog Item
//
// Usage:
// .gh shop buy <catalogId>
//
// Purchase is handled by GuildHouseMgr.
// =====================================================
bool GuildHouseCommandScript::HandlePurchaseCatalog(ChatHandler* handler, char const* args)
{
    Player* player = handler->GetSession()->GetPlayer();
    if (!player)
        return false;

    if (!args || !*args)
    {
        handler->PSendSysMessage("Usage: .gh shop buy <catalogId>");
        return true;
    }

    uint32 catalogId = atoi(args);
    if (!catalogId)
    {
        handler->PSendSysMessage("Invalid catalog id.");
        return true;
    }

    if (!sGuildHouseMgr.PurchaseCatalogItem(player, catalogId))
    {
        handler->PSendSysMessage("Failed purchasing Guild House item.");
        return true;
    }

    handler->PSendSysMessage("Guild House item purchased.");

    return true;
}

// =====================================================
// Script Registration
// =====================================================
void AddSC_GuildHouseCommands()
{
    new GuildHouseCommandScript();
}
