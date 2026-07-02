#ifndef MOD_GUILDHOUSE_INSTANCE_MGR_H
#define MOD_GUILDHOUSE_INSTANCE_MGR_H

#include <unordered_map>
#include <vector>
#include "GuildHouseDefines.h"

struct GHInstanceRecord
{
    uint32_t guildId;
    uint32_t assetId;
    uint32_t catalogId;

    uint32_t guid;
    uint8_t  type; // 0 creature, 1 gameobject

    uint32_t mapId;
    uint32_t phase;

    float x, y, z, o;
};

class GuildHouseInstanceMgr
{
public:
    static GuildHouseInstanceMgr& Instance();

    void Load();
    void Save(const GHInstanceRecord& record);

    void AddInstance(const GHInstanceRecord& record);
    void RemoveGuild(uint32_t guildId);

    const std::vector<GHInstanceRecord>& GetGuildInstances(uint32_t guildId) const;

private:
    GuildHouseInstanceMgr() = default;

    std::unordered_map<uint32_t, std::vector<GHInstanceRecord>> _instances;
};

#define sGuildHouseInstanceMgr GuildHouseInstanceMgr::Instance()

#endif
