#include "GuildHousePhaseMgr.h"

#include "DatabaseEnv.h"
#include "QueryResult.h"
#include "Log.h"


GuildHousePhaseMgr& GuildHousePhaseMgr::Instance()
{
    static GuildHousePhaseMgr instance;
    return instance;
}


// =====================================================
// Load Existing Guild House Phases
//
// One phase belongs to one guild.
// =====================================================

void GuildHousePhaseMgr::Load()
{
    _phases.clear();


    if (QueryResult result = CharacterDatabase.Query(
        "SELECT guildId, phaseMask, mapId "
        "FROM guildhouse_phase"))
    {
        do
        {
            Field* fields = result->Fetch();

            GHPhaseRecord record;

            record.GuildId =
                fields[0].Get<uint32>();

            record.PhaseMask =
                fields[1].Get<uint32>();

            record.MapId =
                fields[2].Get<uint32>();


            _phases.emplace(
                record.GuildId,
                record);


        } while (result->NextRow());
    }


    LOG_INFO(
        "server.loading",
        "GuildHousePhaseMgr loaded {} guild phases",
        _phases.size());
}



// =====================================================
// Create Guild House Phase
//
// Generates one phase mask for a guild.
// =====================================================

uint32_t GuildHousePhaseMgr::CreatePhase(
    uint32_t guildId,
    uint32_t mapId)
{
    if (HasPhase(guildId))
        return GetPhaseMask(guildId);


    uint32_t phaseMask =
        GeneratePhaseMask();


    if (!phaseMask)
    {
        LOG_ERROR(
            "module",
            "No available Guild House phases");

        return 0;
    }


    GHPhaseRecord record;

    record.GuildId =
        guildId;

    record.PhaseMask =
        phaseMask;

    record.MapId =
        mapId;


    _phases.emplace(
        guildId,
        record);


    CharacterDatabase.Execute(
        "INSERT INTO guildhouse_phase "
        "(guildId, phaseMask, mapId) "
        "VALUES ({},{},{})",
        guildId,
        phaseMask,
        mapId);



    LOG_INFO(
        "module",
        "Created Guild House phase {} for guild {}",
        phaseMask,
        guildId);


    return phaseMask;
}



// =====================================================
// Enter Guild House Phase
//
// Applies phase and teleports player.
// =====================================================

bool GuildHousePhaseMgr::EnterPhase(
    Player* player,
    uint32_t guildId,
    uint32_t mapId,
    float x,
    float y,
    float z,
    float o)
{
    if (!player)
        return false;


    uint32_t phaseMask =
        GetPhaseMask(guildId);


    if (!phaseMask)
    {
        LOG_ERROR(
            "module",
            "Guild {} has no phase",
            guildId);

        return false;
    }



    player->SetPhaseMask(
        phaseMask,
        true);



    player->TeleportTo(
        mapId,
        x,
        y,
        z,
        o);



    return true;
}



// =====================================================
// Remove Guild Phase
//
// Removes phase ownership.
// =====================================================

bool GuildHousePhaseMgr::RemovePhase(
    uint32_t guildId)
{
    auto itr =
        _phases.find(guildId);


    if (itr == _phases.end())
        return false;


    uint32_t phaseMask =
        itr->second.PhaseMask;



    _phases.erase(
        itr);



    CharacterDatabase.Execute(
        "DELETE FROM guildhouse_phase "
        "WHERE guildId={}",
        guildId);



    LOG_INFO(
        "module",
        "Removed Guild House phase {} for guild {}",
        phaseMask,
        guildId);



    return true;
}



// =====================================================
// Lookup
// =====================================================

bool GuildHousePhaseMgr::HasPhase(
    uint32_t guildId) const
{
    return _phases.find(guildId)
        != _phases.end();
}



uint32_t GuildHousePhaseMgr::GetPhaseMask(
    uint32_t guildId) const
{
    auto itr =
        _phases.find(guildId);


    if (itr == _phases.end())
        return 0;


    return itr->second.PhaseMask;
}



const GHPhaseRecord*
GuildHousePhaseMgr::GetPhase(
    uint32_t guildId) const
{
    auto itr =
        _phases.find(guildId);


    if (itr == _phases.end())
        return nullptr;


    return &itr->second;
}



// =====================================================
// Phase Generator
//
// Bitmask phases only.
//
// 2,4,8,16,32...
// =====================================================

uint32_t GuildHousePhaseMgr::GeneratePhaseMask()
{
    static uint32_t nextPhase = 2;


    while (nextPhase <= (1 << 30))
    {
        uint32_t candidate =
            nextPhase;


        nextPhase <<= 1;


        bool used = false;


        for (auto const& [guildId, phase] : _phases)
        {
            if (phase.PhaseMask == candidate)
            {
                used = true;
                break;
            }
        }


        if (!used)
            return candidate;
    }


    return 0;
}
