#include "ScriptMgr.h"

void AddSC_GuildHouseBroker();
void AddSC_GuildHouseSalesman();
void AddSC_GuildHouseCommands();
void AddSC_GuildHouseMgr();

void AddGuildHouseScripts()
{
    AddSC_GuildHouseBroker();
    AddSC_GuildHouseSalesman();
    AddSC_GuildHouseCommands();
    AddSC_GuildHouseMgr();
}
