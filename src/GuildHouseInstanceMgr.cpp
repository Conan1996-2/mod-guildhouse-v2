#include "GuildHouseInstanceMgr.h"

#include "DatabaseEnv.h"
#include "QueryResult.h"
#include "Log.h"

GuildHouseInstanceMgr& GuildHouseInstanceMgr::Instance()
{
    static GuildHouseInstanceMgr instance;
    return instance;
}

void GuildHouseInstanceMgr::Load()
{
    _instances.clear();

    QueryResult result = CharacterDatabase.Query(
        "SELECT guildId, assetId, catalogId, guid, type, mapId, phase, x, y, z, o "
        "FROM guildhouse_instance");

    if (!result)
        return;

    do
    {
        Field* f = result->Fetch();

        GHInstanceRecord rec;
        rec.guildId   = f[0].Get<uint32_t>();
        rec.assetId   = f[1].Get<uint32_t>();
        rec.catalogId = f[2].Get<uint32_t>();
        rec.guid      = f[3].Get<uint32_t>();
        rec.type      = f[4].Get<uint8_t>();
        rec.mapId     = f[5].Get<uint32_t>();
        rec.phase     = f[6].Get<uint32_t>();
        rec.x         = f[7].Get<float>();
        rec.y         = f[8].Get<float>();
        rec.z         = f[9].Get<float>();
        rec.o         = f[10].Get<float>();

        _instances[rec.guildId].push_back(rec);

    } while (result->NextRow());

    LOG_INFO("module", "GuildHouseInstanceMgr loaded instances");
}

void GuildHouseInstanceMgr::Save(const GHInstanceRecord& r)
{
    CharacterDatabase.Execute(
        "REPLACE INTO guildhouse_instance "
        "(guildId, assetId, catalogId, guid, type, mapId, phase, x, y, z, o) "
        "VALUES (%u,%u,%u,%u,%u,%u,%u,%f,%f,%f,%f)",
        r.guildId, r.assetId, r.catalogId, r.guid, r.type,
        r.mapId, r.phase, r.x, r.y, r.z, r.o);
}

void GuildHouseInstanceMgr::AddInstance(const GHInstanceRecord& record)
{
    _instances[record.guildId].push_back(record);
    Save(record);
}

void GuildHouseInstanceMgr::RemoveGuild(uint32_t guildId)
{
    _instances.erase(guildId);

    CharacterDatabase.Execute(
        "DELETE FROM guildhouse_instance WHERE guildId = %u",
        guildId);
}

const std::vector<GHInstanceRecord>&
GuildHouseInstanceMgr::GetGuildInstances(uint32_t guildId) const
{
    static std::vector<GHInstanceRecord> empty;
    auto it = _instances.find(guildId);
    return (it != _instances.end()) ? it->second : empty;
}
