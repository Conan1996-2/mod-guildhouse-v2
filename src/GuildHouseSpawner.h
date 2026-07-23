#ifndef MOD_GUILDHOUSE_SPAWNER_H
#define MOD_GUILDHOUSE_SPAWNER_H

#include <cstdint>

class GuildHouseSpawner
{
public:

    static GuildHouseSpawner& Instance();

    // =====================================================
    // Startup
    // =====================================================
    void LoadPlacedAssets();

    // =====================================================
    // Spawn lookup
    // =====================================================
    bool HasExistingSpawn(uint32_t guildId, uint32_t assetId);

    // =====================================================
    // Asset spawning
    // =====================================================
    bool SpawnAsset(uint32_t guildId, uint32_t assetId, uint32_t catalogId, float x, float y, float z, float o, bool checkExists = true);
    bool SpawnCreature(uint32_t guildId, uint32_t assetId, uint32_t phaseMask, uint32_t mapId, uint32_t entry, float x, float y, float z, float o);

    // =====================================================
    // Removal
    // =====================================================
    bool RemoveAsset(uint32_t guildId, uint32_t assetId);
    bool RemoveAllAssets(uint32_t guildId);

private:

    GuildHouseSpawner() = default;

    bool SpawnGameObject(uint32_t guildId, uint32_t assetId, uint32_t phaseMask, uint32_t mapId, uint32_t entry, float x, float y, float z, float o);
    bool GuildHouseSpawner::RemoveCreatureSpawn(uint32 guid);
    bool GuildHouseSpawner::RemoveGameObjectSpawn(uint32 guid);
};

#define sGuildHouseSpawner GuildHouseSpawner::Instance()

#endif
