#ifndef MOD_GUILDHOUSE_TYPES_H
#define MOD_GUILDHOUSE_TYPES_H

#include <cstdint>
#include <string>
#include <vector>
#include <type_traits>
#include <unordered_set>

#include "GuildHouseDefines.h"

// =====================================================
// Catalog Category
// =====================================================
struct GHCategory
{
    uint32_t Id = 0;
    uint32_t ParentId = 0;
    std::string Name;
    uint16_t SortOrder = 0;
    bool Enabled = false;
};

// =====================================================
// Catalog Component
//
// Individual component belonging to a catalog item.
// =====================================================
struct GHCatalogAsset
{
    uint32_t ComponentId = 0;
    uint32_t CatalogId = 0;
    uint32_t Entry = 0;
    uint32_t DisplayId = 0;
    float Scale = 1.0f;

    GHSpawnFlags SpawnFlags = GH_SPAWN_NONE;
    GHBehaviorFlags BehaviorFlags = GH_BEHAVIOR_NONE;
    GHScriptType ScriptType = GH_SCRIPT_NONE;

    std::string ScriptData;

    float XOffset = 0.0f;
    float YOffset = 0.0f;
    float ZOffset = 0.0f;
    float OOffset = 0.0f;

    uint32_t TargetMap = 0;
    float TargetX = 0.0f;
    float TargetY = 0.0f;
    float TargetZ = 0.0f;
    float TargetO = 0.0f;

    uint32_t ChildCatalogId = 0;
    uint16_t SortOrder = 0;
};

// =====================================================
// Catalog Item
// =====================================================
struct GHCatalog
{
    uint32_t CatalogId = 0;
    uint32_t CategoryId = 0;
    std::string Name;

    GHSpawnFlags SpawnFlags = GH_SPAWN_NONE;
    GHBehaviorFlags BehaviorFlags = GH_BEHAVIOR_NONE;

    bool Enabled = false;

    std::vector<GHCatalogAsset> Components;
};

// =====================================================
// Purchased Guild Asset
//
// Stored in guildhouse_asset
//
// Phase based.
// =====================================================
struct GHGuildAsset
{
    uint32_t AssetId = 0;
    uint32_t GuildId = 0;
    uint32_t CatalogId = 0;

    uint16_t LayoutId = 0;

    GHAssetStatus Status = GH_ASSET_PURCHASED;

    float X = 0.0f;
    float Y = 0.0f;
    float Z = 0.0f;
    float O = 0.0f;
};

// =====================================================
// Existing Spawns
//
// Stored in guildhouse_spawn
//
// Phase based.
// =====================================================
struct GHGuildSpawn
{
    uint32_t SpawnId = 0;
    uint32_t GuildId = 0;
    uint32_t AssetId = 0;
    uint32_t PhaseMask = 0;
    uint32_t SpawnGuid = 0;
    uint8_t SpawnType = 0;

    uint32_t MapId = 0;
    float X = 0.0f;
    float Y = 0.0f;
    float Z = 0.0f;
    float O = 0.0f;
}

// =====================================================
// Guild Phase Record
//
// Replaces Guild Instance.
//
// One phase belongs to one guild.
//
// The phase mask is the guild house instance.
// =====================================================
struct GHPhaseRecord
{
    uint32_t GuildId = 0;
    uint32_t PhaseMask = 0;
    uint32_t MapId = 0;

    float X = 0.0f;
    float Y = 0.0f;
    float Z = 0.0f;
    float O = 0.0f;

    // Boundary
    float MinX = 0.0f;
    float MaxX = 0.0f;
    float MinY = 0.0f;
    float MaxY = 0.0f;

    // Online players currently inside
    std::unordered_set<uint64_t> Members;
};

// =====================================================
// Permanent Spawn Registry
//
// Stored in guildhouse_spawn
//
// Phase based.
// =====================================================
struct GHSpawnRecord
{
    uint32_t SpawnId = 0;
    uint32_t GuildId = 0;
    uint32_t AssetId = 0;
    uint32_t CatalogId = 0;
    uint32_t Guid = 0;

    GHSpawnFlags Type = GH_SPAWN_NONE;

    uint32_t MapId = 0;
    uint32_t PhaseMask = 0;
    float X = 0.0f;
    float Y = 0.0f;
    float Z = 0.0f;
    float O = 0.0f;
};

// =====================================================
// Guild House Ownership
//
// One house per guild.
//
// Phase owns the house.
// =====================================================
struct GHGuildHouse
{
    uint32_t GuildId = 0;
    uint32_t OwnerGuid = 0;
    uint32_t LocationId = 0;
    uint32_t PhaseMask = 0;
    std::vector<GHGuildAsset> Assets;
    std::vector<GHGuildSpawn> Spawns;
};

// =====================================================
// Guild House Location
// =====================================================

struct GHLocation
{
    uint32_t Id = 0;
    std::string Name;

    uint32_t MapId = 0;
    float X = 0.0f;
    float Y = 0.0f;
    float Z = 0.0f;
    float O = 0.0f;

    float MinX = 0.0f;
    float MaxX = 0.0f;
    float MinY = 0.0f;
    float MaxY = 0.0f;

    uint64_t Price = 0;
    bool Enabled = false;
};

// =====================================================
// Utility Helpers
// =====================================================

namespace GuildHouseUtil
{
    template<typename T> constexpr bool HasFlag(T value, T flag)
    {
        using U = std::underlying_type_t<T>;

        return (static_cast<U>(value) & static_cast<U>(flag)) != 0;
    }
}

#endif
