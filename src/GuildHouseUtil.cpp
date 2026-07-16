#include "GuildHouseDefines.h"

#include "GuildHouseMgr.h"
#include "GuildHouseTypes.h"

#include "Player.h"
#include "Guild.h"
#include "Chat.h"


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


    const GHGuildHouse* house =
        sGuildHouseMgr.GetGuildHouse(guildId);


    if (!house)
        return false;


    const GHLocation* location =
        sGuildHouseMgr.GetLocation(house->LocationId);


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


    if (x < location->MinX ||
        x > location->MaxX ||
        y < location->MinY ||
        y > location->MaxY)
    {
        return false;
    }



    //
    // Correct guild instance
    //
    uint32 instanceId = player->GetInstanceId();


    if (!instanceId)
        return false;


    return IsGuildHouseInstance(
        guildId,
        instanceId);
}



// =====================================================
// Guild Instance Validation
// =====================================================

bool IsGuildHouseInstance(uint32 guildId, uint32 instanceId)
{
    if (!instanceId)
        return false;


    return sGuildHouseMgr.IsGuildInstance(
        guildId,
        instanceId);
}


}
