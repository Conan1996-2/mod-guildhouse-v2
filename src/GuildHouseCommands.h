#ifndef MOD_GUILDHOUSE_COMMANDS_H
#define MOD_GUILDHOUSE_COMMANDS_H

#include "ScriptMgr.h"

using namespace Acore::ChatCommands;

class GuildHouseCommandScript : public CommandScript
{
public:

    GuildHouseCommandScript();

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable npcTable =
        {
            { "broker",   HandleAddBroker,   SEC_GAMEMASTER, Console::No },
            { "salesman", HandleAddSalesman, SEC_PLAYER, Console::No }
        };
                
        static ChatCommandTable assetTable =
        {
            { "list",  HandleListAssets,  SEC_PLAYER, Console::No },
            { "place", HandlePlaceAsset,  SEC_PLAYER, Console::No },
            { "move",  HandleMoveAsset,   SEC_PLAYER, Console::No },
            { "store", HandleStoreAsset,  SEC_PLAYER, Console::No },
            { "sell",  HandleSellAsset,   SEC_PLAYER, Console::No }
        };
                
        static ChatCommandTable guildHouseTable =
        {
            { "npc",   npcTable },
            { "asset", assetTable }
        };
                
        static ChatCommandTable root =
        {
            { "gh",          guildHouseTable },
            { "guildhouse",  guildHouseTable }
        };

        return root;
    }

private:

    static bool HandleList(ChatHandler* handler, char const* args);

    static bool HandlePlace(ChatHandler* handler, char const* args);

    static bool HandleListAssets(ChatHandler* handler, char const* args);

    static bool HandlePlaceAsset(ChatHandler* handler, char const* args);

    static bool HandleMoveAsset(ChatHandler* handler, char const* args);

    static bool HandleStoreAsset(ChatHandler* handler, char const* args);

    static bool HandleSellAsset(ChatHandler* handler, char const* args);

};
    
#endif
