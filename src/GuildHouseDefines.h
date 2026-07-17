#ifndef MOD_GUILDHOUSE_DEFINES_H
#define MOD_GUILDHOUSE_DEFINES_H

#include <cstdint>

#include "Player.h"
#include "Guild.h"

// =====================================================
// Spawn Flags
// =====================================================

enum GHSpawnFlags : uint32_t
{
    GH_SPAWN_NONE        = 0,

    GH_SPAWN_CREATURE    = 1 << 0,
    GH_SPAWN_GAMEOBJECT  = 1 << 1,
    GH_SPAWN_PORTAL      = 1 << 2,
    GH_SPAWN_TRIGGER     = 1 << 3
};

// =====================================================
// Behavior Flags
// =====================================================

enum GHBehaviorFlags : uint32_t
{
    GH_BEHAVIOR_NONE = 0,

    GH_FACTION_ALLIANCE = 1 << 0,
    GH_FACTION_HORDE    = 1 << 1,
    GH_FACTION_NEUTRAL  = 1 << 2,

    GH_ROLE_VENDOR       = 1 << 3,
    GH_ROLE_REPAIR       = 1 << 4,
    GH_ROLE_BANKER       = 1 << 5,
    GH_ROLE_AUCTIONEER   = 1 << 6,
    GH_ROLE_INNKEEPER    = 1 << 7,
    GH_ROLE_FLIGHTMASTER = 1 << 8,
    GH_ROLE_STABLEMASTER = 1 << 9,
    GH_ROLE_TRAINER      = 1 << 10,

    GH_ROLE_MAILBOX = 1 << 11,
    GH_ROLE_FORGE   = 1 << 12,
    GH_ROLE_ANVIL   = 1 << 13,

    GH_FLAG_UNIQUE  = 1 << 14,
    GH_FLAG_STARTER = 1 << 15
};

// =====================================================
// Asset Status
// =====================================================

enum GHAssetStatus : uint8_t
{
    GH_ASSET_PURCHASED = 0,
    GH_ASSET_PLACED    = 1,
    GH_ASSET_STORED    = 2,
    GH_ASSET_DISABLED  = 3
};

// =====================================================
// Script Types
// =====================================================

enum GHScriptType : uint32_t
{
    GH_SCRIPT_NONE = 0,

    GH_SCRIPT_PORTAL,
    GH_SCRIPT_BUFF,
    GH_SCRIPT_TRIGGER,
    GH_SCRIPT_MUSIC,
    GH_SCRIPT_CUSTOM
};

// =====================================================
// Utility Helpers
// =====================================================

namespace GuildHouseUtil
{
    bool IsGuildHousePhase(uint32_t guildId, uint32_t phaseMask);

    bool IsInGuildHouse(Player* player);

    inline bool HasFlag(uint32_t value, uint32_t flag)
    {
        return (value & flag) != 0;
    }

    inline bool IsAlliance(uint32_t flags)
    {
        return HasFlag(flags, GH_FACTION_ALLIANCE);
    }

    inline bool IsHorde(uint32_t flags)
    {
        return HasFlag(flags, GH_FACTION_HORDE);
    }

    inline bool IsNeutral(uint32_t flags)
    {
        return HasFlag(flags, GH_FACTION_NEUTRAL);
    }

    inline bool IsGuildMaster(Player* player)
    {
        if (!player)
            return false;

        Guild* guild = player->GetGuild();

        if (!guild)
            return false;

        return guild->GetLeaderGUID() == player->GetGUID();
    }

    inline bool CanManageGuildHouse(Player* player)
    {
        return IsGuildMaster(player) && IsInGuildHouse(player);
    }
}

#endif
