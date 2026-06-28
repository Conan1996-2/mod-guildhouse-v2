#include "GuildHouseConfig.h"

static bool enabled;
static uint64 cost;
static uint32 phaseOffset;
static uint32 npcEntry;

void GuildHouseConfig::Load()
{
    enabled = sConfigMgr->GetOption<bool>("GuildHouse.Enable", true);
    cost = sConfigMgr->GetOption<uint64>("GuildHouse.Cost", 50000000);
    phaseOffset = sConfigMgr->GetOption<uint32>("GuildHouse.PhaseOffset", 10000);
    npcEntry = sConfigMgr->GetOption<uint32>("GuildHouse.NpcEntry", 900000);
}

bool GuildHouseConfig::IsEnabled() { return enabled; }
uint64 GuildHouseConfig::GetCost() { return cost; }
uint32 GuildHouseConfig::GetPhaseOffset() { return phaseOffset; }
uint32 GuildHouseConfig::GetNpcEntry() { return npcEntry; }
