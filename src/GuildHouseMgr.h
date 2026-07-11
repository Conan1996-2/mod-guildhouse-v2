#ifndef MOD_GUILDHOUSE_MGR_H
#define MOD_GUILDHOUSE_MGR_H

#include <unordered_map>
#include <vector>

#include "Player.h"
#include "GuildHouseTypes.h"

class GuildHouseMgr
{
public:

    static GuildHouseMgr& Instance();

    void Load();

    bool IsGuildInstance(uint32_t guildId, uint32_t instanceId) const;
    uint32_t GetGuildInstance(uint32_t guildId) const;
    void SetGuildInstance(uint32_t guildId, uint32_t instanceId);
    void RemoveGuildInstance(uint32_t guildId);

    bool HasGuildHouse(uint32_t guildId) const;
    const GHGuildHouse* GetGuildHouse(uint32_t guildId) const;
    uint32_t GetOrCreateGuildInstance(uint32_t guildId);

    const GHLocation* GetLocation(uint32_t locationId) const;
    std::vector<const GHLocation*> GetLocations() const;

    bool CreateGuildHouse(uint32_t guildId, uint32_t ownerGuid, uint32_t locationId);

    bool PurchaseCatalogItem(Player* player, uint32_t catalogId);

    bool HasSalesman(uint32_t guildId) const;
    bool CreatePermanentSalesman(Player* player, uint32 entry);
    void RecordSalesmanSpawn(uint32_t guildId, uint32_t spawnId, uint32_t mapId, uint32_t instanceId, float x, float y, float z, float o);

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
    std::unordered_map<uint32_t, uint32_t> _guildInstances;
    std::unordered_map<uint32, GHLocation> _locations;
};

#define sGuildHouseMgr GuildHouseMgr::Instance()

#endif
