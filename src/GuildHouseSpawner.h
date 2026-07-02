#ifndef MOD_GUILDHOUSE_SPAWNER_H
#define MOD_GUILDHOUSE_SPAWNER_H

#include <cstdint>

class GuildHouseSpawner
{
public:
    static GuildHouseSpawner& Instance();

    void LoadAllGuildHouses();
    void SpawnGuild(uint32_t guildId);
    void DespawnGuild(uint32_t guildId);

private:
    GuildHouseSpawner() = default;

    void SpawnAsset(uint32_t guildId, uint32_t assetId);
};

#define sGuildHouseSpawner GuildHouseSpawner::Instance()

#endif
