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
    player->PlayerTalkClass->ClearMenus();

    Guild* guild = player->GetGuild();

    if (!guild)
    {
        player->ADD_GOSSIP_ITEM(0, "You are not in a guild.", GOSSIP_SENDER_MAIN, 0);
        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return;
    }

    uint32 guildId = guild->GetId();

    if (!sGuildHouseMgr.HasGuildHouse(guildId))
    {
        player->ADD_GOSSIP_ITEM(0, "Buy Guild House", GOSSIP_SENDER_MAIN, 1);
    }
    else
    {
        player->ADD_GOSSIP_ITEM(0, "Teleport to Guild House", GOSSIP_SENDER_MAIN, 2);
    }

    player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
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
