#include "GuildHousePhaseMgr.h"

#include "DatabaseEnv.h"
#include "QueryResult.h"
#include "Log.h"
#include "Player.h"

#include <algorithm>


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
        "SELECT "
        "p.guildId, "
        "p.phaseMask, "
        "p.mapId, "
        "p.positionX, "
        "p.positionY, "
        "p.positionZ, "
        "p.orientation, "
        "p.minX, "
        "p.maxX, "
        "p.minY, "
        "p.maxY "
        "FROM guildhouse_phase p"))
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

            record.X =
                fields[3].Get<float>();

            record.Y =
                fields[4].Get<float>();

            record.Z =
                fields[5].Get<float>();

            record.O =
                fields[6].Get<float>();

            record.MinX =
                fields[7].Get<float>();

            record.MaxX =
                fields[8].Get<float>();

            record.MinY =
                fields[9].Get<float>();

            record.MaxY =
                fields[10].Get<float>();


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
// One phase per guild.
// =====================================================

uint32_t GuildHousePhaseMgr::CreatePhase(
    uint32_t guildId,
    uint32_t locationId)
{
    if (HasPhase(guildId))
        return GetPhaseMask(guildId);


    QueryResult result =
        WorldDatabase.Query(
            "SELECT "
            "mapId, "
            "positionX, "
            "positionY, "
            "positionZ, "
            "orientation, "
            "minX, "
            "maxX, "
            "minY, "
            "maxY "
            "FROM guildhouse_locations "
            "WHERE id={}",
            locationId);


    if (!result)
    {
        LOG_ERROR(
            "module",
            "Guild House location {} does not exist",
            locationId);

        return 0;
    }


    Field* fields = result->Fetch();


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
        fields[0].Get<uint32>();

    record.X =
        fields[1].Get<float>();

    record.Y =
        fields[2].Get<float>();

    record.Z =
        fields[3].Get<float>();

    record.O =
        fields[4].Get<float>();

    record.MinX =
        fields[5].Get<float>();

    record.MaxX =
        fields[6].Get<float>();

    record.MinY =
        fields[7].Get<float>();

    record.MaxY =
        fields[8].Get<float>();


    _phases.emplace(
        guildId,
        record);



    CharacterDatabase.Execute(
        "INSERT INTO guildhouse_phase "
        "(guildId, phaseMask, mapId, "
        "positionX, positionY, positionZ, orientation, "
        "minX, maxX, minY, maxY) "
        "VALUES ({},{},{},{},{},{},{},{},{},{},{})",
        guildId,
        phaseMask,
        record.MapId,
        record.X,
        record.Y,
        record.Z,
        record.O,
        record.MinX,
        record.MaxX,
        record.MinY,
        record.MaxY);



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
    uint32_t guildId)
{
    if (!player)
        return false;


    auto itr =
        _phases.find(guildId);


    if (itr == _phases.end())
        return false;


    GHPhaseRecord& phase =
        itr->second;


    player->SetPhaseMask(
        phase.PhaseMask,
        true);



    AddMember(
        guildId,
        player->GetGUID().GetCounter());



    player->TeleportTo(
        phase.MapId,
        phase.X,
        phase.Y,
        phase.Z,
        phase.O);


    return true;
}



// =====================================================
// Leave Guild House Phase
// =====================================================

bool GuildHousePhaseMgr::LeavePhase(
    Player* player)
{
    if (!player)
        return false;


    uint32_t guildId =
        player->GetGuildId();


    RemoveMember(
        guildId,
        player->GetGUID().GetCounter());


    player->SetPhaseMask(
        1,
        true);


    auto itr =
        _phases.find(guildId);


    if (itr != _phases.end() &&
        itr->second.Members.empty())
    {
        RemovePhase(guildId);
    }


    return true;
}



// =====================================================
// Member Tracking
// =====================================================

bool GuildHousePhaseMgr::AddMember(
    uint32_t guildId,
    uint64_t guid)
{
    auto itr =
        _phases.find(guildId);

    if (itr == _phases.end())
        return false;


    itr->second.Members.insert(guid);

    return true;
}



bool GuildHousePhaseMgr::RemoveMember(
    uint32_t guildId,
    uint64_t guid)
{
    auto itr =
        _phases.find(guildId);

    if (itr == _phases.end())
        return false;


    itr->second.Members.erase(guid);

    return true;
}



bool GuildHousePhaseMgr::IsMember(
    Player* player) const
{
    if (!player)
        return false;


    uint32_t guildId =
        player->GetGuildId();


    auto itr =
        _phases.find(guildId);


    if (itr == _phases.end())
        return false;


    return itr->second.Members.find(
        player->GetGUID().GetCounter())
        != itr->second.Members.end();
}



// =====================================================
// Boundary Check
// =====================================================

bool GuildHousePhaseMgr::CheckBoundary(
    Player* player)
{
    if (!IsMember(player))
        return true;


    uint32_t guildId =
        player->GetGuildId();


    auto itr =
        _phases.find(guildId);


    if (itr == _phases.end())
        return true;


    GHPhaseRecord const& phase =
        itr->second;


    float x =
        player->GetPositionX();

    float y =
        player->GetPositionY();


    if (x < phase.MinX ||
        x > phase.MaxX ||
        y < phase.MinY ||
        y > phase.MaxY)
    {
        player->TeleportTo(
            phase.MapId,
            phase.X,
            phase.Y,
            phase.Z,
            phase.O);

        return false;
    }


    return true;
}



// =====================================================
// Remove Guild Phase
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


    _phases.erase(itr);


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
// 1,2,4,8,16...
// =====================================================

uint32_t GuildHousePhaseMgr::GeneratePhaseMask()
{
    static uint32_t nextPhase = 1;


    while (nextPhase &&
           nextPhase <= (1 << 30))
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
