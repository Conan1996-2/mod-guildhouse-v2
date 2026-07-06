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
    
        float x = player->GetPositionX();
        float y = player->GetPositionY();
        float z = player->GetPositionZ();
        float o = player->GetOrientation();
    
        Map* map = player->GetMap();
    
        ObjectGuid::LowType spawnId = SpawnPermanentCreature(player, entry);
    
        if (!spawnId)
        {
            handler->PSendSysMessage("Failed to spawn broker.");
            return false;
        }
    
        handler->PSendSysMessage("Guild Broker spawned.");
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
            handler->PSendSysMessage("You are not in a guild.");
            return false;
        }
    
        // ONLY GUILD MASTER CAN PLACE
        if (player->GetGUID() != sGuildHouseMgr.GetGuildHouse(guildId)->OwnerGuid)
        {
            handler->PSendSysMessage("Only the Guild Master can place the salesman.");
            return false;
        }
    
        uint32 phase = sGuildHouseMgr.GetPhase(guildId);
    
        // enforce single salesman per guild (DB check inside manager would be better)
        if (sGuildHouseMgr.HasSalesman(guildId))
        {
            handler->PSendSysMessage("Guild already has a salesman.");
            return false;
        }
    
        uint32 entry = (player->GetTeamId() == TEAM_ALLIANCE)
            ? 900002
            : 900003;
    
        ObjectGuid::LowType spawnId = SpawnPermanentCreature(player, entry);
    
        if (!spawnId)
        {
            handler->PSendSysMessage("Failed to spawn salesman.");
            return false;
        }
    
        // IMPORTANT: persist as guildhouse_asset (NOT instance table)
        sGuildHouseMgr.RecordSalesmanSpawn(
            guildId,
            spawnId,
            player->GetMapId(),
            phase,
            player->GetPositionX(),
            player->GetPositionY(),
            player->GetPositionZ(),
            player->GetOrientation()
        );
    
        handler->PSendSysMessage("Guild Salesman placed.");
        return true;
    }

void AddSC_GuildHouseCommands()
{
    new GuildHouseCommandScript();
}
