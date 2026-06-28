#include "GuildHouseMgr.h"

GuildHouseMgr* GuildHouseMgr::instance()
{
    static GuildHouseMgr mgr;
    return &mgr;
}

void GuildHouseMgr::Load()
{
    _houses.clear();

    QueryResult result = WorldDatabase.Query("SELECT guildId, ownerGuid, map, position_x, position_y, position_z, orientation FROM guildhouse");

    if (!result)
    {
        LOG_INFO("module", "GuildHouse: 0 guild houses loaded.");
        return;
    }

    do
    {
        Field* fields = result->Fetch();
        GuildHouseData data;

        data.GuildId   = fields[0].Get<uint32>();
        data.OwnerGuid = fields[1].Get<uint32>();
        data.Map = fields[2].Get<uint32>();
        data.X = fields[3].Get<float>();
        data.Y = fields[4].Get<float>();
        data.Z = fields[5].Get<float>();
        data.O = fields[6].Get<float>();

        _houses[data.GuildId] = data;

    } while (result->NextRow());

    LOG_INFO("module",
        "GuildHouse: Loaded {} guild houses.",
        _houses.size());
}

bool GuildHouseMgr::HasGuildHouse(uint32 guildId) const
{
    return _houses.find(guildId) != _houses.end();
}

const GuildHouseData*
GuildHouseMgr::GetGuildHouse(uint32 guildId) const
{
    auto itr = _houses.find(guildId);

    if (itr == _houses.end())
        return nullptr;

    return &itr->second;
}

bool GuildHouseMgr::Purchase(Player* player)
{
    if (!player)
        return false;

    uint32 guildId = player->GetGuildId();

    if (!guildId)
        return false;

    if (HasGuildHouse(guildId))
        return false;

    uint64 cost = sConfigMgr->GetOption<uint64>("GuildHouse.Cost", 50000000);

    if (player->GetMoney() < cost)
        return false;

    player->ModifyMoney(-(int64)cost);

    GuildHouseData data;
    data.GuildId = guildId;
    data.OwnerGuid = player->GetGUID().GetCounter();

    data.Map = sConfigMgr->GetOption<uint32>("GuildHouse.Map", 1);
    data.X = sConfigMgr->GetOption<float>("GuildHouse.X", 16222.57f);
    data.Y = sConfigMgr->GetOption<float>("GuildHouse.Y", 16265.91f);
    data.Z = sConfigMgr->GetOption<float>("GuildHouse.Z", 13.2f);
    data.O = sConfigMgr->GetOption<float>("GuildHouse.O", 0.0f);

    CharacterDatabase.PExecute("INSERT INTO guildhouse (guildId, ownerGuid, map, position_x, position_y, position_z, orientation) VALUES (%u,%u,%u,%f,%f,%f,%f)", data.GuildId, data.OwnerGuid, data.Map, data.X, data.Y, data.Z, data.O);

    _houses[data.GuildId] = data;

    LOG_INFO("module", "GuildHouse: Guild {} purchased a guild house.", guildId);

    return true;
}

bool GuildHouseMgr::Teleport(Player* player)
{
    if (!player)
        return false;

    uint32 guildId = player->GetGuildId();
    auto house = GetGuildHouse(guildId);

    if (!house)
        return false;

    player->TeleportTo(house->Map, house->X, house->Y, house->Z, house->O);

    return true;
}
