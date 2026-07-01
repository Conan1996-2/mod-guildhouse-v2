#include "GuildHouseMgr.h"

#include "DatabaseEnv.h"
#include "GuildHouseConfig.h"

GuildHouseMgr& GuildHouseMgr::Instance()
{
    static GuildHouseMgr instance;
    return instance;
}

void GuildHouseMgr::Load()
{
    _assets.clear();
    _instances.clear();
    _loadedGuilds.clear();

    QueryResult result = CharacterDatabase.Query(
        "SELECT assetId, guildId, catalogId, status, phase, x, y, z, o FROM guildhouse_asset");

    if (!result)
        return;

    do
    {
        Field* fields = result->Fetch();

        GHAsset asset;
        asset.assetId = fields[0].Get<uint32_t>();
        asset.guildId = fields[1].Get<uint32_t>();
        asset.catalogId = fields[2].Get<uint32_t>();
        asset.status = fields[3].Get<uint8_t>();
        asset.phase = fields[4].Get<uint32_t>();
        asset.x = fields[5].Get<float>();
        asset.y = fields[6].Get<float>();
        asset.z = fields[7].Get<float>();
        asset.o = fields[8].Get<float>();

        _assets[asset.guildId].push_back(asset);

    } while (result->NextRow());
}

bool GuildHouseMgr::HasGuildHouse(uint32_t guildId) const
{
    return _assets.find(guildId) != _assets.end();
}

uint32_t GuildHouseMgr::GetPhase(uint32_t guildId) const
{
    return guildId + GH_PHASE_OFFSET;
}

const std::vector<GHInstance>& GuildHouseMgr::GetInstances(uint32_t guildId) const
{
    static std::vector<GHInstance> empty;
    auto it = _instances.find(guildId);
    return (it != _instances.end()) ? it->second : empty;
}

void GuildHouseMgr::AddInstance(uint32_t guildId, const GHInstance& instance)
{
    _instances[guildId].push_back(instance);
}
