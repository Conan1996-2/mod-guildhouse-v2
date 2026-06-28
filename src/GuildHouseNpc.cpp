#include "GuildHouseNpc.h"
#include "GuildHouseMgr.h"

bool npc_guildhouse::OnGossipHello(Player* player, Creature* creature)
{
    if (!player->GetGuildId())
    {
        player->ADD_GOSSIP_ITEM(0, "You must be in a guild.", GOSSIP_SENDER_MAIN, 0);
        player->SEND_GOSSIP_MENU(1, creature->GetGUID());
        return true;
    }

    uint32 guildId = player->GetGuildId();

    if (!sGuildHouseMgr->HasHouse(guildId))
    {
        player->ADD_GOSSIP_ITEM(0, "Purchase Guild House", GOSSIP_SENDER_MAIN, GH_GOSSIP_BUY);
    }
    else
    {
        player->ADD_GOSSIP_ITEM(0, "Enter Guild House", GOSSIP_SENDER_MAIN, GH_GOSSIP_TELEPORT);
    }

    player->SEND_GOSSIP_MENU(1, creature->GetGUID());
    return true;
}

bool npc_guildhouse::OnGossipSelect(Player* player, Creature* creature, uint32, uint32 action)
{
    player->PlayerTalkClass->ClearMenus();

    if (action == GH_GOSSIP_BUY)
    {
        sGuildHouseMgr->Purchase(player);
    }
    else if (action == GH_GOSSIP_TELEPORT)
    {
        sGuildHouseMgr->Teleport(player);
    }

    player->CLOSE_GOSSIP_MENU();
    return true;
}
