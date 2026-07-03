#include "ScriptMgr.h"
#include "Chat.h"
#include "ChatCommand.h"
#include "Player.h"
#include "Creature.h"
#include "ObjectMgr.h"
#include "Map.h"

using namespace Acore::ChatCommands;

class GuildHouseCommandScript : public CommandScript
{
public:
    GuildHouseCommandScript() : CommandScript("GuildHouseCommandScript") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable npcSubTable =
        {
            { "broker",   HandleAddBroker,   SEC_GAMEMASTER, Console::No },
            { "salesman", HandleAddSalesman, SEC_GAMEMASTER, Console::No }
        };

        static ChatCommandTable npcTable =
        {
            { "npc", npcSubTable }
        };

        static ChatCommandTable addTable =
        {
            { "add", npcTable }
        };

        static ChatCommandTable root =
        {
            { "gh", addTable },
            { "guildhouse", addTable }
        };

        return root;
    }

    // -------------------------------------------------------
    // Helpers
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
        data.mapId = map->GetId();
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

    static bool ExistsNearPlayer(Player* player, uint32 entry, float range = 5.0f)
    {
        Map* map = player->GetMap();

        for (auto const& obj : map->GetObjectsStore())
        {
            if (Creature* creature = obj.second->ToCreature())
            {
                if (creature->GetEntry() == entry &&
                    creature->IsWithinDist(player, range))
                {
                    return true;
                }
            }
        }

        return false;
    }

    // -------------------------------------------------------
    // Broker (GM only)
    // -------------------------------------------------------
    static bool HandleAddBroker(ChatHandler* handler)
    {
        Player* player = handler->GetSession()->GetPlayer();

        uint32 entry = (player->GetTeam() == ALLIANCE)
            ? 900000
            : 900001;

        if (ExistsNearPlayer(player, entry))
        {
            ChatHandler(player->GetSession()).PSendSysMessage(
                "Broker already exists nearby.");
            return true;
        }

        if (!SpawnPermanentCreature(player, entry))
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
    // Salesman (GM or Guild Master later expanded)
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

        uint32 entry = (player->GetTeam() == ALLIANCE)
            ? 900002
            : 900003;

        if (ExistsNearPlayer(player, entry))
        {
            ChatHandler(player->GetSession()).PSendSysMessage(
                "Salesman already exists.");
            return true;
        }

        if (!SpawnPermanentCreature(player, entry))
        {
            ChatHandler(player->GetSession()).PSendSysMessage(
                "Failed to spawn salesman.");
            return false;
        }

        ChatHandler(player->GetSession()).PSendSysMessage(
            "Guild House Salesman spawned.");

        return true;
    }
};

void AddSC_GuildHouseCommands()
{
    new GuildHouseCommandScript();
}
