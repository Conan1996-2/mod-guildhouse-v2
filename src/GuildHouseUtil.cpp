#include "GuildHouseDefines.h"
#include "GuildHouseMgr.h"
#include "GuildHouseTypes.h"

#include "Player.h"
#include "Guild.h"
#include "Chat.h"


namespace GuildHouseUtil
{
    bool IsInGuildHouse(Player* player)
    {
        if (!player)
        {
            ChatHandler(player->GetSession()).PSendSysMessage("Yournot a player.");
            return false;
        }

        Guild* guild = player->GetGuild();
        if (!guild)
        {
            ChatHandler(player->GetSession()).PSendSysMessage("Yournot in a guild.");
            return false;
        }
    
        const GHGuildHouse* house = sGuildHouseMgr.GetGuildHouse(guild->GetId());
        if (!house)
        {
            ChatHandler(player->GetSession()).PSendSysMessage("Your guild does not own a Guild House.");
            return false;
        }
    
        const GHLocation* location = sGuildHouseMgr.GetLocation(house->LocationId);
        if (!location)
        {
            ChatHandler(player->GetSession()).PSendSysMessage("Your guild house is not a valid id.");
            return false;
        }
        
        if (player->GetMapId() != location->MapId)
        {
            ChatHandler(player->GetSession()).PSendSysMessage("Your not in your guild house.");
            return false;
        }

        float x = player->GetPositionX();
        float y = player->GetPositionY();
        if (x < location->MinX || x > location->MaxX || y < location->MinY || y > location->MaxY)
        {
            ChatHandler(player->GetSession()).PSendSysMessage("Your not standing in the guild house boundries.");
            return false;
        }

        return sGuildHouseMgr.IsGuildInstance(guild->GetId(), player->GetInstanceId());
        
        //uint32 instanceId = player->GetInstanceId();
        //if (!instanceId)
        //    return false;

        //return player->GetInstanceId() == sGuildHouseMgr.GetGuildInstance(guild->GetId());

       // return IsGuildHouseInstance(guild->GetId(), instanceId);
    }
}
