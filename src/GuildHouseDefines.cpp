#include "GuildHouseDefines.h"

#include "GuildHouseMgr.h"
#include "GuildHousePhaseMgr.h"
#include "GuildHouseTypes.h"

#include "Player.h"
#include "Guild.h"


namespace GuildHouseUtil
{

// =====================================================
// Is Player Inside Guild House
//
// Phase based validation.
//
// Checks:
// - Guild membership
// - Guild house ownership
// - Correct map
// - Location boundary
// - Phase mask
// =====================================================

bool IsInGuildHouse(Player* player)
{
    if (!player)
        return false;


    Guild* guild = player->GetGuild();

    if (!guild)
        return false;


    uint32_t guildId = guild->GetId();


    const GHGuildHouse* house =
        sGuildHouseMgr.GetGuildHouse(guildId);


    if (!house)
        return false;


    const GHLocation* location =
        sGuildHouseMgr.GetLocation(
            house->LocationId);


    if (!location)
        return false;


    //
    // Must be in the correct map
    //
    if (player->GetMapId() != location->MapId)
        return false;


    //
    // Must be inside purchased location boundary
    //
    float x = player->GetPositionX();
    float y = player->GetPositionY();


    if (x < location->MinX ||
        x > location->MaxX ||
        y < location->MinY ||
        y > location->MaxY)
    {
        return false;
    }


    //
    // Must have guild phase active
    //
    uint32_t phaseMask =
        sGuildHousePhaseMgr.GetPhaseMask(guildId);


    if (!phaseMask)
        return false;


    return (player->GetPhaseMask() & phaseMask) != 0;
}

}
