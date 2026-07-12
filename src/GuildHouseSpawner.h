#ifndef MOD_GUILDHOUSE_SPAWNER_H
#define MOD_GUILDHOUSE_SPAWNER_H

#include <cstdint>

class GuildHouseSpawner
{
public:

    static GuildHouseSpawner& Instance();

    void LoadPlacedAssets();

    bool HasExistingSpawn(uint32_t instanceId, uint32_t assetId);

    bool SpawnAsset(uint32_t guildId, uint32_t assetId);

    bool RemoveAsset(uint32_t guildId, uint32_t instanceId, uint32_t assetId);
    bool RemoveAllAssets(uint32_t guildId, uint32_t instanceId);

private:

    GuildHouseSpawner() = default;

    bool SpawnCreature(uint32_t guildId, uint32_t instanceId, uint32_t assetId, uint32_t entry, float x, float y, float z, float o);

    bool SpawnGameObject(uint32_t guildId, uint32_t instanceId, uint32_t assetId, uint32_t entry, float x, float y, float z, float o);
};

#define sGuildHouseSpawner GuildHouseSpawner::Instance()

#endif
