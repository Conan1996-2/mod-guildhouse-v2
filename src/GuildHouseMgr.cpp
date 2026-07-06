#include "GuildHouseMgr.h"

#include "DatabaseEnv.h"
#include "QueryResult.h"
#include "GuildHouseConfig.h"
#include "GuildHouseDefines.h"
#include "Log.h"
#include "GuildHouseCatalogMgr.h"
#include "GuildHouseSpawner.h"
#include "GuildHouseInstanceMgr.h"
#include "Player.h"
#include "Chat.h"

GuildHouseMgr& GuildHouseMgr::Instance()
{
    static GuildHouseMgr instance;
    return instance;
}

void GuildHouseMgr::Load()
{
    _houses.clear();

    // =====================================================
    // Load guild houses
    // =====================================================
    if (QueryResult result = CharacterDatabase.Query(
        "SELECT guildId, ownerGuid FROM guildhouse"))
    {
        do
        {
            Field* fields = result->Fetch();

            uint32_t guildId  = fields[0].Get<uint32_t>();
            uint32_t ownerGuid = fields[1].Get<uint32_t>();

            GHGuildHouse house;
            house.GuildId = guildId;
            house.OwnerGuid = ownerGuid;
            house.Phase = GetPhase(guildId);

            _houses.emplace(guildId, house);

        } while (result->NextRow());
    }

    // =====================================================
    // Load placed assets
    // =====================================================
    if (QueryResult result = CharacterDatabase.Query(
        "SELECT assetId, guildId, catalogId, phase, x, y, z, o "
        "FROM guildhouse_asset"))
    {
        do
        {
            Field* fields = result->Fetch();

            uint32_t guildId = fields[1].Get<uint32_t>();

            auto it = _houses.find(guildId);
            if (it == _houses.end())
                continue;

            GHGuildAsset asset;
            asset.AssetId   = fields[0].Get<uint32_t>();
            asset.GuildId   = guildId;
            asset.CatalogId = fields[2].Get<uint32_t>();
            asset.Phase     = fields[3].Get<uint32_t>();
            asset.X         = fields[4].Get<float>();
            asset.Y         = fields[5].Get<float>();
            asset.Z         = fields[6].Get<float>();
            asset.O         = fields[7].Get<float>();

            it->second.Assets.push_back(asset);

        } while (result->NextRow());
    }

    LOG_INFO("module", "GuildHouseMgr loaded {} guild houses", _houses.size());
}

bool GuildHouseMgr::HasGuildHouse(uint32_t guildId) const
{
    return _houses.find(guildId) != _houses.end();
}

uint32_t GuildHouseMgr::GetPhase(uint32_t guildId) const
{
    return guildId + GH_PHASE_OFFSET;
}

const GHGuildHouse* GuildHouseMgr::GetGuildHouse(uint32_t guildId) const
{
    auto it = _houses.find(guildId);
    return (it != _houses.end()) ? &it->second : nullptr;
}

bool GuildHouseMgr::CreateGuildHouse(uint32_t guildId, uint32_t ownerGuid)
{
    if (HasGuildHouse(guildId))
        return false;

    GHGuildHouse house;
    house.GuildId = guildId;
    house.OwnerGuid = ownerGuid;
    house.Phase = GetPhase(guildId);

    _houses.emplace(guildId, house);

    std::ostringstream ss;
    
    ss << "INSERT INTO guildhouse (guildId, ownerGuid) VALUES ("
       << guildId << ", "
       << ownerGuid << ")";
    
    CharacterDatabase.Execute(ss.str());

    return true;
}

bool GuildHouseMgr::PurchaseCatalogItem(Player* player, uint32_t catalogId)
{
    if (!player)
        return false;

    uint32 guildId = player->GetGuildId();
    if (!guildId)
    {
        ChatHandler(player->GetSession()).PSendSysMessage("You are not in a guild.");
        return false;
    }

    const GHCatalog* catalog = sGuildHouseCatalogMgr.GetCatalog(catalogId);
    if (!catalog || !catalog->Enabled)
    {
        ChatHandler(player->GetSession()).PSendSysMessage("Invalid catalog item.");
        return false;
    }

    float x = player->GetPositionX();
    float y = player->GetPositionY();
    float z = player->GetPositionZ();
    float o = player->GetOrientation();

    uint32 phase = GetPhase(guildId);

    std::ostringstream ss;
    ss << "INSERT INTO guildhouse_asset "
          "(guildId, layoutId, catalogId, status, phase, "
          "positionX, positionY, positionZ, orientation, createdBy) VALUES ("
       << guildId << ","
       << 1 << ","
       << catalogId << ","
       << 0 << ","
       << phase << ","
       << x << "," << y << "," << z << "," << o << ","
       << player->GetGUID().GetCounter()
       << ")";

    CharacterDatabase.Execute(ss.str());

    uint32 assetId = CharacterDatabase.GetLastInsertId();

    for (auto const& comp : catalog->Components)
    {
        GHInstance inst;

        inst.AssetId   = assetId;
        inst.CatalogId = catalogId;
        inst.GuildId   = guildId;
        inst.Phase     = phase;
        inst.Type      = 0;

        inst.X = x + comp.XOffset;
        inst.Y = y + comp.YOffset;
        inst.Z = z + comp.ZOffset;
        inst.O = o + comp.OOffset;

        uint32 guid = sGuildHouseSpawner.Spawn(comp, inst);
        inst.Guid = guid;

        std::ostringstream ins;
        ins << "INSERT INTO guildhouse_instance "
               "(guildId, assetId, catalogId, guid, type, mapId, phase, x, y, z, o) VALUES ("
            << guildId << ","
            << assetId << ","
            << catalogId << ","
            << guid << ","
            << 0 << ","
            << player->GetMapId() << ","
            << phase << ","
            << inst.X << ","
            << inst.Y << ","
            << inst.Z << ","
            << inst.O << ")";

        CharacterDatabase.Execute(ins.str());
    }
    ChatHandler(player->GetSession()).PSendSysMessage("Guild House item purchased.");
    return true;
}
