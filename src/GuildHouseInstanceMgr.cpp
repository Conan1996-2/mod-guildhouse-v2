#include "GuildHouseInstanceMgr.h"

#include "DatabaseEnv.h"
#include "QueryResult.h"
#include "Log.h"

GuildHouseInstanceMgr& GuildHouseInstanceMgr::Instance()
{
    static GuildHouseInstanceMgr instance;
    return instance;
}

// =====================================================
// Load existing guild instances
//
// One instance belongs to one guild.
// =====================================================

void GuildHouseInstanceMgr::Load()
{
    _instances.clear();
    _guildInstances.clear();

    if (QueryResult result = CharacterDatabase.Query("SELECT instanceId, guildId, mapId, x, y, z, o FROM guildhouse_instance"))
    {
        do
        {
            Field* fields = result->Fetch();

            GHInstanceRecord record;

            record.InstanceId = fields[0].Get<uint32>();
            record.GuildId = fields[1].Get<uint32>();
            record.MapId = fields[2].Get<uint32>();
            record.X = fields[3].Get<float>();
            record.Y = fields[4].Get<float>();
            record.Z = fields[5].Get<float>();
            record.O = fields[6].Get<float>();

            _instances.emplace(record.InstanceId, record);
            _guildInstances.emplace(record.GuildId, record.InstanceId);
        } while(result->NextRow());
    }

    LOG_INFO("module", "GuildHouseInstanceMgr loaded {} instances", _instances.size());
}

// =====================================================
// Create Guild Instance
//
// One instance per guild.
// =====================================================

uint32_t GuildHouseInstanceMgr::CreateInstance(uint32_t guildId)
{
    if (HasInstance(guildId))
        return GetInstanceId(guildId);

    uint32_t instanceId = GenerateInstanceId();

    GHInstanceRecord record;

    record.InstanceId = instanceId;
    record.GuildId = guildId;
    record.MapId = 1;

    _instances.emplace(instanceId, record);
    _guildInstances.emplace(guildId, instanceId);

    CharacterDatabase.Execute("INSERT INTO guildhouse_instance (instanceId,guildId,mapId,x,y,z,o) VALUES (%u,%u,%u,%f,%f,%f,%f)",
        instanceId, guildId, record.MapId, record.X, record.Y, record.Z, record.O);

    return instanceId;
}

// =====================================================
// Remove Guild Instance
//
// Does not remove purchased assets.
// =====================================================

bool GuildHouseInstanceMgr::RemoveInstance(uint32_t guildId)
{
    auto itr = _guildInstances.find(guildId);
    if (itr == _guildInstances.end())
        return false;

    uint32_t instanceId = itr->second;

    _instances.erase(instanceId);
    _guildInstances.erase(itr);

    CharacterDatabase.Execute("DELETE FROM guildhouse_instance WHERE guildId=%u", guildId);

    return true;
}

// =====================================================
// Lookup
// =====================================================

uint32_t GuildHouseInstanceMgr::GetInstanceId(uint32_t guildId) const
{
    auto itr = _guildInstances.find(guildId);
    if (itr == _guildInstances.end())
        return 0;

    return itr->second;
}

bool GuildHouseInstanceMgr::HasInstance(uint32_t guildId) const
{
    return _guildInstances.find(guildId) != _guildInstances.end();
}

bool GuildHouseInstanceMgr::IsGuildInstance(uint32_t guildId, uint32_t instanceId) const
{
    auto itr = _guildInstances.find(guildId);
    if (itr == _guildInstances.end())
        return false;

    return itr->second == instanceId;
}

const GHInstanceRecord* GuildHouseInstanceMgr::GetInstance(uint32_t instanceId) const
{
    auto itr = _instances.find(instanceId);
    if (itr == _instances.end())
        return nullptr;

    return &itr->second;
}

// =====================================================
// Instance ID generation
//
// Replace later with core instance manager integration.
// =====================================================

uint32_t GuildHouseInstanceMgr::GenerateInstanceId()
{
    static uint32_t nextId = 10000;

    return nextId++;
}
