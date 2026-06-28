#include "GuildHouseConfig.h"
#include "GuildHouseMgr.h"
#include "GuildHouseNpc.h"

class GuildHouseWorld : public WorldScript
{
public:
    GuildHouseWorld() : WorldScript("GuildHouseWorld") {}

    void OnStartup() override
    {
        sGuildHouseConfig.Load();
        sGuildHouseMgr->Load();
    }
};

void Addmod_guildhouseScripts()
{
    new GuildHouseWorld();
    new npc_guildhouse();
}
