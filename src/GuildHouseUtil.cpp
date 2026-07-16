#include "GuildHouseDefines.h"
#include "GuildHouseMgr.h"
#include "GuildHouseTypes.h"

#include "Player.h"
#include "Guild.h"


    inline bool IsInGuildHouse(Player* player)
    {
        if (!player)
            return false;

        Guild* guild = player->GetGuild();
        if (!guild)
            return false;
    
        const GHGuildHouse* house = sGuildHouseMgr.GetGuildHouse(guild->GetId());
        if (!house)
            return false;
    
        const GHLocation* location = sGuildHouseMgr.GetLocation(house->LocationId);
        if (!location)
            return false;
        
        if (player->GetMapId() != location->MapId)
            return false;

        float x = player->GetPositionX();
        float y = player->GetPositionY();
        if (x < location->MinX || x > location->MaxX || y < location->MinY || y > location->MaxY)
            return false;

        return sGuildHouseMgr.IsGuildInstance(guild->GetId(), player->GetInstanceId());
        
        //uint32 instanceId = player->GetInstanceId();
        //if (!instanceId)
        //    return false;

        //return player->GetInstanceId() == sGuildHouseMgr.GetGuildInstance(guild->GetId());

       // return IsGuildHouseInstance(guild->GetId(), instanceId);
    }

