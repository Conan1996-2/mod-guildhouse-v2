#include "GuildHouseDefines.h"
#include "GuildHouseMgr.h"
#include "GuildHouseTypes.h"

#include "Player.h"
#include "Guild.h"
#include "Log.h"

namespace GuildHouseUtil
{

bool IsInGuildHouse(Player* player)
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

    // Must be on the correct map
    if (player->GetMapId() != location->MapId)
        return false;

    // Must be inside the guild house instance
    if (!sGuildHouseMgr.IsGuildInstance(
            guild->GetId(),
            player->GetInstanceId()))
    {
        return false;
    }

    // Must be inside allowed placement area
    float x = player->GetPositionX();
    float y = player->GetPositionY();

    if (x < location->MinX ||
        x > location->MaxX ||
        y < location->MinY ||
        y > location->MaxY)
    {
        return false;
    }

    return true;
}

}
