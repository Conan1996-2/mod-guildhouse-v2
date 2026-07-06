#ifndef MOD_GUILDHOUSE_TYPES_H
#define MOD_GUILDHOUSE_TYPES_H

#include <cstdint>
#include <vector>
#include <string>

#include "GuildHouseDefines.h"

// =====================================================
// Catalog structures (static DB data)
// =====================================================

struct GHCategory
{
    uint32_t Id;
    uint32_t ParentId;
    std::string Name;
    uint16_t SortOrder;
    bool Enabled;
};

struct GHCatalogAsset
{
    uint32_t ComponentId;
    uint32_t CatalogId;

    uint32_t Entry;
    uint32_t DisplayId;

    float Scale;

    GHSpawnFlags SpawnFlags;
    GHBehaviorFlags BehaviorFlags;

    float X, Y, Z, O;
    
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
// Catalog package (a buildable item)
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
// Player placed asset (guild-owned)
// =====================================================

struct GHGuildAsset
{
    uint32_t AssetId;
    uint32_t GuildId;
    uint32_t CatalogId;
    uint16_t LayoutId;
    
    GHAssetStatus Status;
    uint32_t Phase;

    float X, Y, Z, O;
};

// =====================================================
// Runtime instance (spawned world object)
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
// Guild House runtime state
// =====================================================

struct GHGuildHouse
{
    uint32_t GuildId;
    uint32_t OwnerGuid;

    uint32_t Phase;

    std::vector<GHGuildAsset> Assets;
};

#endif
