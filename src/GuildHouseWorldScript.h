#ifndef MOD_GUILDHOUSE_WORLD_H
#define MOD_GUILDHOUSE_WORLD_H

#include "ScriptMgr.h"


class GuildHouseWorldScript : public WorldScript
{
public:

    GuildHouseWorldScript()
        : WorldScript("GuildHouseWorldScript")
    {
    }


    void OnAfterConfigLoad(bool reload) override;

    void OnStartup() override;
};


void AddSC_GuildHouseWorld();


#endif
