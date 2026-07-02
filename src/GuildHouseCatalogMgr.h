#ifndef MOD_GUILDHOUSE_CATALOG_MGR_H
#define MOD_GUILDHOUSE_CATALOG_MGR_H

#include <unordered_map>

#include "GuildHouseTypes.h"

class GuildHouseCatalogMgr
{
public:
    static GuildHouseCatalogMgr& Instance();

    void Load();

    const GHCatalog* GetCatalog(uint32_t catalogId) const;
    const std::vector<GHCatalog>& GetAllCatalogs() const;

private:
    GuildHouseCatalogMgr() = default;

    std::unordered_map<uint32_t, GHCatalog> _catalogs;
    std::unordered_map<uint32_t, GHCategory> _categories;
};

#define sGuildHouseCatalogMgr GuildHouseCatalogMgr::Instance()

#endif
