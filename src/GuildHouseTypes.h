#ifndef MOD_GUILDHOUSE_TYPES_H
#define MOD_GUILDHOUSE_TYPES_H

#include <cstdint>
#include <string>
#include <vector>

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
// One permanent object belonging to a catalog item.
// =====================================================

struct GHCatalogAsset
{
    uint32_t ComponentId;
    uint32_t CatalogId;

    uint32_t Entry;
    uint32_t DisplayId;

    float Scale;

    uint32_t SpawnFlags;
    uint32_t BehaviorFlags;
    uint32_t ScriptType;

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

    uint32_t SpawnFlags;
    uint32_t BehaviorFlags;

    bool Enabled;

    std::vector<GHCatalogAsset> Components;
};

// =====================================================
// Purchased Guild Asset
//
// Stored in guildhouse_asset.
//
// Represents purchased objects.
// =====================================================

struct GHGuildAsset
{
    uint32_t AssetId;
    uint32_t GuildId;
    uint32_t CatalogId;
    uint16_t LayoutId;
    uint8_t Status;

    float X;
    float Y;
    float Z;
    float O;
};

// =====================================================
// Permanent Instance Spawn Registry
//
// Stored in guildhouse_instance.
//
// Tracks objects spawned inside the guild instance.
// =====================================================

struct GHInstance
{
    uint32_t SpawnId;
    uint32_t InstanceId;
    uint32_t GuildId;
    uint32_t AssetId;
    uint32_t CatalogId;
    uint32_t Guid;

    uint8_t Type;
    // 0 creature
    // 1 gameobject
    // 2 portal
    // 3 trigger

    uint32_t MapId;

    float X;
    float Y;
    float Z;
    float O;
};

// =====================================================
// Guild House
//
// One permanent instance per guild.
// Instance remains until Guild House is sold.
// =====================================================

struct GHGuildHouse
{
    uint32_t GuildId;
    uint32_t OwnerGuid;
    uint32_t LocationId;
    uint32_t InstanceId;
    std::vector<GHGuildAsset> Assets;
};

// =====================================================
// Guild House Location
//
// Used by Broker purchase menu.
// =====================================================

struct GHLocation
{
    uint32_t Id;
    std::string Name;
    uint32_t MapId;

    float X;
    float Y;
    float Z;
    float O;

    float MinX;
    float MaxX;

    float MinY;
    float MaxY;

    uint64_t Price;

    bool Enabled;
};

#endif
