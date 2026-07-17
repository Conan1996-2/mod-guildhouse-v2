#include "ScriptMgr.h"


void AddSC_GuildHouseWorld();

void AddSC_GuildHouseMgr();
void AddSC_GuildHousePhaseMgr();
void AddSC_GuildHouseInstanceMgr();
void AddSC_GuildHouseSpawner();

void AddSC_GuildHouseBroker();
void AddSC_GuildHouseSalesman();
void AddSC_GuildHouseCommands();



void AddGuildHouseScripts()
{
    //
    // Startup
    //
    AddSC_GuildHouseWorld();


    //
    // Systems
    //
    AddSC_GuildHousePhaseMgr();
    AddSC_GuildHouseInstanceMgr();
    AddSC_GuildHouseMgr();
    AddSC_GuildHouseSpawner();


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
