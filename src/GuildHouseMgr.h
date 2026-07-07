#ifndef MOD_GUILDHOUSE_MGR_H
#define MOD_GUILDHOUSE_MGR_H

#include <unordered_map>

#include "Player.h"
#include "GuildHouseTypes.h"

class GuildHouseMgr
{
public:

    static GuildHouseMgr& Instance();

    void Load();


    bool HasGuildHouse(uint32_t guildId) const;


    uint32_t GetPhase(uint32_t guildId) const;


    bool CreateGuildHouse(
        uint32_t guildId,
        uint32_t ownerGuid);


    const GHGuildHouse*
    GetGuildHouse(uint32_t guildId) const;


    bool PurchaseCatalogItem(
        Player* player,
        uint32_t catalogId);


    bool HasSalesman(
        uint32_t guildId) const;


    void RecordSalesmanSpawn(
        uint32_t guildId,
        uint32_t spawnId,
        uint32_t mapId,
        uint32_t phase,
        float x,
        float y,
        float z,
        float o);


private:

    GuildHouseMgr() = default;


    std::unordered_map<uint32_t, GHGuildHouse> _houses;
};


#define sGuildHouseMgr GuildHouseMgr::Instance()

#endif
