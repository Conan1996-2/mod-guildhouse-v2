#include "GuildHouseSpawner.h"

#include "GuildHouseMgr.h"
#include "GuildHouseCatalogMgr.h"
#include "GuildHouseTypes.h"
#include "GuildHouseDefines.h"

#include "GameObject.h"
#include "Position.h"
#include "Map.h"
#include "MapMgr.h"
#include "ObjectMgr.h"
#include "Log.h"

GuildHouseSpawner& GuildHouseSpawner::Instance()
{
    static GuildHouseSpawner instance;
    return instance;
}

void GuildHouseSpawner::LoadAllGuildHouses()
{
    // Later: iterate all guilds and spawn them
}

void GuildHouseSpawner::SpawnGuild(uint32_t guildId)
{
    const GHGuildHouse* house = sGuildHouseMgr.GetGuildHouse(guildId);
    if (!house)
        return;

    Map* map = sMapMgr->CreateBaseMap(1); // TEMP: GM island map (adjust if needed)
    if (!map)
        return;

    for (const auto& asset : house->Assets)
    {
        SpawnAsset(guildId, asset.AssetId);
    }
}

void GuildHouseSpawner::DespawnGuild(uint32_t guildId)
{
    // TODO: implement tracking table (guildhouse_instance)
    // We will add this AFTER we confirm spawn works
}

void GuildHouseSpawner::SpawnAsset(uint32_t guildId, uint32_t assetId)
{
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

        Map* map = sMapMgr->FindMap(1, 0); // GM island fallback
        if (!map)
            return;

        for (const auto& comp : catalog->Components)
        {
            float x = asset.X + comp.X;
            float y = asset.Y + comp.Y;
            float z = asset.Z + comp.Z;
            float o = asset.O + comp.O;

            // =====================================================
            // CREATURE SPAWN
            // =====================================================
            if (GuildHouseUtil::HasFlag(comp.SpawnFlags, GHSpawnFlags::Creature))
            {
                const SummonPropertiesEntry* props = nullptr;
                
                Position pos;
                pos.Relocate(x, y, z, o);
                
                if (Creature* creature = map->SummonCreature(
                        comp.Entry,
                        pos,
                        props,
                        0,
                        0))
                {
                    LOG_INFO("module",
                        "GuildHouse: Spawned creature {} for guild {}",
                        comp.Entry, guildId);
                }
            }

            // =====================================================
            // GAMEOBJECT SPAWN
            // =====================================================
            if (GuildHouseUtil::HasFlag(comp.SpawnFlags, GHSpawnFlags::GameObject))
            {
                Position pos;
                pos.Relocate(x, y, z, o);
                
                QuaternionData rot;
                rot.x = 0.0f;
                rot.y = 0.0f;
                rot.z = 0.0f;
                rot.w = 1.0f;
                
                if (GameObject* go = map->SummonGameObject(
                        comp.Entry,
                        pos,
                        rot,
                        0,
                        GO_STATE_READY))
                {
                    LOG_INFO("module",
                        "GuildHouse: Spawned GO {} for guild {}",
                        comp.Entry, guildId);
                }
            }

            // =====================================================
            // PORTAL / SPECIAL LOGIC
            // =====================================================
            if (GuildHouseUtil::HasFlag(comp.SpawnFlags, GHSpawnFlags::Portal))
            {
                LOG_INFO("module",
                    "GuildHouse: Portal component {} ready (teleport logic next step)",
                    comp.Entry);
            }
        }
    }
}
