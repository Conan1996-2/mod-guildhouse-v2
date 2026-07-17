#include "GuildHousePhaseMgr.h"

#include "DatabaseEnv.h"
#include "Player.h"
#include "Log.h"

#include <algorithm>



GuildHousePhaseMgr&
GuildHousePhaseMgr::Instance()
{
    static GuildHousePhaseMgr instance;

    return instance;
}



// =====================================================
// Load
// =====================================================

void GuildHousePhaseMgr::Load()
{
    _phases.clear();



    QueryResult result =
        CharacterDatabase.Query(
            "SELECT "
            "guildId,"
            "phaseMask,"
            "mapId,"
            "positionX,"
            "positionY,"
            "positionZ,"
            "orientation,"
            "minX,"
            "maxX,"
            "minY,"
            "maxY "
            "FROM guildhouse_phase");



    if (!result)
    {
        LOG_INFO(
            "server.loading",
            "No Guild House phases found");

        return;
    }



    do
    {
        Field* fields =
            result->Fetch();



        GHPhaseRecord phase;



        phase.GuildId =
            fields[0].Get<uint32>();


        phase.PhaseMask =
            fields[1].Get<uint32>();


        phase.MapId =
            fields[2].Get<uint32>();



        phase.X =
            fields[3].Get<float>();


        phase.Y =
            fields[4].Get<float>();


        phase.Z =
            fields[5].Get<float>();


        phase.O =
            fields[6].Get<float>();



        phase.MinX =
            fields[7].Get<float>();


        phase.MaxX =
            fields[8].Get<float>();


        phase.MinY =
            fields[9].Get<float>();


        phase.MaxY =
            fields[10].Get<float>();



        _phases.emplace(
            phase.GuildId,
            phase);



    } while(result->NextRow());



    LOG_INFO(
        "server.loading",
        "Loaded {} Guild House phases",
        _phases.size());
}



// =====================================================
// Create Phase
// =====================================================

uint32_t GuildHousePhaseMgr::CreatePhase(
    uint32_t guildId,
    uint32_t locationId)
{
    if (HasPhase(guildId))
    {
        return GetPhaseMask(guildId);
    }



    QueryResult result =
        WorldDatabase.Query(
            "SELECT "
            "mapId,"
            "positionX,"
            "positionY,"
            "positionZ,"
            "orientation,"
            "minX,"
            "maxX,"
            "minY,"
            "maxY "
            "FROM guildhouse_locations "
            "WHERE id={}",
            locationId);



    if (!result)
        return 0;



    Field* fields =
        result->Fetch();



    uint32_t phaseMask =
        GeneratePhaseMask();



    if (!phaseMask)
        return 0;



    GHPhaseRecord phase;



    phase.GuildId =
        guildId;


    phase.PhaseMask =
        phaseMask;


    phase.MapId =
        fields[0].Get<uint32>();



    phase.X =
        fields[1].Get<float>();


    phase.Y =
        fields[2].Get<float>();


    phase.Z =
        fields[3].Get<float>();


    phase.O =
        fields[4].Get<float>();



    phase.MinX =
        fields[5].Get<float>();


    phase.MaxX =
        fields[6].Get<float>();


    phase.MinY =
        fields[7].Get<float>();


    phase.MaxY =
        fields[8].Get<float>();



    _phases.emplace(
        guildId,
        phase);



    CharacterDatabase.Execute(
        "INSERT INTO guildhouse_phase "
        "(guildId,phaseMask,mapId,"
        "positionX,positionY,positionZ,orientation,"
        "minX,maxX,minY,maxY)"
        "VALUES "
        "({},{},{},{},{},{},{},{},{},{},{})",

        phase.GuildId,
        phase.PhaseMask,
        phase.MapId,
        phase.X,
        phase.Y,
        phase.Z,
        phase.O,
        phase.MinX,
        phase.MaxX,
        phase.MinY,
        phase.MaxY);



    LOG_INFO(
        "module",
        "Created Guild House phase {} for guild {}",
        phaseMask,
        guildId);



    return phaseMask;
}



// =====================================================
// Enter
// =====================================================

bool GuildHousePhaseMgr::EnterPhase(
    Player* player,
    uint32_t guildId)
{
    if (!player)
        return false;



    const GHPhaseRecord* phase =
        GetPhase(guildId);



    if (!phase)
        return false;



    player->SetPhaseMask(
        phase->PhaseMask,
        true);



    AddMember(
        guildId,
        player->GetGUID().GetCounter());



    player->TeleportTo(
        phase->MapId,
        phase->X,
        phase->Y,
        phase->Z,
        phase->O);



    return true;
}



// =====================================================
// Leave
// =====================================================

bool GuildHousePhaseMgr::LeavePhase(
    Player* player)
{
    if (!player)
        return false;



    RemoveMember(
        player->GetGuildId(),
        player->GetGUID().GetCounter());



    player->SetPhaseMask(
        1,
        true);



    return true;
}



// =====================================================
// Member tracking
// =====================================================

bool GuildHousePhaseMgr::AddMember(
    uint32_t guildId,
    uint64_t guid)
{
    auto itr =
        _phases.find(guildId);



    if (itr == _phases.end())
        return false;



    itr->second.Members.insert(
        guid);



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



    itr->second.Members.erase(
        guid);



    return true;
}



// =====================================================
// Boundary
// =====================================================

bool GuildHousePhaseMgr::CheckBoundary(
    Player* player)
{
    if (!IsMember(player))
        return true;



    const GHPhaseRecord* phase =
        GetPhase(
            player->GetGuildId());



    if (!phase)
        return true;



    float x =
        player->GetPositionX();


    float y =
        player->GetPositionY();



    if (x < phase->MinX ||
        x > phase->MaxX ||
        y < phase->MinY ||
        y > phase->MaxY)
    {
        player->TeleportTo(
            phase->MapId,
            phase->X,
            phase->Y,
            phase->Z,
            phase->O);


        return false;
    }



    return true;
}



// =====================================================
// Remove
// =====================================================

bool GuildHousePhaseMgr::RemovePhase(
    uint32_t guildId)
{
    auto itr =
        _phases.find(guildId);



    if (itr == _phases.end())
        return false;



    _phases.erase(
        itr);



    CharacterDatabase.Execute(
        "DELETE FROM guildhouse_phase "
        "WHERE guildId={}",
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



bool GuildHousePhaseMgr::IsMember(
    Player* player) const
{
    if (!player)
        return false;



    auto itr =
        _phases.find(
            player->GetGuildId());



    if (itr == _phases.end())
        return false;



    return itr->second.Members.find(
        player->GetGUID().GetCounter())
        != itr->second.Members.end();
}



// =====================================================
// Phase generator
// =====================================================

uint32_t GuildHousePhaseMgr::GeneratePhaseMask()
{
    static uint32_t nextMask = 1;



    while(nextMask &&
          nextMask <= (1 << 30))
    {
        uint32_t candidate =
            nextMask;



        nextMask <<= 1;



        bool used = false;



        for(auto const& [guildId, phase] : _phases)
        {
            if(phase.PhaseMask == candidate)
            {
                used = true;
                break;
            }
        }



        if(!used)
            return candidate;
    }



    return 0;
}
