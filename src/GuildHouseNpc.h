#ifndef MOD_GUILDHOUSE_NPC_H
#define MOD_GUILDHOUSE_NPC_H

#include "Common.h"

class npc_guildhouse : public CreatureScript
{
public:
    npc_guildhouse() : CreatureScript("npc_guildhouse") {}

    bool OnGossipHello(Player* player, Creature* creature) override;
    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override;
};

#endif
