#include "GuildHouseMgr.h"

GuildHouseMgr* GuildHouseMgr::instance()
{
    static GuildHouseMgr mgr;
    return &mgr;
}
