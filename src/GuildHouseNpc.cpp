#include "GuildHouseNpc.h"

#include "Guild.h"
#include "GuildMgr.h"
#include "GuildHouseConfig.h"
#include "GuildHouseMgr.h"
#include "GuildHouseSpawner.h"

bool GuildHouseNpc::OnGossipHello(Player* player, Creature* creature)
{
    SendMainMenu(player, creature);
    return true;
}

void GuildHouseNpc::SendMainMenu(Player* player, Creature* creature)
{
    ClearGossipMenuFor(player);

    Guild* guild = player->GetGuild();

    if (!guild)
    {
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "You are not in a guild.", GOSSIP_SENDER_MAIN, 0);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return;
    }

    uint32 guildId = guild->GetId();

    if (!sGuildHouseMgr.HasGuildHouse(guildId))
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Buy Guild House", GOSSIP_SENDER_MAIN, 1);
    else
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Teleport to Guild House", GOSSIP_SENDER_MAIN, 2);

    SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
}

bool GuildHouseNpc::OnGossipSelect(Player* player, Creature* creature, uint32, uint32 action)
{
    player->PlayerTalkClass->ClearMenus();

    Guild* guild = player->GetGuild();
    if (!guild)
        return true;

    uint32 guildId = guild->GetId();

    // -------------------------------------------------
    // BUY HOUSE
    // -------------------------------------------------
    if (action == 1)
    {
        uint32 cost = sGuildHouseConfig.GetHouseCost();

        if (!player->HasEnoughMoney(cost))
        {
            player->SendBroadcastMessage("Not enough gold.");
            return true;
        }

        player->ModifyMoney(-int32(cost));

        sGuildHouseMgr.CreateGuildHouse(guildId);

        sGuildHouseSpawner.SpawnGuild(guildId);

        player->SendBroadcastMessage("Guild House purchased!");
    }

    // -------------------------------------------------
    // TELEPORT
    // -------------------------------------------------
    if (action == 2)
    {
        if (!sGuildHouseMgr.HasGuildHouse(guildId))
            return true;

        uint32 phase = GuildHouseUtil::GetGuildHousePhase(guildId);

        player->SetPhaseMask(phase, true);
        player->TeleportTo(1, GH_X, GH_Y, GH_Z, GH_O);
    }

    return true;
}

class GuildHouseNpcLoader
{
public:
    GuildHouseNpcLoader()
    {
        new GuildHouseNpc();
    }
};

static GuildHouseNpcLoader loader;
