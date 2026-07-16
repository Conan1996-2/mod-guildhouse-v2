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

    void SetGuildInstance(uint32_t guildId, uint32_t instanceId);

    void RemoveGuildInstance(uint32_t guildId);

    uint32_t GetOrCreateGuildInstance(uint32_t guildId);

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
    bool EnsureGuildInstanceSave(uint32_t guildId);

    bool BindPlayerToGuildInstance(Player* player);

    bool TeleportToGuildHouse(Player* player);

    // =====================================================
    // Locations
    // =====================================================
    const GHLocation* GetLocation(uint32_t locationId) const;

    std::vector<const GHLocation*> GetLocations() const;

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
    std::vector<const GHGuildAsset*> GetPurchasedAssets(uint32 guildId) const;

    const GHGuildAsset* GetAsset(uint32_t guildId, uint32_t assetId) const;

    GHGuildAsset* GetAsset(uint32_t guildId, uint32_t assetId);

    bool PlaceAsset(Player* player, uint32 assetId);

    bool MoveAsset(Player* player, uint32 assetId);

    bool StoreAsset(Player* player, uint32 assetId);

    bool SellAsset(Player* player, uint32 assetId);

private:

    GuildHouseMgr() = default;

    std::unordered_map<uint32_t, GHGuildHouse> _houses;

    // guildId -> instanceId
    std::unordered_map<uint32_t, uint32_t> _guildInstances;

    // instanceId -> guildId
    std::unordered_map<uint32_t, uint32_t> _instanceGuilds;

    // locationId -> location
    std::unordered_map<uint32_t, GHLocation> _locations;

    // instanceId -> spawned objects
    std::unordered_multimap<uint32_t, GHInstance> _instanceObjects;
};

#define sGuildHouseMgr GuildHouseMgr::Instance()

#endif
