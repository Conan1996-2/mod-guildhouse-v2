#ifndef MOD_GUILDHOUSE_CONFIG_H
#define MOD_GUILDHOUSE_CONFIG_H

#include "Common.h"

class GuildHouseConfig
{
public:
    static void Load();

    static bool IsEnabled();
    static uint64 GetCost();
    static uint32 GetPhaseOffset();
    static uint32 GetNpcEntry();
};

#define sGuildHouseConfig GuildHouseConfig

#endif
