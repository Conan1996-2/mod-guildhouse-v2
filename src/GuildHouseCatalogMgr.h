#ifndef MOD_GUILDHOUSE_CATALOG_MGR_H
#define MOD_GUILDHOUSE_CATALOG_MGR_H

#include <unordered_map>
#include <vector>

#include "GuildHouseTypes.h"

class GuildHouseCatalogMgr
{
public:

    static GuildHouseCatalogMgr& Instance();

    void Load();

    // =====================================================
    // Direct lookups
    // =====================================================
    const GHCatalog* GetCatalog(uint32_t catalogId) const;
    const GHCategory* GetCategory(uint32_t categoryId) const;
    const GHCatalogAsset* GetCatalogAsset(uint32_t componentId) const;

    // =====================================================
    // Salesman browsing
    // =====================================================
    std::vector<const GHCategory*> GetRootCategories() const;
    std::vector<const GHCategory*> GetChildCategories(uint32_t parentId) const;
    std::vector<const GHCatalog*> GetCatalogs(uint32_t categoryId) const;
    std::vector<const GHCatalog*> GetAllCatalogs() const;

private:

    GuildHouseCatalogMgr() = default;

    // =====================================================
    // Catalog data
    // =====================================================
    std::unordered_map<uint32_t, GHCategory> _categories;
    std::unordered_map<uint32_t, GHCatalog> _catalogs;
    std::unordered_map<uint32_t, GHCatalogAsset> _assets;
};

#define sGuildHouseCatalogMgr GuildHouseCatalogMgr::Instance()

#endif
