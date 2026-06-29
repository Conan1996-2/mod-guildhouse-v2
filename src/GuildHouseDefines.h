#ifndef MOD_GUILDHOUSE_DEFINES_H
#define MOD_GUILDHOUSE_DEFINES_H

#include <cstdint>

constexpr uint32_t GH_MAP = 1;

constexpr float GH_X = 16222.57f;
constexpr float GH_Y = 16265.91f;
constexpr float GH_Z = 13.21f;
constexpr float GH_O = 0.0f;

constexpr uint32_t GH_PHASE_OFFSET = 10000;

enum class GuildHouseAssetType : uint8_t
{
    Creature = 0,
    GameObject = 1,
    Portal = 2
};

enum class GuildHouseAssetStatus : uint8_t
{
    Purchased = 0,
    Placed = 1,
    Stored = 2
};

#endif
