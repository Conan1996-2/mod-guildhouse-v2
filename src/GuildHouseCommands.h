#ifndef MOD_GUILDHOUSE_COMMANDS_H
#define MOD_GUILDHOUSE_COMMANDS_H

#include "ScriptMgr.h"

using namespace Acore::ChatCommands;

class GuildHouseCommandScript : public CommandScript
{
public:

    GuildHouseCommandScript();

    ChatCommandTable GetCommands() const;

    static bool CanManageGuildHouse(Player* player);

    static bool HandleAddBroker(ChatHandler* handler);

    static bool HandleAddSalesman(ChatHandler* handler);

    static bool HandleListAssets(ChatHandler* handler, char const* args);

    static bool HandlePlaceAsset(ChatHandler* handler, char const* args);

    static bool HandleMoveAsset(ChatHandler* handler, char const* args);

    static bool HandleStoreAsset(ChatHandler* handler, char const* args);

    static bool HandleSellAsset(ChatHandler* handler, char const* args);

    void AddSC_GuildHouseCommands();

};
    
#endif
