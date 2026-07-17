#include "GuildHouseInstanceMgr.h"

#include "DatabaseEnv.h"
#include "QueryResult.h"
#include "Log.h"
#include "Player.h"
#include "MapMgr.h"
#include "Map.h"

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
        "SELECT i.guildId, i.instanceId, h.locationId "
        "FROM guildhouse_instance i "
        "JOIN guildhouse h ON h.guildId = i.guildId"))
    {
        do
        {
            Field* fields = result->Fetch();

            GHInstanceRecord record;
            record.GuildId = fields[0].Get<uint32>();
            record.InstanceId = fields[1].Get<uint32>();
            record.LocationId = fields[2].Get<uint32>();

            if (QueryResult location = WorldDatabase.Query(
                "SELECT mapId FROM guildhouse_locations WHERE id={}",
                record.LocationId))
            {
                record.MapId = location->Fetch()[0].Get<uint32>();
            }
            else
            {
                record.MapId = 0;
            }
            
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
    Player* player,
    uint32_t guildId,
    uint32_t mapId)
{
    if (!player)
        return 0;

    if (HasInstance(guildId))
        return GetInstanceId(guildId);


    Map* map = sMapMgr->CreateMap(
        mapId,
        player);


    if (!map)
    {
        LOG_ERROR(
            "module",
            "Failed creating Guild House map {} for guild {}",
            mapId,
            guildId);

        return 0;
    }


    uint32_t instanceId =
        map->GetInstanceId();


    GHInstanceRecord record;

    record.GuildId = guildId;
    record.InstanceId = instanceId;
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
        "(guildId, instanceId) "
        "VALUES ({},{})",
        guildId,
        instanceId);


    LOG_INFO(
        "module",
        "Created Guild House instance {} for guild {}",
        instanceId,
        guildId);


    return instanceId;
}

// =====================================================
// Enter Guild Instance
//
// Allows the member to enter instance.
// =====================================================

bool GuildHouseInstanceMgr::EnterInstance(
    Player* player,
    uint32_t guildId,
    uint32_t mapId)
{
    if (!player)
        return false;


    uint32_t instanceId = GetInstanceId(guildId);


    //
    // First entry:
    // Let the core create the instance.
    //
    if (!instanceId)
    {
        Map* map = sMapMgr->CreateMap(
            mapId,
            player);

        if (!map)
        {
            LOG_ERROR(
                "module",
                "Failed creating Guild House instance for guild {} on map {}",
                guildId,
                mapId);

            return false;
        }


        instanceId = map->GetInstanceId();


        GHInstanceRecord record;

        record.GuildId = guildId;
        record.InstanceId = instanceId;
        record.MapId = mapId;


        _instances.emplace(
            instanceId,
            record);


        _guildInstances[guildId] = instanceId;
        _instanceGuilds[instanceId] = guildId;


        CharacterDatabase.Execute(
            "INSERT INTO guildhouse_instance "
            "(guildId, instanceId) VALUES ({},{})",
            guildId,
            instanceId);
    }


    //
    // Bind player to existing guild instance
    //
    InstanceSave* save =
        sInstanceSaveMgr->GetInstanceSave(instanceId);


    if (!save)
    {
        LOG_ERROR(
            "module",
            "Guild House instance {} has no InstanceSave",
            instanceId);

        return false;
    }


    sInstanceSaveMgr->PlayerBindToInstance(
        player->GetGUID(),
        save,
        false,
        player);


    return true;
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

