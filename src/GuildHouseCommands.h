#ifndef MOD_GUILDHOUSE_COMMANDS_H
#define MOD_GUILDHOUSE_COMMANDS_H

#include "ScriptMgr.h"

using namespace Acore::ChatCommands;

class GuildHouseCommandScript : public CommandScript
{
public:

    GuildHouseCommandScript();

    ChatCommandTable GetCommands() const override;


    // =====================================================
    // NPC MANAGEMENT
    // =====================================================

    static bool HandleAddBroker(ChatHandler* handler);

    static bool HandleAddSalesman(ChatHandler* handler);


    // =====================================================
    // GUILD HOUSE MANAGEMENT
    // =====================================================

    static bool HandleSellGuildHouse(ChatHandler* handler, char const* args);

    static bool HandleTeleportGuildHouse(ChatHandler* handler, char const* args);


    // =====================================================
    // ASSET MANAGEMENT
    // =====================================================

    static bool HandleListAssets(ChatHandler* handler, char const* args);

    static bool HandlePlaceAsset(ChatHandler* handler, char const* args);

    static bool HandleMoveAsset(ChatHandler* handler, char const* args);

    static bool HandleStoreAsset(ChatHandler* handler, char const* args);

    static bool HandleSellAsset(ChatHandler* handler, char const* args);


    // =====================================================
    // CATALOG / SHOP
    // =====================================================

    static bool HandleListCategories(ChatHandler* handler, char const* args);

    static bool HandleListCatalog(ChatHandler* handler, char const* args);

    static bool HandlePurchaseCatalog(ChatHandler* handler, char const* args);
};


// =====================================================
// Script registration
// =====================================================

void AddSC_GuildHouseCommands();


#endif
