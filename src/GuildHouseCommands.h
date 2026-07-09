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
            { "broker",   GuildHouseCommandScript::HandleAddBroker,   SEC_GAMEMASTER, Console::No },
            { "salesman", GuildHouseCommandScript::HandleAddSalesman, SEC_PLAYER, Console::No }
        };
                
        static ChatCommandTable assetTable =
        {
            { "list",  GuildHouseCommandScript::HandleListAssets,  SEC_PLAYER, Console::No },
            { "place", GuildHouseCommandScript::HandlePlaceAsset,  SEC_PLAYER, Console::No },
            { "move",  GuildHouseCommandScript::HandleMoveAsset,   SEC_PLAYER, Console::No },
            { "store", GuildHouseCommandScript::HandleStoreAsset,  SEC_PLAYER, Console::No },
            { "sell",  GuildHouseCommandScript::HandleSellAsset,   SEC_PLAYER, Console::No }
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
