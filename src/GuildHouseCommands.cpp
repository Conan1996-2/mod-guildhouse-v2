#include "ScriptMgr.h"
#include "Chat.h"
#include "ChatCommand.h"
#include "Player.h"
#include "Creature.h"
#include "ObjectMgr.h"
#include "Map.h"
#include "GuildHouseMgr.h"
#include "DatabaseEnv.h"

using namespace Acore::ChatCommands;

class GuildHouseCommandScript : public CommandScript
{
public:
    GuildHouseCommandScript() : CommandScript("GuildHouseCommandScript") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable npcTable =
        {
            { "broker",   HandleAddBroker,   SEC_GAMEMASTER, Console::No },
            { "salesman", HandleAddSalesman, SEC_GAMEMASTER, Console::No }
        };

        static ChatCommandTable addTable =
        {
            { "npc", npcTable }
        };

        static ChatCommandTable root =
        {
            { "gh", addTable },
            { "guildhouse", addTable }
        };

        return root;
    }

    // -------------------------------------------------------
    // Spawn core (same pipeline as .npc add)
    // -------------------------------------------------------
    static bool SpawnPermanentCreature(Player* player, uint32 entry)
    {
        float x = player->GetPositionX();
        float y = player->GetPositionY();
        float z = player->GetPositionZ();
        float o = player->GetOrientation();

        Map* map = player->GetMap();

        ObjectGuid::LowType spawnId = sObjectMgr->GenerateCreatureSpawnId();

        CreatureData& data = sObjectMgr->NewOrExistCreatureData(spawnId);
        data.id = entry;
        data.mapid = map->GetId();   // IMPORTANT: mapid (your correction)
        data.phaseMask = player->GetPhaseMaskForSpawn();
        data.posX = x;
        data.posY = y;
        data.posZ = z;
        data.orientation = o;

        Creature* creature = new Creature();

        if (!creature->Create(map->GenerateLowGuid<HighGuid::Unit>(),
                              map,
                              player->GetPhaseMaskForSpawn(),
                              entry, 0,
                              x, y, z, o))
        {
            delete creature;
            return false;
        }

        creature->SaveToDB(map->GetId(),
                           (1 << map->GetSpawnMode()),
                           player->GetPhaseMaskForSpawn());

        ObjectGuid::LowType newSpawnId = creature->GetSpawnId();

        creature->CleanupsBeforeDelete();
        delete creature;

        creature = new Creature();

        if (!creature->LoadCreatureFromDB(newSpawnId, map, true, true))
        {
            delete creature;
            return false;
        }

        sObjectMgr->AddCreatureToGrid(newSpawnId,
            sObjectMgr->GetCreatureData(newSpawnId));

        return true;
    }

    // -------------------------------------------------------
    // BROKER (GM only, global world NPC)
    // -------------------------------------------------------
    static bool HandleAddBroker(ChatHandler* handler)
    {
        Player* player = handler->GetSession()->GetPlayer();

        uint32 entry = (player->GetTeamId() == TEAM_ALLIANCE)
            ? 900000
            : 900001;

        if (!SpawnPermanentCreature(player, entry))
        {
            ChatHandler(player->GetSession()).PSendSysMessage(
                "Failed to spawn Guild House Broker.");
            return false;
        }

        ChatHandler(player->GetSession()).PSendSysMessage(
            "Guild House Broker spawned.");

        return true;
    }

    // -------------------------------------------------------
    // SALESMAN (GM or Guild Master, guild + phase bound)
    // -------------------------------------------------------
    static bool HandleAddSalesman(ChatHandler* handler)
    {
        Player* player = handler->GetSession()->GetPlayer();

        Guild* guild = player->GetGuild();
        if (!guild)
        {
            ChatHandler(player->GetSession()).PSendSysMessage(
                "You are not in a guild.");
            return true;
        }

        uint32 guildId = player->GetGuildId();
        uint32 phase   = GuildHouseMgr::Instance().GetPhase(guildId);

        uint32 entry = (player->GetTeamId() == TEAM_ALLIANCE)
            ? 900002
            : 900003;

        // ---------------------------------------------------
        // DB CHECK (NO WORLD SCANNING)
        // ---------------------------------------------------
        std::ostringstream ss;
        ss << "SELECT guid FROM guildhouse_instance "
              "WHERE guildId = " << guildId
           << " AND phase = " << phase
           << " AND type = 1 LIMIT 1"; // 1 = salesman

        QueryResult result = CharacterDatabase.Query(ss.str());

        if (result)
        {
            ChatHandler(player->GetSession()).PSendSysMessage(
                "Salesman already exists for this guild.");
            return true;
        }

        // ---------------------------------------------------
        // SPAWN
        // ---------------------------------------------------
        if (!SpawnPermanentCreature(player, entry))
        {
            ChatHandler(player->GetSession()).PSendSysMessage(
                "Failed to spawn Guild House Salesman.");
            return false;
        }

        // ---------------------------------------------------
        // RECORD INTO MODULE DB
        // ---------------------------------------------------
        std::ostringstream ins;
        ins << "INSERT INTO guildhouse_instance "
               "(guildId, assetId, catalogId, guid, type, mapid, phase, x, y, z, o) VALUES ("
            << guildId << ","
            << 0 << ","
            << 0 << ","
            << 0 << ","
            << 1 << "," // salesman
            << player->GetMapId() << ","
            << phase << ","
            << player->GetPositionX() << ","
            << player->GetPositionY() << ","
            << player->GetPositionZ() << ","
            << player->GetOrientation()
            << ")";

        CharacterDatabase.Execute(ins.str());

        ChatHandler(player->GetSession()).PSendSysMessage(
            "Guild House Salesman spawned.");

        return true;
    }
};

void AddSC_GuildHouseCommands()
{
    new GuildHouseCommandScript();
}
