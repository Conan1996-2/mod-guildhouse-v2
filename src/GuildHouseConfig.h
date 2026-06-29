#ifndef MOD_GUILDHOUSE_CONFIG_H
#define MOD_GUILDHOUSE_CONFIG_H

#include <cstdint>

class GuildHouseConfig
{
public:
    static GuildHouseConfig& Instance();

    void Load();

    bool Enabled() const;
    bool Free() const;
    bool LoadAll() const;

    uint64_t HouseCost() const;
    uint64_t NpcCost() const;
    uint64_t PortalCost() const;
    uint64_t ObjectCost() const;

private:
    GuildHouseConfig() = default;

    bool _enabled = false;
    bool _free = false;
    bool _loadAll = false;

    uint64_t _houseCost = 1000000ULL;
    uint64_t _npcCost = 1000000ULL;
    uint64_t _portalCost = 10000000ULL;
    uint64_t _objectCost = 500000ULL;
};

#define sGuildHouseConfig GuildHouseConfig::Instance()

#endif
