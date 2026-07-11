#include "GuildHouseConfig.h"

#include "Config.h"
#include "Log.h"

GuildHouseConfig& GuildHouseConfig::Instance()
{
    static GuildHouseConfig instance;
    return instance;
}

void GuildHouseConfig::Load()
{
    _enabled = sConfigMgr->GetOption<bool>("GuildHouse.Enable", false);
    _free = sConfigMgr->GetOption<bool>("GuildHouse.Free", false);
    _loadAll = sConfigMgr->GetOption<bool>("GuildHouse.LoadAll", false);

    _houseCost = sConfigMgr->GetOption<uint64_t>("GuildHouse.Cost.House", 1000000ULL);
    _npcCost = sConfigMgr->GetOption<uint64_t>("GuildHouse.Cost.NPC", 1000000ULL);
    _portalCost = sConfigMgr->GetOption<uint64_t>("GuildHouse.Cost.Portal", 10000000ULL);
    _objectCost = sConfigMgr->GetOption<uint64_t>("GuildHouse.Cost.Object", 500000ULL);

    LOG_INFO("server.loading", "GuildHouse: Configuration loaded.");
}

bool GuildHouseConfig::IsEnabled() const
{
    return _enabled;
}

bool GuildHouseConfig::IsFree() const
{
    return _free;
}

bool GuildHouseConfig::LoadAll() const
{
    return _loadAll;
}

uint64_t GuildHouseConfig::GetHouseCost() const
{
    return _houseCost;
}

uint64_t GuildHouseConfig::GetNpcCost() const
{
    return _npcCost;
}

uint64_t GuildHouseConfig::GetPortalCost() const
{
    return _portalCost;
}

uint64_t GuildHouseConfig::GetObjectCost() const
{
    return _objectCost;
}
