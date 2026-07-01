#ifndef MOD_GUILDHOUSE_MGR_H
#define MOD_GUILDHOUSE_MGR_H

#include <unordered_map>
#include <vector>
#include <cstdint>

#include "GuildHouseDefines.h"

struct GHAsset
{
    uint32_t assetId;
    uint32_t guildId;
    uint32_t catalogId;

    uint8_t status;

    uint32_t phase;

    float x, y, z, o;
};

struct GHInstance
{
    uint32_t instanceId;
    uint32_t assetId;
    uint32_t componentId;

    uint32_t guid;

    uint32_t phase;

    float x, y, z, o;

    GHInstanceState state;
};

class GuildHouseMgr
{
public:
    static GuildHouseMgr& Instance();

    void Load();

    // Guild lifecycle
    bool HasGuildHouse(uint32_t guildId) const;
    uint32_t GetPhase(uint32_t guildId) const;

    // Asset lifecycle
    void LoadGuildAssets(uint32_t guildId);
    void SpawnGuild(uint32_t guildId);
    void DespawnGuild(uint32_t guildId);

    // Instance tracking
    void AddInstance(uint32_t guildId, const GHInstance& instance);
    const std::vector<GHInstance>& GetInstances(uint32_t guildId) const;

private:
    GuildHouseMgr() = default;

    std::unordered_map<uint32_t, std::vector<GHAsset>> _assets;
    std::unordered_map<uint32_t, std::vector<GHInstance>> _instances;
    std::unordered_map<uint32_t, bool> _loadedGuilds;
};

#define sGuildHouseMgr GuildHouseMgr::Instance()

#endif
