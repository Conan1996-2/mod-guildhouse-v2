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
};

struct GHComponent
{
    uint32_t ComponentId;
    uint32_t CatalogId;

    uint32_t Entry;
    uint32_t DisplayId;

    float Scale;

    GHSpawnFlags SpawnFlags;
    GHBehaviorFlags BehaviorFlags;

    GHScriptType ScriptType;

    float X, Y, Z, O;
};

// =====================================================
// Catalog package (a buildable item)
// =====================================================

struct GHCatalog
{
    uint32_t CatalogId;
    uint32_t CategoryId;

    std::string Name;

    std::vector<GHComponent> Components;
};

// =====================================================
// Player placed asset (guild-owned)
// =====================================================

struct GHPlacedAsset
{
    uint32_t AssetId;
    uint32_t GuildId;
    uint32_t CatalogId;

    uint32_t Phase;

    float X, Y, Z, O;
};

// =====================================================
// Runtime instance (spawned world object)
// =====================================================

struct GHInstance
{
    uint32_t InstanceId;

    uint32_t AssetId;
    uint32_t ComponentId;

    uint32_t Guid;

    uint32_t Phase;

    float X, Y, Z, O;

    GHAssetStatus State;

    GHSpawnFlags SpawnFlags;
};

// =====================================================
// Guild House runtime state
// =====================================================

struct GHGuildHouse
{
    uint32_t GuildId;
    uint32_t OwnerGuid;

    uint32_t Phase;

    std::vector<GHPlacedAsset> Assets;
};

#endif
