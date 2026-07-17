#include "GuildHouseWorld.h"

#include "GuildHouseMgr.h"
#include "GuildHousePhaseMgr.h"
#include "GuildHouseSpawner.h"



void GuildHouseWorldScript::OnStartup()
{
    //
    // Load database definitions
    //

    sGuildHouseMgr.Load();

    sGuildHousePhaseMgr.Load();


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
