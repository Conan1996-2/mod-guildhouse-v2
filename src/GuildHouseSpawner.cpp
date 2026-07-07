#include "GuildHouseSpawner.h"

#include "GuildHouseInstanceMgr.h"

#include "Player.h"

GuildHouseSpawner& GuildHouseSpawner::Instance()
{
    static GuildHouseSpawner instance;
    return instance;
}

bool GuildHouseSpawner::SpawnCatalogAsset(
    Player* /*player*/,
    uint32_t /*guildId*/,
    uint32_t /*assetId*/,
    const GHCatalogAsset& /*component*/,
    float /*baseX*/,
    float /*baseY*/,
    float /*baseZ*/,
    float /*baseO*/)
{
    //
    // Permanent spawn creation
    //
    // This will:
    //
    // 1) Create permanent creature/gameobject
    // 2) Assign guild phase
    // 3) Save to AzerothCore spawn tables
    // 4) Record GUID in guildhouse_instance
    //
    // Implement once the permanent spawn pipeline is finalized.
    //

    return true;
}

bool GuildHouseSpawner::RemoveAsset(uint32_t /*assetId*/)
{
    //
    // Later:
    //
    // Lookup guildhouse_instance
    //
    // Remove permanent spawn from AC DB
    //
    // Remove guildhouse_instance record
    //
    return true;
}
