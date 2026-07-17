#include "ScriptMgr.h"


void AddSC_GuildHouseWorld();

void AddSC_GuildHouseBroker();
void AddSC_GuildHouseSalesman();
void AddSC_GuildHouseCommands();
void AddSC_GuildHouseMgr();

void AddGuildHouseScripts()
{
    //
    // Startup
    //
    AddSC_GuildHouseWorld();

    //
    // Systems
    //
    AddSC_GuildHouseMgr();

    //
    // Gameplay
    //
    AddSC_GuildHouseCommands();

    //
    // NPCs
    //
    AddSC_GuildHouseBroker();
    AddSC_GuildHouseSalesman();
}
