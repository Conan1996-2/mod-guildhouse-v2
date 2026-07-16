#ifndef MOD_GUILDHOUSE_UTIL_H
#define MOD_GUILDHOUSE_UTIL_H

#include <cstdint>

class Player;

namespace GuildHouseUtil
{
    bool HasFlag(uint32_t value, uint32_t flag);

    bool IsGuildMaster(Player* player);

    bool CanManageGuildHouse(Player* player);

    bool IsInGuildHouse(Player* player);

    bool IsGuildHouseInstance(uint32_t guildId, uint32_t instanceId);
}

#endif
