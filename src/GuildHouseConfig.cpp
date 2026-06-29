#include <cstdint>

#include "GuildHouseConfig.h"

#include "Config.h"

bool GuildHouseConfig::_enabled = false;
bool GuildHouseConfig::_free = false;
bool GuildHouseConfig::_loadAll = false;

uint64_t GuildHouseConfig::_houseCost = 1000000;
uint64_t GuildHouseConfig::_npcCost = 1000000;
uint64_t GuildHouseConfig::_portalCost = 10000000;
uint64_t GuildHouseConfig::_objectCost = 500000;

void GuildHouseConfig::Load()
{
    _enabled = sConfigMgr->GetOption<bool>("GuildHouse.Enable", false);

    _free = sConfigMgr->GetOption<bool>("GuildHouse.Free", false);

    _loadAll = sConfigMgr->GetOption<bool>("GuildHouse.LoadAll", false);

    _houseCost = sConfigMgr->GetOption<uint64>("GuildHouse.Cost.House", 1000000);

    _npcCost = sConfigMgr->GetOption<uint64>("GuildHouse.Cost.NPC", 1000000);

    _portalCost = sConfigMgr->GetOption<uint64>("GuildHouse.Cost.Portal", 10000000);

    _objectCost = sConfigMgr->GetOption<uint64>("GuildHouse.Cost.Object", 500000);
}

bool GuildHouseConfig::Enabled()
{
    return _enabled;
}

bool GuildHouseConfig::Free()
{
    return _free;
}

bool GuildHouseConfig::LoadAll()
{
    return _loadAll;
}

uint64_t GuildHouseConfig::HouseCost()
{
    return _houseCost;
}

uint64_t GuildHouseConfig::NpcCost()
{
    return _npcCost;
}

uint64_t GuildHouseConfig::PortalCost()
{
    return _portalCost;
}

uint64_t GuildHouseConfig::ObjectCost()
{
    return _objectCost;
}
