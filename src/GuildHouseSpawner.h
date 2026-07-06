#ifndef MOD_GUILDHOUSE_SPAWNER_H
#define MOD_GUILDHOUSE_SPAWNER_H

#include <cstdint>
#include "GuildHouseTypes.h"

class GuildHouseSpawner
{
public:
    static GuildHouseSpawner& Instance();

    void LoadAllGuildHouses();

    void SpawnGuild(uint32_t guildId);
    void DespawnGuild(uint32_t guildId);

    void SpawnAsset(uint32_t guildId, uint32_t assetId);

private:
    GuildHouseSpawner() = default;
};

#define sGuildHouseSpawner GuildHouseSpawner::Instance()

#endif
