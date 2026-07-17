#include "GuildHouseSalesman.h"

#include "GuildHouseMgr.h"
#include "GuildHouseCatalogMgr.h"
#include "GuildHouseDefines.h"
#include "GuildHousePhaseMgr.h"

#include "Player.h"
#include "Creature.h"
#include "Guild.h"

#include "GossipDef.h"
#include "ScriptedGossip.h"

#include "Chat.h"

namespace
{
    
    enum GuildHouseSalesmanActions
    {
        ACTION_CATEGORY_START = 1000,
        ACTION_CATALOG_START  = 2000
    };

}

// =====================================================
// Validate Access
//
// Salesman exists inside one guild bit phase.
//
// Rules:
// - Player must have guild
// - Guild must own house
// - Player must be inside guild phase
// - Creature must be inside same guild phase
// - Player must be inside boundary
//
// =====================================================
bool GuildHouseSalesman::ValidateSalesmanAccess(Player* player, Creature* creature)
{
    if (!player || !creature)
        return false;

    Guild* guild = player->GetGuild();
    if (!guild)
    {
        ChatHandler(player->GetSession()).PSendSysMessage("You must belong to a guild.");
        return false;
    }

    uint32 guildId = guild->GetId();
    if (!sGuildHouseMgr.HasGuildHouse(guildId))
    {
        ChatHandler(player->GetSession()).PSendSysMessage("Your guild does not own a Guild House.");
        return false;
    }

    //
    // Verify player phase
    //
    uint32 phaseMask = sGuildHouseMgr.GetPhaseMask(guildId);
    if (!phaseMask)
    {
        ChatHandler(player->GetSession()).PSendSysMessage("Guild House phase unavailable.");
        return false;
    }

    if (player->GetPhaseMask() != phaseMask)
    {
        ChatHandler(player->GetSession()).PSendSysMessage("You must be inside your Guild House.");
        return false;
    }

    //
    // Verify salesman phase
    //
    if (creature->GetPhaseMask() != phaseMask)
    {
        ChatHandler(player->GetSession()).PSendSysMessage("This salesman belongs to another Guild House.");
        return false;
    }

    //
    // Boundary check
    //
    if (!sGuildHouseMgr.IsInsideGuildHouseBoundary(guildId, player->GetPositionX(), player->GetPositionY()))
    {
        ChatHandler(player->GetSession()).PSendSysMessage("You are outside the Guild House area.");
        return false;
    }

    return true;
}

// =====================================================
// Gossip Hello
// =====================================================
bool GuildHouseSalesman::OnGossipHello(Player* player, Creature* creature)
{
    ClearGossipMenuFor(player);

    if (!ValidateSalesmanAccess(player, creature))
    {
        CloseGossipMenuFor(player);
        return true;
    }

    SendCatalogMenu(player, creature);

    return true;
}

// =====================================================
// Root Catalog Menu
// =====================================================
void GuildHouseSalesman::SendCatalogMenu(Player* player, Creature* creature)
{
    auto categories = sGuildHouseCatalogMgr.GetRootCategories();

    for (const GHCategory* category : categories)
    {
        if (!category)
            continue;

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, category->Name, GOSSIP_SENDER_MAIN, ACTION_CATEGORY_START + category->Id);
    }

    SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
}

// =====================================================
// Gossip Selection
// =====================================================
bool GuildHouseSalesman::OnGossipSelect(Player* player, Creature* creature, uint32, uint32 action)
{
    ClearGossipMenuFor(player);

    if (!ValidateSalesmanAccess(player, creature))
    {
        CloseGossipMenuFor(player);
        return true;
    }

    if (action >= ACTION_CATEGORY_START && action < ACTION_CATALOG_START)
    {
        uint32 categoryId = action - ACTION_CATEGORY_START;
        SendCategoryMenu(player, creature, categoryId);
        return true;
    }

    if (action >= ACTION_CATALOG_START)
    {
        uint32 catalogId = action - ACTION_CATALOG_START;
        if (!sGuildHouseMgr.PurchaseCatalogItem(player, catalogId))
        {
            ChatHandler(player->GetSession()).PSendSysMessage("Unable to purchase item.");
        }

        CloseGossipMenuFor(player);
        return true;
    }

    CloseGossipMenuFor(player);

    return true;
}

// =====================================================
// Category Menu
// =====================================================
void GuildHouseSalesman::SendCategoryMenu(Player* player, Creature* creature, uint32 categoryId)
{
    //
    // Child categories
    //
    auto children = sGuildHouseCatalogMgr.GetChildCategories( categoryId);

    for (const GHCategory* child : children)
    {
        if (!child)
            continue;

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, child->Name, GOSSIP_SENDER_MAIN, ACTION_CATEGORY_START + child->Id);
    }

    //
    // Catalog items
    //
    auto catalogs = sGuildHouseCatalogMgr.GetCatalogs(categoryId);
    for (const GHCatalog* catalog : catalogs)
    {
        if (!catalog)
            continue;

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, catalog->Name, GOSSIP_SENDER_MAIN, ACTION_CATALOG_START + catalog->CatalogId);
    }

    SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
}

// =====================================================
// Registration
// =====================================================
void AddSC_GuildHouseSalesman()
{
    new GuildHouseSalesman();
}
