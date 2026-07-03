#include "ScriptMgr.h"
#include "Chat.h"
#include "ChatCommand.h"
#include "Player.h"

using namespace Acore::ChatCommands;

class GuildHouseCommandScript : public CommandScript
{
public:
    GuildHouseCommandScript() : CommandScript("GuildHouseCommandScript") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable addNpcTable =
        {
            { "broker", HandleAddBroker, SEC_GAMEMASTER, Console::No },
            // salesman comes next
        };

        static ChatCommandTable npcTable =
        {
            { "npc", addNpcTable }
        };

        static ChatCommandTable addTable =
        {
            { "add", npcTable }
        };

        static ChatCommandTable ghTable =
        {
            { "gh", addTable },
            { "guildhouse", addTable }
        };

        return ghTable;
    }

    static bool HandleAddBroker(ChatHandler* handler)
    {
        Player* player = handler->GetSession()->GetPlayer();

        ChatHandler(player->GetSession()).PSendSysMessage(
            "GuildHouse: Broker command received.");

        return true;
    }
};

void AddSC_GuildHouseCommands()
{
    new GuildHouseCommandScript();
}
