#include "GuildHouseBroker.h"

#include "GuildHouseMgr.h"
#include "GuildHouseConfig.h"
#include "GuildHouseDefines.h"

#include "Guild.h"
#include "GossipDef.h"
#include "ScriptedGossip.h"
#include "Chat.h"
#include "MapMgr.h"

namespace
{

    enum GuildHouseActions
    {
        ACTION_NONE = 0,
        ACTION_BUY_START = 1000,
        ACTION_TELEPORT = 2000,
        ACTION_SELL = 2001
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
            auto locations = sGuildHouseMgr.GetLocations();
    
            if (locations.empty())
            {
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "No Guild House locations are available.", GOSSIP_SENDER_MAIN, ACTION_NONE);
            }
            else
            {
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Purchase a Guild House:", GOSSIP_SENDER_MAIN, ACTION_NONE);
                for (const GHLocation* location : locations)
                {
                    AddGossipItemFor(player, GOSSIP_ICON_CHAT, location->Name, GOSSIP_SENDER_MAIN, ACTION_BUY_START + location->Id);
                }
            }
        }
        else
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Your Guild Master must purchase the Guild House.", GOSSIP_SENDER_MAIN, ACTION_NONE);
        }
    }
    else
    {
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Teleport to Guild House", GOSSIP_SENDER_MAIN, ACTION_TELEPORT);
        if (IsGuildMaster(player))
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Sell Guild House", GOSSIP_SENDER_MAIN, ACTION_SELL);
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

    if (action >= ACTION_BUY_START && action < ACTION_TELEPORT)
    {
        uint32 locationId = action - ACTION_BUY_START;
    
        const GHLocation* location = sGuildHouseMgr.GetLocation(locationId);
        if (!location)
        {
            ChatHandler(player->GetSession()).PSendSysMessage("Invalid Guild House location.");
            CloseGossipMenuFor(player);
            return true;
        }
    
        ChatHandler(player->GetSession()).PSendSysMessage("Selected Guild House location: {}", location->Name);
        CloseGossipMenuFor(player);
        return true;
    }
    
    switch(action)
    {
/*        case ACTION_BUY:
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
        
            if (!sGuildHouseMgr.CreateGuildHouse(guildId, player->GetGUID().GetCounter(), locationId))
            {
                ChatHandler(player->GetSession()).PSendSysMessage("Failed to create Guild House.");
                break;
            }
        
            player->ModifyMoney(-int64(cost));        
            ChatHandler(player->GetSession()).PSendSysMessage("Guild House purchased. The Guild Master may now place the salesman.");
            break;
        }
*/
        case ACTION_TELEPORT:
        {
            if (!sGuildHouseMgr.HasGuildHouse(guildId))
                break;
        
            const GHLocation* location = sGuildHouseMgr.GetGuildLocation(guildId);
            if (!location)
            {
                ChatHandler(player->GetSession()).PSendSysMessage("Your guild house location is invalid.");
                break;
            }
        
            uint32 instanceId = sGuildHouseMgr.GetOrCreateGuildInstance(guildId);
            
            player->TeleportTo(location->MapId, instanceId, location->X, location->Y, location->Z, location->O);
            break;
        }

        case ACTION_NONE:
            return false;

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
