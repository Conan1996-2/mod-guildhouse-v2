#ifndef MOD_GUILDHOUSE_SPAWNER_H
#define MOD_GUILDHOUSE_SPAWNER_H

#include <cstdint>

#include "GuildHouseTypes.h"

class Creature;
class GameObject;
class Player;

class GuildHouseSpawner
{
public:
    static GuildHouseSpawner& Instance();

    // =====================================================
    // Permanent spawn creation
    // =====================================================

    bool SpawnCatalogAsset(
        Player* player,
        uint32_t guildId,
        uint32_t assetId,
        const GHCatalogAsset& component,
        float baseX,
        float baseY,
        float baseZ,
        float baseO);

    // =====================================================
    // Permanent removal
    // =====================================================

    bool RemoveAsset(uint32_t assetId);

private:
    GuildHouseSpawner() = default;
};

#define sGuildHouseSpawner GuildHouseSpawner::Instance()

#endif
