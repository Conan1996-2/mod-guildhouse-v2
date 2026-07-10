#ifndef MOD_GUILDHOUSE_INSTANCE_MGR_H
#define MOD_GUILDHOUSE_INSTANCE_MGR_H

#include <cstdint>
#include <unordered_map>

struct GHInstanceRecord
{
    uint32_t InstanceId = 0;
    uint32_t GuildId = 0;

    uint32_t MapId = 0;

    float X = 0;
    float Y = 0;
    float Z = 0;
    float O = 0;
};

class GuildHouseInstanceMgr
{
public:

    static GuildHouseInstanceMgr& Instance();

    /*
        Instance lifecycle
    */

    void Load();
    uint32_t CreateInstance(uint32_t guildId);
    bool RemoveInstance(uint32_t guildId);

    /*
        Lookup
    */

    uint32_t GetInstanceId(uint32_t guildId) const;
    bool HasInstance(uint32_t guildId) const;
    bool IsGuildInstance(uint32_t guildId, uint32_t instanceId) const;

    /*
        Runtime tracking
    */

    const GHInstanceRecord* GetInstance(uint32_t instanceId) const;

private:

    GuildHouseInstanceMgr() = default;
    uint32_t GenerateInstanceId();
    std::unordered_map<uint32_t, GHInstanceRecord> _instances;
    std::unordered_map<uint32_t, uint32_t> _guildInstances;
};

#define sGuildHouseInstanceMgr GuildHouseInstanceMgr::Instance()

#endif
