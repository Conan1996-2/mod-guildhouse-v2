#ifndef MOD_GUILDHOUSE_SPAWNER_H
#define MOD_GUILDHOUSE_SPAWNER_H

#include "Common.h"

struct GHSpawnNpc
{
    uint32 guildId;
    uint32 entry;
    uint32 map;
    float x, y, z, o;
};

struct GHSpawnObject
{
    uint32 guildId;
    uint32 entry;
    uint32 map;
    float x, y, z, o;
};

class GuildHouseSpawner
{
public:
    static GuildHouseSpawner* instance();

    void LoadAll();
    void SpawnGuild(uint32 guildId);
    void DespawnGuild(uint32 guildId);

private:
    void SpawnNPC(const GHSpawnNpc& data, uint32 phase);
    void SpawnObject(const GHSpawnObject& data, uint32 phase);

    std::unordered_map<uint32, std::vector<GHSpawnNpc>> _npcs;
    std::unordered_map<uint32, std::vector<GHSpawnObject>> _objects;
};

#define sGuildHouseSpawner GuildHouseSpawner::instance()

#endif
