#ifndef MOD_GUILDHOUSE_MGR_H
#define MOD_GUILDHOUSE_MGR_H

#include "Common.h"

struct GuildHouseData
{
    uint32 GuildId;
    uint32 OwnerGuid;
    uint32 Map;
    float X;
    float Y;
    float Z;
    float O;
};

class GuildHouseMgr
{
  public:
      static GuildHouseMgr* instance();
      void Load();
      bool HasGuildHouse(uint32 guildId) const;
      const GuildHouseData* GetGuildHouse(uint32 guildId) const;
      bool Purchase(Player* player);
      bool Teleport(Player* player);
  
  private:
      GuildHouseMgr() = default;
      std::unordered_map<uint32, GuildHouseData> _houses;
};

#define sGuildHouseMgr GuildHouseMgr::instance()

#endif
