#ifndef MOD_GUILDHOUSE_NPC_H
#define MOD_GUILDHOUSE_NPC_H

#include "ScriptMgr.h"
#include "Player.h"
#include "Creature.h"

class GuildHouseNpc : public CreatureScript
{
public:
    GuildHouseNpc() : CreatureScript("GuildHouseNpc") { }

    bool OnGossipHello(Player* player, Creature* creature) override;
    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override;

private:
    void SendMainMenu(Player* player, Creature* creature);
};

#endif
