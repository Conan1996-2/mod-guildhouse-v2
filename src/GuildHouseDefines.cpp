#include "GuildHouseDefines.h"

#include "GuildHouseMgr.h"

bool GuildHouseUtil::IsInGuildHouse(Player* player)
{
    if (!player)
        return false;

    Guild* guild = player->GetGuild();

    if (!guild)
        return false;

    const GHGuildHouse* house = sGuildHouseMgr.GetGuildHouse(guild->GetId());

    if (!house)
        return false;

    uint32 instanceId = player->GetInstanceId();

    if (!instanceId)
        return false;

    return house->InstanceId == instanceId;
}
