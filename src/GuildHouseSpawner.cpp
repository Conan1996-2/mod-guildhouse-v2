#include "GuildHouseSpawner.h"

#include "GuildHouseMgr.h"
#include "GuildHouseCatalogMgr.h"
#include "GuildHouseTypes.h"
#include "GuildHouseDefines.h"

#include "DatabaseEnv.h"
#include "QueryResult.h"
#include "Log.h"

GuildHouseSpawner& GuildHouseSpawner::Instance()
{
    static GuildHouseSpawner instance;
    return instance;
}

void GuildHouseSpawner::LoadAllGuildHouses()
{
    // Called on world startup AFTER managers are loaded
    // We only prepare structure here
}

void GuildHouseSpawner::SpawnGuild(uint32_t guildId)
{
    const GHGuildHouse* house = sGuildHouseMgr.GetGuildHouse(guildId);
    if (!house)
        return;

    for (const auto& asset : house->Assets)
    {
        SpawnAsset(guildId, asset.AssetId);
    }
}

void GuildHouseSpawner::DespawnGuild(uint32_t guildId)
{
    // We will implement this once we confirm spawn tracking method in your core
}

void GuildHouseSpawner::SpawnAsset(uint32_t guildId, uint32_t assetId)
{
    // STEP 1 (SAFE): resolve catalog entry only
    const GHGuildHouse* house = sGuildHouseMgr.GetGuildHouse(guildId);
    if (!house)
        return;

    for (const auto& asset : house->Assets)
    {
        if (asset.AssetId != assetId)
            continue;

        const GHCatalog* catalog = sGuildHouseCatalogMgr.GetCatalog(asset.CatalogId);
        if (!catalog)
            return;

        // -------------------------------------------------
        // IMPORTANT:
        // We DO NOT spawn yet because we must confirm:
        // - Creature spawn method in your NPCBots core
        // - GameObject spawn method signature
        // - Phase assignment method
        // -------------------------------------------------

        LOG_INFO("module",
            "GuildHouse: Ready to spawn asset {} (catalog {}) for guild {}",
            asset.AssetId,
            asset.CatalogId,
            guildId);
    }
}
