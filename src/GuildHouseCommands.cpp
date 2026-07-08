#include "ScriptMgr.h"
#include "Chat.h"
#include "ChatCommand.h"
#include "Player.h"
#include "Creature.h"
#include "ObjectMgr.h"
#include "Map.h"
#include "Guild.h"

#include "GuildHouseMgr.h"

using namespace Acore::ChatCommands;


class GuildHouseCommandScript : public CommandScript
{
public:

    GuildHouseCommandScript()
        : CommandScript("GuildHouseCommandScript")
    {
    }


    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable npcTable =
        {
            { "broker",   HandleAddBroker,   SEC_GAMEMASTER, Console::No },
            { "salesman", HandleAddSalesman, SEC_PLAYER, Console::No }
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



private:

    // =====================================================
    // Permission Check
    // =====================================================

    static bool IsGuildMaster(Player* player)
    {
        if (!player)
            return false;

        Guild* guild = player->GetGuild();

        if (!guild)
            return false;

        return guild->GetLeaderGUID() == player->GetGUID();
    }



    // =====================================================
    // Permanent Creature Spawn
    //
    // Creates a real AzerothCore creature spawn.
    //
    // phaseMask:
    //      0 = normal world
    //      guild phase = guild house visibility
    // =====================================================

    static ObjectGuid::LowType SpawnPermanentCreature(
        Player* player,
        uint32 entry,
        uint32 phaseMask)
    {
        if (!player)
            return 0;


        Map* map = player->GetMap();

        if (!map)
            return 0;


        float x = player->GetPositionX();
        float y = player->GetPositionY();
        float z = player->GetPositionZ();
        float o = player->GetOrientation();



        ObjectGuid::LowType spawnId =
            sObjectMgr->GenerateCreatureSpawnId();



        CreatureData& data =
            sObjectMgr->NewOrExistCreatureData(spawnId);


        data.id          = entry;
        data.mapid       = map->GetId();
        data.phaseMask   = phaseMask;
        data.posX        = x;
        data.posY        = y;
        data.posZ        = z;
        data.orientation = o;



        Creature* creature = new Creature();



        if (!creature->Create(
                map->GenerateLowGuid<HighGuid::Unit>(),
                map,
                phaseMask,
                entry,
                0,
                x,
                y,
                z,
                o))
        {
            delete creature;
            return 0;
        }



        creature->SaveToDB(
            map->GetId(),
            (1 << map->GetSpawnMode()),
            phaseMask);



        ObjectGuid::LowType newSpawnId =
            creature->GetSpawnId();



        creature->CleanupsBeforeDelete();

        delete creature;



        creature = new Creature();



        if (!creature->LoadCreatureFromDB(
                newSpawnId,
                map,
                true,
                true))
        {
            delete creature;
            return 0;
        }



        sObjectMgr->AddCreatureToGrid(
            newSpawnId,
            sObjectMgr->GetCreatureData(newSpawnId));



        return newSpawnId;
    }




    // =====================================================
    // BROKER
    //
    // Global faction NPC.
    //
    // Multiple brokers allowed.
    // =====================================================

    static bool HandleAddBroker(ChatHandler* handler)
    {
        Player* player =
            handler->GetSession()->GetPlayer();


        uint32 entry =
            (player->GetTeamId() == TEAM_ALLIANCE)
                ? 900000
                : 900001;



        ObjectGuid::LowType spawnId =
            SpawnPermanentCreature(
                player,
                entry,
                player->GetPhaseMaskForSpawn());



        if (!spawnId)
        {
            handler->PSendSysMessage(
                "Failed to spawn Guild House Broker.");

            return false;
        }



        handler->PSendSysMessage(
            "Guild House Broker spawned.");

        return true;
    }




    // =====================================================
    // SALESMAN
    //
    // Guild Master only.
    //
    // Permanent guild phased NPC.
    // =====================================================

    static bool HandleAddSalesman(ChatHandler* handler)
    {
        Player* player =
            handler->GetSession()->GetPlayer();



        if (!IsGuildMaster(player))
        {
            handler->PSendSysMessage(
                "Only the Guild Master may place a Guild House salesman.");

            return false;
        }



        uint32 guildId =
            player->GetGuildId();



        if (!guildId)
        {
            handler->PSendSysMessage(
                "You are not in a guild.");

            return false;
        }



        uint32 phase =
            sGuildHouseMgr.GetPhase(guildId);



        if (sGuildHouseMgr.HasSalesman(guildId))
        {
            handler->PSendSysMessage(
                "This guild already has a Guild House salesman.");

            return false;
        }



        uint32 entry =
            (player->GetTeamId() == TEAM_ALLIANCE)
                ? 900002
                : 900003;



        ObjectGuid::LowType spawnId =
            SpawnPermanentCreature(
                player,
                entry,
                phase);



        if (!spawnId)
        {
            handler->PSendSysMessage(
                "Failed to spawn Guild House salesman.");

            return false;
        }



        sGuildHouseMgr.RecordSalesmanSpawn(
            guildId,
            spawnId,
            player->GetMapId(),
            phase,
            player->GetPositionX(),
            player->GetPositionY(),
            player->GetPositionZ(),
            player->GetOrientation());



        handler->PSendSysMessage(
            "Guild House salesman placed.");

        return true;
    }
};



void AddSC_GuildHouseCommands()
{
    new GuildHouseCommandScript();
}
