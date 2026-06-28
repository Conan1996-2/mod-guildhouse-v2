#include "GuildHouseBuilderNpc.h"
#include "GuildHouseMgr.h"
#include "GuildHouseConfig.h"
#include "GuildHouseSpawner.h"

bool npc_guildhouse_builder::OnGossipHello(Player* player, Creature* creature)
{
    if (!player->GetGuildId())
    {
        player->ADD_GOSSIP_ITEM(0, "You are not in a guild.", GOSSIP_SENDER_MAIN, 0);
        player->SEND_GOSSIP_MENU(1, creature->GetGUID());
        return true;
    }

    player->ADD_GOSSIP_ITEM(0, "Buy NPC for Guildhouse", GOSSIP_SENDER_MAIN, 1);
    player->ADD_GOSSIP_ITEM(0, "Buy GameObject for Guildhouse", GOSSIP_SENDER_MAIN, 2);
    player->ADD_GOSSIP_ITEM(0, "Buy Portal for Guildhouse", GOSSIP_SENDER_MAIN, 3);
    player->ADD_GOSSIP_ITEM(0, "Teleport to Guildhouse", GOSSIP_SENDER_MAIN, 4);

    player->SEND_GOSSIP_MENU(1, creature->GetGUID());
    return true;
}

bool npc_guildhouse_builder::OnGossipSelect(Player* player, Creature* creature, uint32, uint32 action)
{
    player->PlayerTalkClass->ClearMenus();

    uint32 guildId = player->GetGuildId();

    if (!guildId)
    {
        player->CLOSE_GOSSIP_MENU();
        return true;
    }

if (action == 4)
{
    sGuildHouseMgr->Teleport(player);
    player->CLOSE_GOSSIP_MENU();
    return true;
}

if (action == 1)
{
    uint64 cost = sGuildHouseConfig.GetCost() / 10;

    if (player->GetMoney() < cost)
        return true;

    player->ModifyMoney(-(int64)cost);

    uint32 entry = 100000 + urand(1, 99999); // placeholder NPC

    WorldDatabase.PExecute(
        "INSERT INTO guildhouse_npc (guildId, entry, map, x, y, z, o) "
        "VALUES (%u, %u, %u, %f, %f, %f, %f)",
        guildId,
        entry,
        GH_MAP,
        GH_X, GH_Y, GH_Z, GH_O);

    sGuildHouseSpawner->LoadAll();
    sGuildHouseSpawner->SpawnGuild(guildId);
}

if (action == 2)
{
    uint64 cost = sGuildHouseConfig.GetCost() / 20;

    if (player->GetMoney() < cost)
        return true;

    player->ModifyMoney(-(int64)cost);

    uint32 entry = 200000 + urand(1, 99999);

    WorldDatabase.PExecute(
        "INSERT INTO guildhouse_object (guildId, entry, map, x, y, z, o) "
        "VALUES (%u, %u, %u, %f, %f, %f, %f)",
        guildId,
        entry,
        GH_MAP,
        GH_X, GH_Y, GH_Z, GH_O);

    sGuildHouseSpawner->LoadAll();
    sGuildHouseSpawner->SpawnGuild(guildId);
}

if (action == 3)
{
    uint64 cost = sGuildHouseConfig.GetCost() / 5;

    if (player->GetMoney() < cost)
        return true;

    player->ModifyMoney(-(int64)cost);

    WorldDatabase.PExecute(
        "INSERT INTO guildhouse_portal "
        "(guildId, map, x, y, z, o, targetMap, targetX, targetY, targetZ, targetO) "
        "VALUES (%u,%u,%f,%f,%f,%f,%u,%f,%f,%f,%f)",
        guildId,
        GH_MAP,
        GH_X, GH_Y, GH_Z, GH_O,
        0,
        16222.0f, 16265.0f, 13.0f, 0.0f);

    sGuildHouseSpawner->LoadAll();
    sGuildHouseSpawner->SpawnGuild(guildId);
}

void GuildHouseSpawner::SpawnGuild(uint32 guildId)
{
    DespawnGuild(guildId);

void GuildHouseSpawner::DespawnGuild(uint32 guildId)
{
    uint32 phase = guildId + sGuildHouseConfig.GetPhaseOffset();

    for (auto& itr : MapMgr::Instance()->GetMapRange())
    {
        Map* map = itr.second;

        map->DoForAllPlayers([&](Player* player)
        {
            if (player->GetPhaseMask() & phase)
            {
                player->SetPhaseMask(1, true);
            }
        });
    }
}

