#include "GuildHouseNpc.h"

#include "Guild.h"
#include "GuildMgr.h"
#include "GuildHouseConfig.h"
#include "GuildHouseMgr.h"
#include "GuildHouseSpawner.h"

#include "ScriptMgr.h"
#include "Player.h"
#include "Creature.h"
#include "ScriptPCH.h"

bool GuildHouseNpc::OnGossipHello(Player* player, Creature* creature)
{
    player->PlayerTalkClass->ClearMenus();

    Guild* guild = player->GetGuild();

    if (!guild)
    {
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "You are not in a guild.", GOSSIP_SENDER_MAIN, 0);
        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }

    uint32 guildId = guild->GetId();

    if (!sGuildHouseMgr.HasGuildHouse(guildId))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Buy Guild House", GOSSIP_SENDER_MAIN, 1);
    else
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Guild House", GOSSIP_SENDER_MAIN, 2);

    player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
    return true;
}

void GuildHouseNpc::SendMainMenu(Player* player, Creature* creature)
{
    player->PlayerTalkClass->ClearMenus();

    Guild* guild = player->GetGuild();

    if (!guild)
    {
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "You are not in a guild.", GOSSIP_SENDER_MAIN, 0);
        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return;
    }

    uint32 guildId = guild->GetId();

    if (!sGuildHouseMgr.HasGuildHouse(guildId))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Buy Guild House", GOSSIP_SENDER_MAIN, 1);
    else
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Guild House", GOSSIP_SENDER_MAIN, 2);

    player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
}

bool GuildHouseNpc::OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
{
    player->PlayerTalkClass->ClearMenus();

    Guild* guild = player->GetGuild();
    if (!guild)
        return true;

    uint32 guildId = guild->GetId();

    switch (action)
    {
        // -----------------------------------------
        // BUY GUILD HOUSE
        // -----------------------------------------
        case 1:
        {
            uint32 cost = sGuildHouseConfig.GetHouseCost();

            if (player->GetMoney() < cost)
            {
                player->SendBroadcastMessage("Not enough gold.");
                return true;
            }

            player->ModifyMoney(-int32(cost));

            if (!sGuildHouseMgr.HasGuildHouse(guildId))
            {
                sGuildHouseMgr.CreateGuildHouse(guildId);
                sGuildHouseSpawner.SpawnGuild(guildId);

                player->SendBroadcastMessage("Guild House purchased!");
            }

            break;
        }

        // -----------------------------------------
        // TELEPORT
        // -----------------------------------------
        case 2:
        {
            if (!sGuildHouseMgr.HasGuildHouse(guildId))
                return true;

            uint32 phase = GuildHouseUtil::GetGuildHousePhase(guildId);

            player->SetPhaseMask(phase, true);

            player->TeleportTo(1, GH_X, GH_Y, GH_Z, GH_O);

            break;
        }
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
