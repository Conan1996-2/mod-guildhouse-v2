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
    // Guild Instance Management
    // =====================================================

    bool IsGuildInstance(uint32_t guildId, uint32_t instanceId) const;

    uint32_t GetGuildInstance(uint32_t guildId) const;

    uint32_t GetGuildByInstance(uint32_t instanceId) const;

    uint32_t GetOrCreateGuildInstance(Player* player, uint32_t guildId);

    bool EnsureGuildInstanceSave(uint32_t guildId);

    bool BindPlayerToGuildInstance(Player* player);


    // =====================================================
    // Guild House Ownership
    // =====================================================

    bool HasGuildHouse(uint32_t guildId) const;

    const GHGuildHouse* GetGuildHouse(uint32_t guildId) const;

    const GHLocation* GetGuildLocation(uint32_t guildId) const;

    bool CreateGuildHouse(uint32_t guildId, uint32_t ownerGuid, uint32_t locationId);

    bool SellGuildHouse(uint32_t guildId);


    // =====================================================
    // Instance Teleport
    // =====================================================

    bool TeleportToGuildHouse(Player* player);


    // =====================================================
    // Locations
    // =====================================================

    const GHLocation* GetLocation(uint32_t locationId) const;

    std::vector<const GHLocation*> GetLocations() const;


    // =====================================================
    // Boundary Validation
    // =====================================================

    bool IsInsideGuildHouseBoundary(uint32_t guildId, float x, float y) const;


    // =====================================================
    // Catalog Purchasing
    // =====================================================

    bool PurchaseCatalogItem(Player* player, uint32_t catalogId);


    // =====================================================
    // Salesman
    // =====================================================

    bool HasSalesman(uint32_t guildId) const;

    bool CreatePermanentSalesman(Player* player, uint32 entry);

    void RecordSalesmanSpawn(uint32_t guildId, uint32_t spawnId, uint32_t mapId, uint32_t instanceId, float x, float y, float z, float o);


    // =====================================================
    // Assets
    // =====================================================

    std::vector<const GHGuildAsset*> GetPurchasedAssets(uint32_t guildId) const;

    const GHGuildAsset* GetAsset(uint32_t guildId, uint32_t assetId) const;

    GHGuildAsset* GetAsset(uint32_t guildId, uint32_t assetId);

    bool PlaceAsset(Player* player, uint32_t assetId);

    bool MoveAsset(Player* player, uint32_t assetId);

    bool StoreAsset(Player* player, uint32_t assetId);

    bool SellAsset(Player* player, uint32_t assetId);


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
