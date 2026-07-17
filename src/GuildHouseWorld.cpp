#include "GuildHouseWorld.h"

#include "GuildHouseMgr.h"
#include "GuildHousePhaseMgr.h"
#include "GuildHouseInstanceMgr.h"
#include "GuildHouseSpawner.h"



void GuildHouseWorldScript::OnStartup()
{
    //
    // Load database definitions
    //

    sGuildHouseMgr.Load();

    sGuildHousePhaseMgr.Load();

    sGuildHouseInstanceMgr.Load();


    //
    // Restore placed assets
    //

    sGuildHouseSpawner.LoadPlacedAssets();
}



void GuildHouseWorldScript::OnAfterConfigLoad(bool)
{

}



void AddSC_GuildHouseWorld()
{
    new GuildHouseWorldScript();
}
