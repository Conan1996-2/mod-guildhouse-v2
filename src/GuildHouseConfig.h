#ifndef MOD_GUILDHOUSE_CONFIG_H
#define MOD_GUILDHOUSE_CONFIG_H

#include <cstdint>

class GuildHouseConfig
{
public:
    static void Load();

    static bool Enabled();
    static bool Free();
    static bool LoadAll();

    static uint64_t HouseCost();
    static uint64_t NpcCost();
    static uint64_t PortalCost();
    static uint64_t ObjectCost();

private:
    static bool _enabled;
    static bool _free;
    static bool _loadAll;

    static uint64_t _houseCost;
    static uint64_t _npcCost;
    static uint64_t _portalCost;
    static uint64_t _objectCost;
};

#endif
