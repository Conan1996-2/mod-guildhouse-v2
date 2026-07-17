#ifndef MOD_GUILDHOUSE_PHASE_MGR_H
#define MOD_GUILDHOUSE_PHASE_MGR_H

#include <cstdint>
#include <unordered_map>

#include "GuildHouseTypes.h"


class Player;


class GuildHousePhaseMgr
{
public:

    static GuildHousePhaseMgr& Instance();


    /*
        Phase lifecycle
    */

    void Load();


    uint32_t CreatePhase(
        uint32_t guildId,
        uint32_t mapId);


    bool RemovePhase(
        uint32_t guildId);


    bool EnterPhase(
        Player* player,
        uint32_t guildId,
        uint32_t mapId,
        float x,
        float y,
        float z,
        float o);



    /*
        Lookup
    */

    uint32_t GetPhaseMask(
        uint32_t guildId) const;


    bool HasPhase(
        uint32_t guildId) const;


    const GHPhaseRecord* GetPhase(
        uint32_t guildId) const;



private:

    GuildHousePhaseMgr() = default;


    /*
        guildId -> phase record
    */

    std::unordered_map<uint32_t, GHPhaseRecord> _phases;



    /*
        Generates a free bit phase

        2
        4
        8
        16
        ...
    */

    uint32_t GeneratePhaseMask();
};



#define sGuildHousePhaseMgr GuildHousePhaseMgr::Instance()


#endif
