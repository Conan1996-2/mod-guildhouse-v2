#include "Player.h"
#include "PlayerScript.h"
#include "GuildHousePhaseMgr.h"

class GuildHousePlayerScript : public PlayerScript
{
public:
    GuildHousePlayerScript() : PlayerScript("GuildHousePlayerScript") { }

    void OnPlayerUpdate(Player* player, uint32 /*diff*/) override
    {
        if (sGuildHousePhaseMgr.IsMember(player))
            sGuildHousePhaseMgr.CheckBoundary(player);
    }

    void OnPlayerMapChanged(Player* player) override
    {
        if (sGuildHousePhaseMgr.IsMember(player))
            sGuildHousePhaseMgr.LeavePhase(player);
    }

    void OnPlayerLogout(Player* player) override
    {
        if (sGuildHousePhaseMgr.IsMember(player))
            sGuildHousePhaseMgr.LeavePhase(player);
    }
};

void AddSC_GuildHouseScripts()
{
    new GuildHousePlayerScript();
}
