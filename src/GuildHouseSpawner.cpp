#include "GuildHouseSpawner.h"
#include "GuildHouseConfig.h"

GuildHouseSpawner* GuildHouseSpawner::instance()
{
    static GuildHouseSpawner inst;
    return &inst;
}

void GuildHouseSpawner::LoadAll()
{
    _npcs.clear();
    _objects.clear();

QueryResult npcResult = WorldDatabase.Query(
    "SELECT guildId, entry, map, x, y, z, o FROM guildhouse_npc");

if (npcResult)
{
    do
    {
        Field* f = npcResult->Fetch();

        GHSpawnNpc npc;
        npc.guildId = f[0].Get<uint32>();
        npc.entry   = f[1].Get<uint32>();
        npc.map     = f[2].Get<uint32>();
        npc.x       = f[3].Get<float>();
        npc.y       = f[4].Get<float>();
        npc.z       = f[5].Get<float>();
        npc.o       = f[6].Get<float>();

        _npcs[npc.guildId].push_back(npc);

    } while (npcResult->NextRow());
}

QueryResult objResult = WorldDatabase.Query(
    "SELECT guildId, entry, map, x, y, z, o FROM guildhouse_object");

if (objResult)
{
    do
    {
        Field* f = objResult->Fetch();

        GHSpawnObject obj;
        obj.guildId = f[0].Get<uint32>();
        obj.entry   = f[1].Get<uint32>();
        obj.map     = f[2].Get<uint32>();
        obj.x       = f[3].Get<float>();
        obj.y       = f[4].Get<float>();
        obj.z       = f[5].Get<float>();
        obj.o       = f[6].Get<float>();

        _objects[obj.guildId].push_back(obj);

    } while (objResult->NextRow());
}

LOG_INFO("module", "GuildHouse: Loaded spawns for {} guilds",
    _npcs.size());
}

void GuildHouseSpawner::SpawnGuild(uint32 guildId)
{
    uint32 phase = guildId + sGuildHouseConfig.GetPhaseOffset();

auto npcIt = _npcs.find(guildId);

if (npcIt != _npcs.end())
{
    for (const auto& npc : npcIt->second)
        SpawnNPC(npc, phase);
}

auto objIt = _objects.find(guildId);

if (objIt != _objects.end())
{
    for (const auto& obj : objIt->second)
        SpawnObject(obj, phase);
}
}

void GuildHouseSpawner::SpawnNPC(const GHSpawnNpc& data, uint32 phase)
{
    Creature* creature = new Creature();

    if (!creature->Create(
        ObjectGuid::Create<HighGuid::Unit>(data.entry, 1),
        nullptr,
        data.map,
        data.x, data.y, data.z, data.o))
    {
        delete creature;
        return;
    }

creature->SetPhaseMask(phase, true);

Map* map = creature->GetMap();
map->AddToMap(creature);
}

void GuildHouseSpawner::SpawnObject(const GHSpawnObject& data, uint32 phase)
{
    GameObject* go = new GameObject();

    if (!go->Create(
        ObjectGuid::Create<HighGuid::GameObject>(data.entry, 1),
        data.entry,
        nullptr,
        data.map,
        data.x, data.y, data.z, data.o,
        QuaternionData()))
    {
        delete go;
        return;
    }

go->SetPhaseMask(phase, true);

Map* map = go->GetMap();
map->AddToMap(go);
}

class GuildHouseWorld : public WorldScript
{
public:
    GuildHouseWorld() : WorldScript("GuildHouseWorld") {}

    void OnStartup() override
    {
        sGuildHouseSpawner->LoadAll();
    }
};
