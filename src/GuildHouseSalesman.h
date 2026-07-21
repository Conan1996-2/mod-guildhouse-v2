#ifndef MOD_GUILDHOUSE_SALESMAN_H
#define MOD_GUILDHOUSE_SALESMAN_H

#include "ScriptMgr.h"

class Player;
class Creature;

// all Salesman will be owned by a guild and listed in table characters.guildhouse_spawns with guildId=guild and assetId=0

class GuildHouseSalesman : public CreatureScript
{
public:

    GuildHouseSalesman() : CreatureScript("GuildHouseSalesman")
    {
    }

    bool OnGossipHello(Player* player, Creature* creature) override;
    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override;

private:

    void SendCatalogMenu(Player* player, Creature* creature);
    void SendCategoryMenu(Player* player, Creature* creature, uint32 categoryId);
    bool ValidateSalesmanAccess(Player* player, Creature* creature);

};


#endif
