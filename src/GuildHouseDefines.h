#ifndef MOD_GUILDHOUSE_DEFINES_H
#define MOD_GUILDHOUSE_DEFINES_H

#include <cstdint>

// =====================================================
// Core Constants
// =====================================================

constexpr uint32_t GH_PHASE_OFFSET = 100000;
constexpr uint32_t GH_MAP = 1;

// Default fallback spawn position (GM island / housing zone)
constexpr float GH_X = 16222.57f;
constexpr float GH_Y = 16265.91f;
constexpr float GH_Z = 13.21f;
constexpr float GH_O = 0.0f;

// =====================================================
// Spawn Flags (WHAT is spawned)
// Bitmask system (DB-driven)
// =====================================================

enum GHSpawnFlags : uint32_t
{
    GH_SPAWN_NONE        = 0,

    GH_SPAWN_CREATURE   = 1 << 0,
    GH_SPAWN_GAMEOBJECT = 1 << 1,
    GH_SPAWN_PORTAL     = 1 << 2,
    GH_SPAWN_TRIGGER    = 1 << 3
};

// =====================================================
// Behavior Flags (HOW it behaves)
// Bitmask system (DB-driven)
// =====================================================

enum GHBehaviorFlags : uint32_t
{
    GH_BEHAVIOR_NONE        = 0,

    // Faction restrictions
    GH_FACTION_ALLIANCE     = 1 << 0,
    GH_FACTION_HORDE        = 1 << 1,
    GH_FACTION_NEUTRAL      = 1 << 2,

    // NPC roles
    GH_ROLE_VENDOR          = 1 << 3,
    GH_ROLE_REPAIR          = 1 << 4,
    GH_ROLE_BANKER          = 1 << 5,
    GH_ROLE_AUCTIONEER      = 1 << 6,
    GH_ROLE_INNKEEPER       = 1 << 7,
    GH_ROLE_FLIGHTMASTER    = 1 << 8,
    GH_ROLE_STABLEMASTER    = 1 << 9,
    GH_ROLE_TRAINER         = 1 << 10,

    // World objects
    GH_ROLE_MAILBOX         = 1 << 11,
    GH_ROLE_FORGE           = 1 << 12,
    GH_ROLE_ANVIL           = 1 << 13,

    // Rules
    GH_FLAG_UNIQUE          = 1 << 14,
    GH_FLAG_STARTER         = 1 << 15
};

// =====================================================
// Asset Status (persistent state)
// =====================================================

enum GHAssetStatus : uint8_t
{
    GH_ASSET_PURCHASED = 0,
    GH_ASSET_PLACED    = 1,
    GH_ASSET_STORED    = 2,
    GH_ASSET_DISABLED  = 3
};

// =====================================================
// Script Types (runtime behavior hooks)
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
// Utility helpers
// =====================================================

namespace GuildHouseUtil
{
    inline bool HasFlag(uint32_t value, uint32_t flag)
    {
        return (value & flag) != 0;
    }

    inline uint32_t GetGuildHousePhase(uint32_t guildId)
    {
        return guildId + GH_PHASE_OFFSET;
    }

    inline bool IsAlliance(uint32_t flags)
    {
        return HasFlag(flags, GH_FACTION_ALLIANCE);
    }

    inline bool IsHorde(uint32_t flags)
    {
        return HasFlag(flags, GH_FACTION_HORDE);
    }
}

#endif
