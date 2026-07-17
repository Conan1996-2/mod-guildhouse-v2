#ifndef MOD_GUILDHOUSE_MGR_H
#define MOD_GUILDHOUSE_MGR_H

#include <unordered_map>
#include <vector>

#include "GuildHouseTypes.h"


class Player;


class GuildHouseMgr
{
public:

    static GuildHouseMgr& Instance();


    void Load();



    // =====================================================
    // Guild Phase Management
    // =====================================================

    uint32_t GetGuildPhase(
        uint32_t guildId) const;


    uint32_t GetOrCreateGuildPhase(
        Player* player,
        uint32_t guildId);


    bool EnterGuildHousePhase(
        Player* player);


    bool LeaveGuildHousePhase(
        Player* player);



    // =====================================================
    // Guild House Ownership
    // =====================================================

    bool HasGuildHouse(
        uint32_t guildId) const;


    const GHGuildHouse* GetGuildHouse(
        uint32_t guildId) const;


    const GHLocation* GetGuildLocation(
        uint32_t guildId) const;


    bool CreateGuildHouse(
        Player* player,
        uint32_t guildId,
        uint32_t ownerGuid,
        uint32_t locationId);


    bool SellGuildHouse(
        uint32_t guildId);



    // =====================================================
    // Guild House Teleport
    // =====================================================

    bool TeleportToGuildHouse(
        Player* player);



    // =====================================================
    // Locations
    // =====================================================

    const GHLocation* GetLocation(
        uint32_t locationId) const;


    std::vector<const GHLocation*> GetLocations() const;



    // =====================================================
    // Boundary Validation
    // =====================================================

    bool IsInsideGuildHouseBoundary(
        uint32_t guildId,
        float x,
        float y) const;



    // =====================================================
    // Catalog Purchasing
    // =====================================================

    bool PurchaseCatalogItem(
        Player* player,
        uint32_t catalogId);



    // =====================================================
    // Salesman
    // =====================================================

    bool HasSalesman(
        uint32_t guildId) const;


    bool CreatePermanentSalesman(
        Player* player,
        uint32_t entry);


    void RecordSalesmanSpawn(
        uint32_t guildId,
        uint32_t spawnId,
        uint32_t mapId,
        uint32_t phaseMask,
        float x,
        float y,
        float z,
        float o);



    // =====================================================
    // Assets
    // =====================================================

    std::vector<const GHGuildAsset*> GetPurchasedAssets(
        uint32_t guildId) const;


    const GHGuildAsset* GetAsset(
        uint32_t guildId,
        uint32_t assetId) const;


    GHGuildAsset* GetAsset(
        uint32_t guildId,
        uint32_t assetId);



    bool PlaceAsset(
        Player* player,
        uint32_t assetId);


    bool MoveAsset(
        Player* player,
        uint32_t assetId);


    bool StoreAsset(
        Player* player,
        uint32_t assetId);


    bool SellAsset(
        Player* player,
        uint32_t assetId);



private:

    GuildHouseMgr() = default;



    // =====================================================
    // Guild Houses
    // =====================================================

    std::unordered_map<uint32_t, GHGuildHouse> _houses;



    // =====================================================
    // Locations
    // =====================================================

    std::unordered_map<uint32_t, GHLocation> _locations;

};



#define sGuildHouseMgr GuildHouseMgr::Instance()


#endif
