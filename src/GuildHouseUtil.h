#ifndef MOD_GUILDHOUSE_UTIL_H
#define MOD_GUILDHOUSE_UTIL_H

#include <cstdint>

class Player;

namespace GuildHouseUtil
{
    // =====================================================
    // Bit flag helper
    // =====================================================

    bool HasFlag(uint32_t value, uint32_t flag);

    // =====================================================
    // Guild permissions
    // =====================================================

    bool IsGuildMaster(Player* player);

    bool CanManageGuildHouse(Player* player);

    // =====================================================
    // Guild House checks
    // =====================================================

    bool IsInGuildHouse(Player* player);

    // =====================================================
    // Phase validation
    // =====================================================

    bool IsGuildHousePhase(uint32_t guildId, uint32_t phaseMask);
}

#endif
