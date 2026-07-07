#ifndef MOD_GUILDHOUSE_INSTANCE_MGR_H
#define MOD_GUILDHOUSE_INSTANCE_MGR_H

#include <unordered_map>
#include <vector>

struct GHInstanceRecord
{
    uint32_t guildId;

    uint32_t assetId;
    uint32_t catalogId;

    uint32_t guid;

    uint8_t type;          // creature/gameobject/salesman

    uint32_t mapId;
    uint32_t phase;

    float x;
    float y;
    float z;
    float o;
};

class GuildHouseInstanceMgr
{
public:

    static GuildHouseInstanceMgr& Instance();

    void Load();

    void Save(const GHInstanceRecord& record);

    void AddInstance(const GHInstanceRecord& record);

    void RemoveGuild(uint32_t guildId);

    void RemoveGuid(uint32_t guid);

    const std::vector<GHInstanceRecord>&
    GetGuildInstances(uint32_t guildId) const;

    GHInstanceRecord* GetByGuid(uint32_t guid);

private:

    GuildHouseInstanceMgr() = default;

    std::unordered_map<uint32_t,
        std::vector<GHInstanceRecord>> _instances;
};

#define sGuildHouseInstanceMgr GuildHouseInstanceMgr::Instance()

#endif
