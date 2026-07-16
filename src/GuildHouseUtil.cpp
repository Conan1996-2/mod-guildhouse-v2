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
            return false;

        Guild* guild = player->GetGuild();
        if (!guild)
            return false;

        const GHGuildHouse* house =
            sGuildHouseMgr.GetGuildHouse(guild->GetId());

        if (!house)
            return false;

        const GHLocation* location =
            sGuildHouseMgr.GetLocation(house->LocationId);

        if (!location)
            return false;

        //
        // Must be inside the correct map
        //
        if (player->GetMapId() != location->MapId)
            return false;

        //
        // Must be inside the configured guild house area
        //
        float x = player->GetPositionX();
        float y = player->GetPositionY();

        if (x < location->MinX || x > location->MaxX || y < location->MinY || y > location->MaxY)
        {
            return false;
        }

        //
        // Must be inside this guild's instance
        //
        uint32 instanceId = player->GetInstanceId();

        if (!instanceId)
            return false;

        return sGuildHouseMgr.IsGuildInstance(
            guild->GetId(),
            instanceId);
    }

    bool IsGuildHouseInstance(uint32 guildId, uint32 instanceId)
    {
        if (!instanceId)
            return false;

        return sGuildHouseMgr.IsGuildInstance(guildId, instanceId);
    }

}
