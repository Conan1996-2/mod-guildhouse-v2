#include "ScriptMgr.h"

#include "GuildHouseConfig.h"

class GuildHouseWorldScript : public WorldScript
{
public:
    GuildHouseWorldScript() : WorldScript("GuildHouseWorldScript")
    {
    }

    void OnBeforeConfigLoad(bool /*reload*/) override
    {
        sGuildHouseConfig.Load();
    }
};

void Addmod_guildhouseScripts()
{
    new GuildHouseWorldScript();
}
