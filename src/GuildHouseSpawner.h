#ifndef MOD_GUILDHOUSE_SPAWNER_H
#define MOD_GUILDHOUSE_SPAWNER_H

#include <cstdint>

class GuildHouseSpawner
{
public:

    static GuildHouseSpawner& Instance();


    // Optional startup loading
    // Disabled by default
    void LoadAllGuildHouses();


    // Spawn every purchased asset belonging to guild
    void SpawnGuild(uint32_t guildId);


    // Spawn one purchased catalog item
    bool SpawnAsset(
        uint32_t guildId,
        uint32_t assetId);


private:

    GuildHouseSpawner() = default;


    bool CreatePermanentCreature(
        uint32_t guildId,
        uint32_t entry,
        float x,
        float y,
        float z,
        float o);


    bool CreatePermanentGameObject(
        uint32_t guildId,
        uint32_t entry,
        float x,
        float y,
        float z,
        float o);

};


#define sGuildHouseSpawner GuildHouseSpawner::Instance()


#endif
