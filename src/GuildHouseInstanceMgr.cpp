#include "GuildHouseInstanceMgr.h"

#include "Log.h"


GuildHouseInstanceMgr&
GuildHouseInstanceMgr::Instance()
{
    static GuildHouseInstanceMgr instance;

    return instance;
}




void GuildHouseInstanceMgr::Load()
{
    /*
        Deprecated.

        Permanent GuildHouse objects are loaded
        from guildhouse_asset.
    */

    LOG_INFO(
        "module",
        "GuildHouseInstanceMgr disabled (permanent spawn mode)");
}




void GuildHouseInstanceMgr::LoadInstances()
{
    /*
        Deprecated.

        No runtime instances exist.
    */
}




void GuildHouseInstanceMgr::Save(
    const GHInstanceRecord& record)
{
    /*
        Deprecated.

        Do not save runtime GUIDs.

        Permanent records belong in:
            guildhouse_asset
    */
}




void GuildHouseInstanceMgr::AddInstance(
    const GHInstanceRecord& record)
{
    /*
        Deprecated.

        Previously stored spawned objects.

        No longer used.
    */
}




void GuildHouseInstanceMgr::RemoveGuild(
    uint32_t guildId)
{
    /*
        Deprecated.

        Do NOT remove permanent assets.

        Permanent removal will be handled
        by future GM commands.
    */
}
