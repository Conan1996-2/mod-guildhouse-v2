#include "GuildHouseInstanceMgr.h"
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
                    GHInstanceRecord rec;
                    rec.guildId   = guildId;
                    rec.assetId   = asset.AssetId;
                    rec.catalogId = asset.CatalogId;
                    rec.guid      = creature->GetGUID().GetCounter();
                    rec.type      = 0; // creature
                    rec.mapId     = map->GetId();
                    rec.phase     = GuildHouseUtil::GetGuildHousePhase(guildId);
                    rec.x = x; rec.y = y; rec.z = z; rec.o = o;
            
                    sGuildHouseInstanceMgr.AddInstance(rec);
            
                    LOG_INFO("module",
                        "GH: Stored creature {} for guild {}",
                        rec.guid, guildId);
                }
            }

            // =====================================================
            // GAMEOBJECT SPAWN
            // =====================================================
            if (GuildHouseUtil::HasFlag(comp.SpawnFlags, GHSpawnFlags::GameObject))
            {
                Position pos;
                pos.Relocate(x, y, z, o);
            
                if (GameObject* go = map->SummonGameObject(
                        comp.Entry,
                        x, y, z, o,
                        0,
                        GO_STATE_READY))
                {
                    GHInstanceRecord rec;
                    rec.guildId   = guildId;
                    rec.assetId   = asset.AssetId;
                    rec.catalogId = asset.CatalogId;
                    rec.guid      = go->GetGUID().GetCounter();
                    rec.type      = 1; // gameobject
                    rec.mapId     = map->GetId();
                    rec.phase     = GetPhase(guildId);
                    rec.x = x; rec.y = y; rec.z = z; rec.o = o;
            
                    sGuildHouseInstanceMgr.AddInstance(rec);
            
                    LOG_INFO("module",
                        "GH: Stored GO {} for guild {}",
                        rec.guid, guildId);
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
