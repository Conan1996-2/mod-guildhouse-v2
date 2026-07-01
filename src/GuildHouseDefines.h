#ifndef MOD_GUILDHOUSE_DEFINES_H
#define MOD_GUILDHOUSE_DEFINES_H

#include <cstdint>

// ===============================
// Phase system
// ===============================
constexpr uint32_t GH_PHASE_OFFSET = 10000;

// ===============================
// Spawn flags (what to spawn)
// ===============================
enum class GHSpawnFlags : uint32_t
{
    None        = 0,

    Creature    = 1 << 0,
    GameObject  = 1 << 1,
    Portal      = 1 << 2,
    Trigger     = 1 << 3
};

// ===============================
// Behavior flags (rules & filters)
// ===============================
enum class GHBehaviorFlags : uint32_t
{
    None            = 0,

    Alliance        = 1 << 0,
    Horde           = 1 << 1,
    Neutral         = 1 << 2,

    Vendor          = 1 << 3,
    Repair          = 1 << 4,
    Banker          = 1 << 5,
    Auctioneer      = 1 << 6,

    Mailbox         = 1 << 7,
    Forge           = 1 << 8,
    Anvil           = 1 << 9,

    Innkeeper       = 1 << 10,
    FlightMaster    = 1 << 11,

    StableMaster    = 1 << 12,
    Trainer         = 1 << 13,

    Unique          = 1 << 14,
    Starter         = 1 << 15
};

// ===============================
// Instance state (runtime tracking)
// ===============================
enum class GHInstanceState : uint8_t
{
    Active      = 0,
    Hidden      = 1,
    Disabled    = 2,
    Pending     = 3
};

// ===============================
// Script types for components
// ===============================
enum class GHScriptType : uint8_t
{
    None        = 0,
    Portal      = 1,
    Buff        = 2,
    Music       = 3,
    Trigger     = 4,
    Custom      = 5
};

#endif
