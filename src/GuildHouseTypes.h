#ifndef MOD_GUILDHOUSE_TYPES_H
#define MOD_GUILDHOUSE_TYPES_H

#include <cstdint>
#include <string>
#include <vector>

#include "GuildHouseDefines.h"

// =====================================================
// Catalog Category
// =====================================================

struct GHCategory
{
    uint32_t Id;
    uint32_t ParentId;
    std::string Name;
    uint16_t SortOrder;
    bool Enabled;
};

// =====================================================
// Catalog Component
//
// One permanent object that belongs to a catalog item.
// =====================================================

struct GHCatalogAsset
{
    uint32_t ComponentId;
    uint32_t CatalogId;
    uint32_t Entry;
    uint32_t DisplayId;
    float Scale;

    GHSpawnFlags SpawnFlags;
    GHBehaviorFlags BehaviorFlags;
    GHScriptType ScriptType;
    std::string ScriptData;

    float XOffset;
    float YOffset;
    float ZOffset;
    float OOffset;

    uint32_t TargetMap;
    float TargetX;
    float TargetY;
    float TargetZ;
    float TargetO;
    uint32_t ChildCatalogId;
    uint16_t SortOrder;
};

// =====================================================
// Catalog Item
//
// What the salesman sells.
// =====================================================

struct GHCatalog
{
    uint32_t CatalogId;
    uint32_t CategoryId;
    std::string Name;
    GHSpawnFlags SpawnFlags;
    GHBehaviorFlags BehaviorFlags;
    bool Enabled;
    std::vector<GHCatalogAsset> Components;
};

// =====================================================
// Purchased Guild Asset
//
// Stored in guildhouse_asset
// =====================================================

struct GHGuildAsset
{
    uint32_t AssetId;
    uint32_t GuildId;
    uint32_t CatalogId;
    uint16_t LayoutId;
    GHAssetStatus Status;
    uint32_t Phase;
    float X;
    float Y;
    float Z;
    float O;
};

// =====================================================
// Permanent Spawn Registry
//
// Stored in guildhouse_instance
//
// This is NOT a temporary runtime instance.
// It tracks permanent world objects.
// =====================================================

struct GHInstance
{
    uint32_t InstanceId;
    uint32_t GuildId;
    uint32_t AssetId;
    uint32_t CatalogId;
    uint32_t Guid;
    uint8_t Type;
    uint32_t MapId;
    uint32_t Phase;
    float X;
    float Y;
    float Z;
    float O;
};

// =====================================================
// Guild House
//
// One per guild.
// =====================================================

struct GHGuildHouse
{
    uint32_t GuildId;
    uint32_t OwnerGuid;
    uint32_t Phase;
    std::vector<GHGuildAsset> Assets;
};

// =====================================================
// Guild House locations
//
// Used to populate the Broker purchasing
// =====================================================

struct GHLocation
{
    uint32 Id;
    std::string Name;
    uint32 MapId;
    float X;
    float Y;
    float Z;
    float O;
    float MinX;
    float MaxX;
    float MinY;
    float MaxY;
    uint64 Price;
    bool Enabled;
};

#endif
