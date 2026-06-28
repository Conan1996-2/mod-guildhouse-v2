#include "GuildHouseMgr.h"
#include "GuildHouseConfig.h"

GuildHouseMgr* GuildHouseMgr::instance()
{
    static GuildHouseMgr instance;
    return &instance;
}

void GuildHouseMgr::Load()
{
    _guilds.clear();

    QueryResult result = CharacterDatabase.Query(
        "SELECT guildId FROM guildhouse");

    if (!result)
        return;

    do
    {
        Field* fields = result->Fetch();
        _guilds.insert(fields[0].Get<uint32>());
    } while (result->NextRow());
}

bool GuildHouseMgr::HasHouse(uint32 guildId) const
{
    return _guilds.count(guildId) > 0;
}

bool GuildHouseMgr::Purchase(Player* player)
{
    if (!player || !sGuildHouseConfig.IsEnabled())
        return false;

    uint32 guildId = player->GetGuildId();
    if (!guildId || HasHouse(guildId))
        return false;

    uint64 cost = sGuildHouseConfig.GetCost();

    if (player->GetMoney() < cost)
        return false;

    player->ModifyMoney(-(int64)cost);

    CharacterDatabase.PExecute(
        "INSERT INTO guildhouse (guildId, ownerGuid) VALUES (%u, %u)",
        guildId,
        player->GetGUID().GetCounter());

    _guilds.insert(guildId);

    return true;
}

bool GuildHouseMgr::Teleport(Player* player)
{
    if (!player)
        return false;

    uint32 guildId = player->GetGuildId();

    if (!HasHouse(guildId))
        return false;

    player->SetPhaseMask(GetGuildPhase(guildId), true);

    player->TeleportTo(GH_MAP, GH_X, GH_Y, GH_Z, GH_O);

    return true;
}

uint32 GuildHouseMgr::GetGuildPhase(uint32 guildId) const
{
    return guildId + sGuildHouseConfig.GetPhaseOffset();
}
