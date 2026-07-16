#ifndef MOD_GUILDHOUSE_TYPES_H
#define MOD_GUILDHOUSE_TYPES_H

#include <cstdint>
#include <string>
#include <vector>


// =====================================================
// Guild House Enums
// =====================================================


enum GHAssetStatus : uint8_t
{
    GH_ASSET_PURCHASED = 0,
    GH_ASSET_PLACED    = 1,
    GH_ASSET_STORED    = 2,
    GH_ASSET_DISABLED  = 3
};


// =====================================================
// Spawn Flags
// =====================================================

enum GHSpawnFlags : uint32_t
{
    GH_SPAWN_NONE        = 0x00,
    GH_SPAWN_CREATURE    = 0x01,
    GH_SPAWN_GAMEOBJECT  = 0x02,
    GH_SPAWN_PORTAL      = 0x04,
    GH_SPAWN_TRIGGER     = 0x08
};


// =====================================================
// Behavior Flags
// =====================================================

enum GHBehaviorFlags : uint32_t
{
    GH_BEHAVIOR_NONE       = 0x00,
    GH_BEHAVIOR_INTERACT   = 0x01,
    GH_BEHAVIOR_VENDOR    = 0x02,
    GH_BEHAVIOR_TELEPORT  = 0x04
};


// =====================================================
// Script Types
// =====================================================

enum GHScriptType : uint32_t
{
    GH_SCRIPT_NONE      = 0,
    GH_SCRIPT_VENDOR    = 1,
    GH_SCRIPT_TELEPORT  = 2,
    GH_SCRIPT_CUSTOM    = 3
};


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
// Individual spawned component belonging to a catalog item.
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
//
// What the salesman sells.
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
// Stored in guildhouse_asset.
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
// Guild Instance Record
//
// One instance belongs to one guild.
// Stored in guildhouse_instance.
// =====================================================

struct GHInstanceRecord
{
    uint32_t InstanceId = 0;
    uint32_t GuildId = 0;

    uint32_t MapId = 0;

    float X = 0.0f;
    float Y = 0.0f;
    float Z = 0.0f;
    float O = 0.0f;
};


// =====================================================
// Permanent Spawn Registry
//
// Stored in guildhouse_spawn.
// =====================================================

struct GHInstance
{
    uint32_t SpawnId = 0;

    uint32_t InstanceId = 0;
    uint32_t GuildId = 0;

    uint32_t AssetId = 0;
    uint32_t CatalogId = 0;

    uint32_t Guid = 0;


    GHSpawnFlags Type = GH_SPAWN_NONE;


    uint32_t MapId = 0;


    float X = 0.0f;
    float Y = 0.0f;
    float Z = 0.0f;
    float O = 0.0f;
};


// =====================================================
// Guild House Ownership
//
// One house per guild.
// =====================================================

struct GHGuildHouse
{
    uint32_t GuildId = 0;
    uint32_t OwnerGuid = 0;

    uint32_t LocationId = 0;

    uint32_t InstanceId = 0;


    std::vector<GHGuildAsset> Assets;
};


// =====================================================
// Guild House Location
//
// Available purchase locations.
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
    template<typename T>
    constexpr bool HasFlag(T value, T flag)
    {
        using U = std::underlying_type_t<T>;

        return (static_cast<U>(value) & static_cast<U>(flag)) != 0;
    }
}


#endif
