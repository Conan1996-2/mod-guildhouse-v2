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
    _instanceGuilds.clear();


    if (QueryResult result = CharacterDatabase.Query(
        "SELECT "
        "i.guildId, "
        "i.instanceId, "
        "h.locationId, "
        "l.mapId "
        "FROM guildhouse_instance i "
        "JOIN guildhouse h ON h.guildId = i.guildId "
        "JOIN guildhouse_locations l ON l.id = h.locationId"))
    {
        do
        {
            Field* fields = result->Fetch();


            GHInstanceRecord record;
            record.GuildId = fields[0].Get<uint32>();
            record.InstanceId = fields[1].Get<uint32>();
            record.MapId = fields[3].Get<uint32>();
            
            _instances.emplace(
                record.InstanceId,
                record);


            _guildInstances.emplace(
                record.GuildId,
                record.InstanceId);


            _instanceGuilds.emplace(
                record.InstanceId,
                record.GuildId);


        } while(result->NextRow());
    }


    LOG_INFO(
        "server.loading",
        "GuildHouseInstanceMgr loaded {} instances",
        _instances.size());
}



// =====================================================
// Create Guild Instance
//
// One instance per guild.
// =====================================================

uint32_t GuildHouseInstanceMgr::CreateInstance(
    uint32_t guildId,
    uint32_t mapId)
{
    if (HasInstance(guildId))
        return GetInstanceId(guildId);


    uint32_t instanceId = GenerateInstanceId();


    GHInstanceRecord record;

    record.InstanceId = instanceId;

    record.GuildId = guildId;

    record.MapId = mapId;



    _instances.emplace(
        instanceId,
        record);


    _guildInstances.emplace(
        guildId,
        instanceId);


    _instanceGuilds.emplace(
        instanceId,
        guildId);



    CharacterDatabase.Execute(
        "INSERT INTO guildhouse_instance "
        "(guildId, instanceId, mapId) "
        "VALUES ({},{},{})",
        guildId,
        instanceId,
        mapId);



    LOG_INFO(
        "module",
        "Created Guild House instance {} for guild {} on map {}",
        instanceId,
        guildId,
        mapId);


    return instanceId;
}



// =====================================================
// Remove Guild Instance
//
// Does not remove purchased assets.
// =====================================================

bool GuildHouseInstanceMgr::RemoveInstance(
    uint32_t guildId)
{
    auto itr = _guildInstances.find(guildId);

    if (itr == _guildInstances.end())
        return false;


    return RemoveInstanceById(
        itr->second);
}



bool GuildHouseInstanceMgr::RemoveInstanceById(
    uint32_t instanceId)
{
    auto itr = _instances.find(instanceId);

    if (itr == _instances.end())
        return false;


    uint32_t guildId = itr->second.GuildId;


    _instances.erase(itr);


    _guildInstances.erase(
        guildId);


    _instanceGuilds.erase(
        instanceId);



    CharacterDatabase.Execute(
        "DELETE FROM guildhouse_instance WHERE instanceId={}",
        instanceId);



    LOG_INFO(
        "module",
        "Removed Guild House instance {} for guild {}",
        instanceId,
        guildId);


    return true;
}



// =====================================================
// Lookup
// =====================================================

uint32_t GuildHouseInstanceMgr::GetInstanceId(
    uint32_t guildId) const
{
    auto itr = _guildInstances.find(guildId);

    if (itr == _guildInstances.end())
        return 0;


    return itr->second;
}



uint32_t GuildHouseInstanceMgr::GetGuildId(
    uint32_t instanceId) const
{
    auto itr = _instanceGuilds.find(instanceId);

    if (itr == _instanceGuilds.end())
        return 0;


    return itr->second;
}



bool GuildHouseInstanceMgr::HasInstance(
    uint32_t guildId) const
{
    return _guildInstances.find(guildId)
        != _guildInstances.end();
}



bool GuildHouseInstanceMgr::IsGuildInstance(
    uint32_t guildId,
    uint32_t instanceId) const
{
    auto itr = _guildInstances.find(guildId);

    if (itr == _guildInstances.end())
        return false;


    return itr->second == instanceId;
}



const GHInstanceRecord* GuildHouseInstanceMgr::GetInstance(
    uint32_t instanceId) const
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

    while (_instances.find(nextId) != _instances.end())
        ++nextId;


    return nextId++;
}
