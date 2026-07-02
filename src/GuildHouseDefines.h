#ifndef MOD_GUILDHOUSE_DEFINES_H
#define MOD_GUILDHOUSE_DEFINES_H

#include <cstdint>

// =====================================================
// Guild House Constants 12
// =====================================================

constexpr uint32_t GH_PHASE_OFFSET = 100000;

// =====================================================
// Spawn Flags
// Determines WHAT the component spawns.
// =====================================================

enum class GHSpawnFlags : uint32_t
{
    None        = 0,

    Creature    = 1 << 0,
    GameObject  = 1 << 1,
    Portal      = 1 << 2,
    Trigger     = 1 << 3
};

// =====================================================
// Behavior Flags
// Determines HOW the component behaves.
// =====================================================

enum class GHBehaviorFlags : uint32_t
{
    None            = 0,

    // Faction
    Alliance        = 1 << 0,
    Horde           = 1 << 1,
    Neutral         = 1 << 2,

    // NPC Roles
    Vendor          = 1 << 3,
    Repair          = 1 << 4,
    Banker          = 1 << 5,
    Auctioneer      = 1 << 6,
    Innkeeper       = 1 << 7,
    FlightMaster    = 1 << 8,
    StableMaster    = 1 << 9,
    Trainer         = 1 << 10,

    // Objects
    Mailbox         = 1 << 11,
    Forge           = 1 << 12,
    Anvil           = 1 << 13,

    // Rules
    Unique          = 1 << 14,
    Starter         = 1 << 15
};

// =====================================================
// Asset Status
// =====================================================

enum class GHAssetStatus : uint8_t
{
    Purchased = 0,
    Placed    = 1,
    Stored    = 2,
    Disabled  = 3
};

// =====================================================
// Script Types
// =====================================================

enum class GHScriptType : uint8_t
{
    None = 0,

    Portal,
    Buff,
    Trigger,
    Music,
    Custom
};

// =====================================================
// Bitmask Operators
// =====================================================

namespace GuildHouseUtil
{
    template<typename T>
    inline bool HasFlag(T value, T flag)
    {
        return (static_cast<uint32_t>(value) &
                static_cast<uint32_t>(flag)) != 0;
    }

    inline uint32_t GetGuildHousePhase(uint32_t guildId)
    {
        return guildId + GH_PHASE_OFFSET;
    }
}

#endif
