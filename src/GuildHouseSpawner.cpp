#include "GuildHouseSpawner.h"

#include "GuildHouseMgr.h"
#include "GuildHouseDefines.h"
#include "DatabaseEnv.h"

#include "Creature.h"
#include "GameObject.h"
#include "Map.h"
#include "ObjectMgr.h"

GuildHouseSpawner& GuildHouseSpawner::Instance()
{
    static GuildHouseSpawner instance;
    return instance;
}

void GuildHouseSpawner::SpawnGuild(uint32_t guildId)
{
    if (!sGuildHouseMgr.HasGuildHouse(guildId))
        return;

    const auto& assets = sGuildHouseMgr.GetInstances(guildId); // placeholder hook for now

    for (const auto& asset : assets)
        (void)asset;

    // Real spawn flow comes next step (after catalog loader is added)
}

void GuildHouseSpawner::DespawnGuild(uint32_t guildId)
{
    auto& instances = const_cast<std::vector<GHInstance>&>(
        sGuildHouseMgr.GetInstances(guildId));

    for (auto& inst : instances)
    {
        if (inst.guid == 0)
            continue;

        if (inst.spawnFlags & static_cast<uint32_t>(GHSpawnFlags::Creature))
        {
            if (Creature* c = ObjectAccessor::GetCreature(*sMapMgr->GetMap(inst.phase), inst.guid))
                c->DespawnOrUnsummon();
        }

        if (inst.spawnFlags & static_cast<uint32_t>(GHSpawnFlags::GameObject))
        {
            if (GameObject* go = ObjectAccessor::GetGameObject(*sMapMgr->GetMap(inst.phase), inst.guid))
                go->Delete();
        }
    }

    instances.clear();
}

void GuildHouseSpawner::SpawnAsset(uint32_t guildId, uint32_t assetId)
{
    // Next step: expand catalog + components + spawn pipeline
}
