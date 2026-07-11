#ifndef MOD_GUILDHOUSE_CONFIG_H
#define MOD_GUILDHOUSE_CONFIG_H

#include <cstdint>

class GuildHouseConfig
{
public:
    static GuildHouseConfig& Instance();

    void Load();

    bool IsEnabled() const;
    bool IsFree() const;
    bool LoadAll() const;

    uint64_t GetHouseCost() const;
    uint64_t GetNpcCost() const;
    uint64_t GetPortalCost() const;
    uint64_t GetObjectCost() const;
    uint64_t RefundPercent() const;

private:
    GuildHouseConfig() = default;

    bool _enabled = false;
    bool _free = false;
    bool _loadAll = false;

    uint64_t _houseCost;
    uint64_t _npcCost;
    uint64_t _portalCost;
    uint64_t _objectCost;
    uint64_t _refundPercent;
};

#define sGuildHouseConfig GuildHouseConfig::Instance()

#endif
