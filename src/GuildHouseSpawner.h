#ifndef MOD_GUILDHOUSE_SPAWNER_H
#define MOD_GUILDHOUSE_SPAWNER_H

#include <cstdint>

class GuildHouseSpawner
{
public:

    static GuildHouseSpawner& Instance();


    // Called on server startup
    void LoadAllGuildHouses();


    // Spawn all permanent assets for one guild
    void SpawnGuild(uint32_t guildId);


    // Spawn one purchased catalog asset
    bool SpawnAsset(
        uint32_t guildId,
        uint32_t assetId);



private:

    GuildHouseSpawner() = default;


    bool SpawnCreature(
        uint32_t guildId,
        uint32_t entry,
        float x,
        float y,
        float z,
        float o);


    bool SpawnGameObject(
        uint32_t guildId,
        uint32_t entry,
        float x,
        float y,
        float z,
        float o);

};


#define sGuildHouseSpawner GuildHouseSpawner::Instance()


#endif
