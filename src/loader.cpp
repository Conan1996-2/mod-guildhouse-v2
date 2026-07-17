#include "ScriptMgr.h"


// Core Guild House systems
void AddSC_GuildHouseMgr();
void AddSC_GuildHousePhaseMgr();
void AddSC_GuildHouseInstanceMgr();
void AddSC_GuildHouseSpawner();


// NPC / Commands
void AddSC_GuildHouseBroker();
void AddSC_GuildHouseSalesman();
void AddSC_GuildHouseCommands();



void AddGuildHouseScripts()
{
    //
    // Managers first
    //
    AddSC_GuildHousePhaseMgr();
    AddSC_GuildHouseInstanceMgr();
    AddSC_GuildHouseMgr();
    AddSC_GuildHouseSpawner();


    //
    // Player interaction
    //
    AddSC_GuildHouseCommands();


    //
    // NPC scripts
    //
    AddSC_GuildHouseBroker();
    AddSC_GuildHouseSalesman();
}
