#include "GuildHouseNpc.h"

#include "GuildHouseMgr.h"
#include "GuildHouseSpawner.h"
#include "GuildHouseConfig.h"

#include "Guild.h"
#include "GuildMgr.h"
#include "GossipDef.h"
#include "ScriptedGossip.h"

namespace
{
    enum GuildHouseActions
    {
        ACTION_NONE      = 0,
        ACTION_BUY       = 1,
        ACTION_TELEPORT  = 2
    };
}

bool GuildHouseNpc::OnGossipHello(Player* player, Creature* creature)
{
    ClearGossipMenuFor(player);

    Guild* guild = player->GetGuild();

    if (!guild)
    {
        AddGossipItemFor(
            player,
            GOSSIP_ICON_CHAT,
            "You must be in a guild.",
            GOSSIP_SENDER_MAIN,
            ACTION_NONE);

        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }

    uint32 guildId = guild->GetId();

    if (!sGuildHouseMgr.HasGuildHouse(guildId))
    {
        AddGossipItemFor(
            player,
            GOSSIP_ICON_CHAT,
            "Purchase Guild House",
            GOSSIP_SENDER_MAIN,
            ACTION_BUY);
    }
    else
    {
        AddGossipItemFor(
            player,
            GOSSIP_ICON_CHAT,
            "Teleport to Guild House",
            GOSSIP_SENDER_MAIN,
            ACTION_TELEPORT);
    }

    SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
    return true;
}

bool GuildHouseNpc::OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
{
    ClearGossipMenuFor(player);

    Guild* guild = player->GetGuild();

    if (!guild)
    {
        CloseGossipMenuFor(player);
        return true;
    }

    uint32 guildId = guild->GetId();

    switch (action)
    {
        case ACTION_BUY:
        {
            if (sGuildHouseMgr.HasGuildHouse(guildId))
            {
                player->GetSession()).PSendSysMessage("Your guild already owns a Guild House.");
                break;
            }

            uint32 cost = sGuildHouseConfig.GetHouseCost();

            if (!player->HasEnoughMoney(cost))
            {
                player->GetSession()).PSendSysMessage("You do not have enough gold.");
                break;
            }

            player->ModifyMoney(-int32(cost));

            sGuildHouseMgr.CreateGuildHouse(guildId);

            sGuildHouseSpawner.SpawnGuild(guildId);

            player->GetSession()).PSendSysMessage("Your Guild House has been purchased!");

            break;
        }

        case ACTION_TELEPORT:
        {
            if (!sGuildHouseMgr.HasGuildHouse(guildId))
                break;

            player->SetPhaseMask(
                GuildHouseUtil::GetGuildHousePhase(guildId),
                true);

            player->TeleportTo(
                GH_MAP,
                GH_X,
                GH_Y,
                GH_Z,
                GH_O);

            break;
        }

        default:
            break;
    }

    CloseGossipMenuFor(player);
    return true;
}
