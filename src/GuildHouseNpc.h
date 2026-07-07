#ifndef MOD_GUILDHOUSE_NPC_H
#define MOD_GUILDHOUSE_NPC_H

#include "ScriptMgr.h"

class Player;
class Creature;


class GuildHouseNpc : public CreatureScript
{
public:

    GuildHouseNpc()
        : CreatureScript("GuildHouseNpc")
    {
    }


    bool OnGossipHello(
        Player* player,
        Creature* creature) override;


    bool OnGossipSelect(
        Player* player,
        Creature* creature,
        uint32 sender,
        uint32 action) override;


private:

    bool IsGuildMaster(Player* player);

    void SendMainMenu(Player* player, Creature* creature);

#endif
