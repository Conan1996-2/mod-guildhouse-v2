#ifndef MOD_GUILDHOUSE_DEFINES_H
#define MOD_GUILDHOUSE_DEFINES_H

#include <cstdint>
#include "Player.h"
#include "Guild.h"

// =====================================================
// Core Constants
// =====================================================

// Default Guild House location
constexpr uint32_t GH_MAP = 1;
constexpr float GH_X = 16222.57f;
constexpr float GH_Y = 16265.91f;
constexpr float GH_Z = 13.21f;
constexpr float GH_O = 0.0f;

// Other areas: Azshara Crater x=128, y=135, z=236.1, map=37
// horde  Valormok 88.38 915.82 338.12 37
// either Talrendis Outpost 1144.43 240 353 37


// =====================================================
// GM Island Placement Boundary
//
// Salesman and Guild House objects may only be placed
// inside this area.
// =====================================================

constexpr float GH_GM_ISLAND_MIN_X = 16000.0f;
constexpr float GH_GM_ISLAND_MAX_X = 16500.0f;
constexpr float GH_GM_ISLAND_MIN_Y = 16000.0f;
constexpr float GH_GM_ISLAND_MAX_Y = 16500.0f;

// =====================================================
// Spawn Flags
//
// Defines WHAT permanent world object is created.
//
// Stored in database as INT UNSIGNED.
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
//
// Defines WHAT the object does.
//
// Stored in database as INT UNSIGNED.
// =====================================================

enum GHBehaviorFlags : uint32_t
{
    GH_BEHAVIOR_NONE = 0,

    // Faction
    GH_FACTION_ALLIANCE = 1 << 0,
    GH_FACTION_HORDE    = 1 << 1,
    GH_FACTION_NEUTRAL  = 1 << 2,

    // NPC Services
    GH_ROLE_VENDOR       = 1 << 3,
    GH_ROLE_REPAIR       = 1 << 4,
    GH_ROLE_BANKER       = 1 << 5,
    GH_ROLE_AUCTIONEER   = 1 << 6,
    GH_ROLE_INNKEEPER    = 1 << 7,
    GH_ROLE_FLIGHTMASTER = 1 << 8,
    GH_ROLE_STABLEMASTER = 1 << 9,
    GH_ROLE_TRAINER      = 1 << 10,

    // Objects
    GH_ROLE_MAILBOX = 1 << 11,
    GH_ROLE_FORGE   = 1 << 12,
    GH_ROLE_ANVIL   = 1 << 13,

    // Rules
    GH_FLAG_UNIQUE  = 1 << 14,
    GH_FLAG_STARTER = 1 << 15
};

// =====================================================
// Persistent Asset Status
//
// Stored in guildhouse_asset.status
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
//
// Used for special permanent objects.
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

    bool IsGuildHouseInstance(uint32 guildId, uint32 instanceId);
    
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

    inline bool IsInGuildHouse(Player* player)
    {
        if (!player)
            return false;

        if (player->GetMapId() != GH_MAP)
            return false;

        Guild* guild = player->GetGuild();
        if (!guild)
            return false;

        float x = player->GetPositionX();
        float y = player->GetPositionY();
        if (x < GH_GM_ISLAND_MIN_X || x > GH_GM_ISLAND_MAX_X || y < GH_GM_ISLAND_MIN_Y || y > GH_GM_ISLAND_MAX_Y)
            return false;

        uint32 instanceId = player->GetInstanceId();
        if (!instanceId)
            return false;

        return IsGuildHouseInstance(guild->GetId(), instanceId);
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
