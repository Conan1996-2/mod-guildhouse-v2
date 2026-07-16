#ifndef MOD_GUILDHOUSE_SPAWNER_H
#define MOD_GUILDHOUSE_SPAWNER_H

#include <cstdint>


class GuildHouseSpawner
{
public:

    static GuildHouseSpawner& Instance();


    /*
        Startup loading
    */

    void LoadPlacedAssets();


    /*
        Spawn tracking
    */

    bool HasExistingSpawn(uint32_t instanceId, uint32_t assetId);


    /*
        Asset spawning
    */

    bool SpawnAsset(uint32_t guildId, uint32_t assetId);


    /*
        Asset removal
    */

    bool RemoveAsset(uint32_t guildId, uint32_t instanceId, uint32_t assetId);

    bool RemoveAllAssets(uint32_t guildId, uint32_t instanceId);


private:

    GuildHouseSpawner() = default;


    bool SpawnCreature(
        uint32_t guildId,
        uint32_t instanceId,
        uint32_t assetId,
        uint32_t mapId,
        uint32_t entry,
        float x,
        float y,
        float z,
        float o);


    bool SpawnGameObject(
        uint32_t guildId,
        uint32_t instanceId,
        uint32_t assetId,
        uint32_t mapId,
        uint32_t entry,
        float x,
        float y,
        float z,
        float o);
};


#define sGuildHouseSpawner GuildHouseSpawner::Instance()


#endif
