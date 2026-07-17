#ifndef MOD_GUILDHOUSE_SPAWNER_H
#define MOD_GUILDHOUSE_SPAWNER_H

#include <cstdint>


class GuildHouseSpawner
{
public:

    static GuildHouseSpawner& Instance();


    // =====================================================
    // Startup loading
    //
    // Loads all placed guild assets and recreates missing
    // phase based spawns.
    // =====================================================

    void LoadPlacedAssets();



    // =====================================================
    // Spawn tracking
    // =====================================================

    bool HasExistingSpawn(
        uint32_t guildId,
        uint32_t assetId);



    // =====================================================
    // Asset spawning
    // =====================================================

    bool SpawnAsset(
        uint32_t guildId,
        uint32_t assetId);



    // =====================================================
    // Asset removal
    // =====================================================

    bool RemoveAsset(
        uint32_t guildId,
        uint32_t assetId);


    bool RemoveAllAssets(
        uint32_t guildId);



    // =====================================================
    // Asset movement
    // =====================================================

    bool MoveAsset(
        uint32_t guildId,
        uint32_t assetId,
        float deltaX,
        float deltaY,
        float deltaZ,
        float orientation);



private:

    GuildHouseSpawner() = default;



    // =====================================================
    // Creature spawning
    // =====================================================

    bool SpawnCreature(
        uint32_t guildId,
        uint32_t assetId,
        uint32_t phaseMask,
        uint32_t mapId,
        uint32_t entry,
        float x,
        float y,
        float z,
        float o);



    // =====================================================
    // GameObject spawning
    // =====================================================

    bool SpawnGameObject(
        uint32_t guildId,
        uint32_t assetId,
        uint32_t phaseMask,
        uint32_t mapId,
        uint32_t entry,
        float x,
        float y,
        float z,
        float o);



    // =====================================================
    // Spawn removal helpers
    // =====================================================

    bool RemoveCreatureSpawn(
        uint32_t spawnGuid);


    bool RemoveGameObjectSpawn(
        uint32_t spawnGuid);

};


#define sGuildHouseSpawner GuildHouseSpawner::Instance()


#endif
