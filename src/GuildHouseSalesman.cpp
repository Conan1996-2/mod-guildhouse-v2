#include "GuildHouseSalesman.h"

#include "GuildHouseMgr.h"
#include "GuildHouseCatalogMgr.h"
#include "GuildHouseDefines.h"

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

bool GuildHouseSalesman::ValidateSalesmanAccess(
    Player* player,
    Creature* creature)
{
    if (!player || !creature)
        return false;

    Guild* guild =
        player->GetGuild();

    if (!guild)
    {
        ChatHandler(player->GetSession())
            .PSendSysMessage(
                "You must belong to a guild.");

        return false;
    }

    if (guild->GetLeaderGUID() != player->GetGUID())
    {
        ChatHandler(player->GetSession())
            .PSendSysMessage(
                "Only the Guild Master may use the Guild House salesman.");
    
        return false;
    }
    
    uint32 guildId =
        guild->GetId();

    if (!sGuildHouseMgr.HasGuildHouse(guildId))
    {
        ChatHandler(player->GetSession())
            .PSendSysMessage(
                "Your guild does not own a Guild House.");

        return false;
    }

    uint32 phase =
        GuildHouseUtil::GetGuildHousePhase(guildId);

    //
    // Player must be in guild phase
    //

    if ((player->GetPhaseMask() & phase) == 0)
    {
        ChatHandler(player->GetSession())
            .PSendSysMessage(
                "You must be inside your Guild House phase.");

        return false;
    }

    //
    // Player must be on GM Island
    //

    if (!GuildHouseUtil::IsOnGMIsland(player))
    {
        ChatHandler(player->GetSession())
            .PSendSysMessage(
                "Guild House items may only be purchased on GM Island.");

        return false;
    }

    //
    // Salesman must also be in correct phase
    //

    if ((creature->GetPhaseMask() & phase) == 0)
    {
        ChatHandler(player->GetSession())
            .PSendSysMessage(
                "This Guild House salesman is incorrectly phased.");

        return false;
    }

    //
    // Salesman must be on GM Island
    //

    if (creature->GetMapId() != GH_MAP)
    {
        ChatHandler(player->GetSession())
            .PSendSysMessage(
                "Guild House salesman is not on GM Island.");

        return false;
    }

    return true;
}

bool GuildHouseSalesman::OnGossipHello(
    Player* player,
    Creature* creature)
{
    ClearGossipMenuFor(player);

    if (!ValidateSalesmanAccess(
            player,
            creature))
    {
        CloseGossipMenuFor(player);
        return true;
    }

    SendCatalogMenu(
        player,
        creature);

    return true;
}

void GuildHouseSalesman::SendCatalogMenu(
    Player* player,
    Creature* creature)
{
    auto categories =
        sGuildHouseCatalogMgr.GetRootCategories();

    for (const GHCategory* category :
         categories)
    {
        AddGossipItemFor(
            player,
            GOSSIP_ICON_CHAT,
            category->Name,
            GOSSIP_SENDER_MAIN,
            ACTION_CATEGORY_START + category->Id);
    }

    SendGossipMenuFor(
        player,
        DEFAULT_GOSSIP_MESSAGE,
        creature->GetGUID());
}

bool GuildHouseSalesman::OnGossipSelect(
    Player* player,
    Creature* creature,
    uint32,
    uint32 action)
{
    ClearGossipMenuFor(player);

    if (!ValidateSalesmanAccess(
            player,
            creature))
    {
        CloseGossipMenuFor(player);
        return true;
    }

    if (action >= ACTION_CATEGORY_START &&
        action < ACTION_CATALOG_START)
    {
        uint32 categoryId =
            action - ACTION_CATEGORY_START;

        SendCategoryMenu(
            player,
            creature,
            categoryId);

        return true;
    }

    if (action >= ACTION_CATALOG_START)
    {
        uint32 catalogId =
            action - ACTION_CATALOG_START;

        sGuildHouseMgr.PurchaseCatalogItem(
            player,
            catalogId);

        CloseGossipMenuFor(player);

        return true;
    }

    CloseGossipMenuFor(player);

    return true;
}

void GuildHouseSalesman::SendCategoryMenu(
    Player* player,
    Creature* creature,
    uint32 categoryId)
{
    //
    // Child categories
    //

    auto children =
        sGuildHouseCatalogMgr.GetChildCategories(
            categoryId);

    for (const GHCategory* child :
         children)
    {
        AddGossipItemFor(
            player,
            GOSSIP_ICON_CHAT,
            child->Name,
            GOSSIP_SENDER_MAIN,
            ACTION_CATEGORY_START + child->Id);
    }

    //
    // Items
    //

    auto catalogs =
        sGuildHouseCatalogMgr.GetCatalogs(
            categoryId);

    for (const GHCatalog* catalog :
         catalogs)
    {
        AddGossipItemFor(
            player,
            GOSSIP_ICON_CHAT,
            catalog->Name,
            GOSSIP_SENDER_MAIN,
            ACTION_CATALOG_START + catalog->CatalogId);
    }

  SendGossipMenuFor(
        player,
        DEFAULT_GOSSIP_MESSAGE,
        creature->GetGUID());
}

void AddSC_GuildHouseSalesman()
{
    new GuildHouseSalesman();
}
