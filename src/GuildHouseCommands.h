#ifndef MOD_GUILDHOUSE_COMMANDS_H
#define MOD_GUILDHOUSE_COMMANDS_H

#include "ScriptMgr.h"

class GuildHouseCommandScript : public CommandScript
{
public:

    GuildHouseCommandScript();

    Acore::ChatCommands::ChatCommandTable GetCommands() const override;

private:

    static bool HandleList(
        ChatHandler* handler,
        char const* args);


    static bool HandlePlace(
        ChatHandler* handler,
        char const* args);


    static bool HandleListAssets(
        ChatHandler* handler,
        char const* args);


    static bool HandlePlaceAsset(
        ChatHandler* handler,
        char const* args);


    static bool HandleMoveAsset(
        ChatHandler* handler,
        char const* args);


    static bool HandleStoreAsset(
        ChatHandler* handler,
        char const* args);


    static bool HandleSellAsset(
        ChatHandler* handler,
        char const* args);

};
    
#endif
