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
    static ObjectGuid::LowType SpawnPermanentCreature(Player* player, uint32 entry)
    {
        float x = player->GetPositionX();
        float y = player->GetPositionY();
        float z = player->GetPositionZ();
        float o = player->GetOrientation();
    
        Map* map = player->GetMap();
    
        ObjectGuid::LowType spawnId = sObjectMgr->GenerateCreatureSpawnId();
    
        CreatureData& data = sObjectMgr->NewOrExistCreatureData(spawnId);
        data.id = entry;
        data.mapid = map->GetId();
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
            return 0;
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
            return 0;
        }
    
        sObjectMgr->AddCreatureToGrid(newSpawnId,
            sObjectMgr->GetCreatureData(newSpawnId));
    
        return newSpawnId;
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

        ObjectGuid::LowType spawnId = SpawnPermanentCreature(player, entry);
        
        if (!spawnId)
        {
            ChatHandler(player->GetSession()).PSendSysMessage(
                "Failed to spawn broker.");
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

        uint32 guildId = player->GetGuildId();
        
        if (!guildId)
        {
            ChatHandler(player->GetSession()).PSendSysMessage(
                "You are not in a guild.");
            return true;
        }

        uint32 phase = GuildHouseMgr::Instance().GetPhase(guildId);
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
        ObjectGuid::LowType spawnId = SpawnPermanentCreature(player, entry);
        
        if (!spawnId)
        {
            ChatHandler(player->GetSession()).PSendSysMessage(
                "Failed to spawn salesman.");
            return false;
        }

        // ---------------------------------------------------
        // RECORD INTO MODULE DB
        // ---------------------------------------------------
        std::ostringstream ins;
        
        ins << "INSERT INTO guildhouse_instance "
               "(guildId, assetId, catalogId, guid, type, mapid, phase, x, y, z, o) VALUES ("
            << guildId << ","
            << 0 << ","              // assetId (not used yet)
            << 0 << ","              // catalogId (not used yet)
            << spawnId << ","        // ✔ REAL GUID FIX
            << 1 << ","              // salesman type
            << player->GetMapId() << ","
            << phase << ","
            << x << ","
            << y << ","
            << z << ","
            << o
            << ")";

CharacterDatabase.Execute(ins.str());

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
