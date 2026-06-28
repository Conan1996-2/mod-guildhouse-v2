#ifndef MOD_GUILDHOUSE_MGR_H
#define MOD_GUILDHOUSE_MGR_H

#include "Common.h"

class GuildHouseMgr
{
public:
    static GuildHouseMgr* instance();

    void Load();

    bool HasHouse(uint32 guildId) const;

    bool Purchase(Player* player);

    bool Teleport(Player* player);

    uint32 GetGuildPhase(uint32 guildId) const;

private:
    GuildHouseMgr() = default;

    std::unordered_set<uint32> _guilds;
};

#define sGuildHouseMgr GuildHouseMgr::instance()

#endif
