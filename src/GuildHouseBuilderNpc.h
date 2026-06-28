#ifndef MOD_GUILDHOUSE_BUILDER_NPC_H
#define MOD_GUILDHOUSE_BUILDER_NPC_H

#include "Common.h"

class npc_guildhouse_builder : public CreatureScript
{
public:
    npc_guildhouse_builder() : CreatureScript("npc_guildhouse_builder") {}

    bool OnGossipHello(Player* player, Creature* creature) override;
    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override;
};

#endif
