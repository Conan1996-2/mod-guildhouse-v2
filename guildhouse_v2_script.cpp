#include "ScriptMgr.h"
#include "Player.h"
#include "Chat.h"
#include "ScriptedGossip.h"
#include "Config.h"

class GuildHouseV2NPC : public CreatureScript
{
public:
    GuildHouseV2NPC() : CreatureScript("GuildHouseV2NPC") { }

}


// Function called by the loader
void Addmod_guildhouse_v2_Scripts()
{
    new GuildHouseV2NPC();
}
