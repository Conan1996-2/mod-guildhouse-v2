#include "GuildHouseUtil.h"

#include "GuildHouseMgr.h"
#include "GuildHousePhaseMgr.h"

#include "GuildHouseTypes.h"

#include "Guild.h"
#include "Player.h"

namespace GuildHouseUtil
{
    
    // =====================================================
    // Is Player Inside Guild House
    // =====================================================
    bool IsInGuildHouse(Player* player)
    {
        if (!player)
            return false;
    
        Guild* guild = player->GetGuild();
        if (!guild)
            return false;
    
        uint32 guildId = guild->GetId();
    
        const GHGuildHouse* house = sGuildHouseMgr.GetGuildHouse(guildId);
        if (!house)
            return false;
    
        const GHLocation* location = sGuildHouseMgr.GetLocation(house->LocationId);
        if (!location)
            return false;
    
        //
        // Correct map
        //
        if (player->GetMapId() != location->MapId)
            return false;
    
        //
        // Correct coordinates
        //
        float x = player->GetPositionX();
        float y = player->GetPositionY();
    
        if (x < location->MinX || x > location->MaxX || y < location->MinY || y > location->MaxY)
        {
            return false;
        }
    
        //
        // Correct guild phase
        //
        return sGuildHousePhaseMgr.IsMember(player);
    }
    
    // =====================================================
    // Guild Phase Validation
    // =====================================================
    
    bool IsGuildHousePhase(uint32_t guildId, uint32_t phaseMask)
    {
        uint32_t guildPhase = sGuildHousePhaseMgr.GetPhaseMask(guildId);
        if (!guildPhase)
            return false;
    
        return (phaseMask & guildPhase) != 0;
    }
    
}
