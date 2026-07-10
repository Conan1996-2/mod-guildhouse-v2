#include "GuildHouseBroker.h"

#include "GuildHouseMgr.h"
#include "GuildHouseConfig.h"
#include "GuildHouseDefines.h"

#include "Guild.h"
#include "GossipDef.h"
#include "ScriptedGossip.h"
#include "Chat.h"

namespace
{

    enum GuildHouseActions
    {
        ACTION_NONE = 0,
        ACTION_BUY = 1,
        ACTION_TELEPORT = 2
    };

}

bool GuildHouseBroker::IsGuildMaster(Player* player)
{
    if (!player)
        return false;

    Guild* guild = player->GetGuild();
    if (!guild)
        return false;

    return guild->GetLeaderGUID() == player->GetGUID();
}

bool GuildHouseBroker::OnGossipHello(Player* player, Creature* creature)
{
    ClearGossipMenuFor(player);

    Guild* guild = player->GetGuild();
    if (!guild)
    {
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "You must be in a guild.", GOSSIP_SENDER_MAIN, ACTION_NONE);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }

    uint32 guildId = guild->GetId();
    if (!sGuildHouseMgr.HasGuildHouse(guildId))
    {
        if (IsGuildMaster(player))
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Purchase Guild House", GOSSIP_SENDER_MAIN, ACTION_BUY);
        }
        else
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Your Guild Master must purchase the Guild House.", GOSSIP_SENDER_MAIN, ACTION_NONE);
        }
    }
    else
    {
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Teleport to Guild House", GOSSIP_SENDER_MAIN, ACTION_TELEPORT);
    }

    SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
    return true;
}

bool GuildHouseBroker::OnGossipSelect(Player* player, Creature* creature, uint32, uint32 action)
{
    ClearGossipMenuFor(player);

    Guild* guild = player->GetGuild();
    if (!guild)
    {
        CloseGossipMenuFor(player);
        return true;
    }

    uint32 guildId = guild->GetId();
    switch(action)
    {
        case ACTION_BUY:
        {
            if (!IsGuildMaster(player))
            {
                ChatHandler(player->GetSession()).PSendSysMessage("Only the Guild Master may purchase a Guild House.");
                break;
            }
        
            if (sGuildHouseMgr.HasGuildHouse(guildId))
            {
                ChatHandler(player->GetSession()).PSendSysMessage("Your guild already owns a Guild House.");
                break;
            }
  
            uint64 cost = sGuildHouseConfig.GetHouseCost();
            if (!player->HasEnoughMoney(uint32(cost)))
            {
                ChatHandler(player->GetSession()).PSendSysMessage("You do not have enough gold.");
                break;
            }
        
            if (!sGuildHouseMgr.CreateGuildHouse(guildId, player->GetGUID().GetCounter()))
            {
                ChatHandler(player->GetSession()).PSendSysMessage("Failed to create Guild House.");
                break;
            }
        
            player->ModifyMoney(-int64(cost));        
            ChatHandler(player->GetSession()).PSendSysMessage("Guild House purchased. The Guild Master may now place the salesman.");
            break;
        }

        case ACTION_TELEPORT:
        {
            if (!sGuildHouseMgr.HasGuildHouse(guildId))
                break;

            player->SetPhaseMask(GuildHouseUtil::GetGuildHousePhase(guildId), true);
            player->TeleportTo(GH_MAP, GH_X, GH_Y, GH_Z, GH_O);
            break;
        }

        default:
            break;
    }

    CloseGossipMenuFor(player);

    return true;
}

void AddSC_GuildHouseBroker()
{
    new GuildHouseBroker();
}
