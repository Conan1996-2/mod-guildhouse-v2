#include "GuildHouseCatalogMgr.h"

#include "DatabaseEnv.h"
#include "QueryResult.h"
#include "Log.h"

#include <algorithm>


GuildHouseCatalogMgr& GuildHouseCatalogMgr::Instance()
{
    static GuildHouseCatalogMgr instance;

    return instance;
}


// =====================================================
// Load catalog database
//
// Loads:
// - categories
// - catalog entries
// - catalog components
//
// Components are stored both:
// - inside catalog.Components
// - inside _assets lookup map
// =====================================================

void GuildHouseCatalogMgr::Load()
{
    _categories.clear();
    _catalogs.clear();
    _assets.clear();


    //
    // Categories
    //
    if (QueryResult result = WorldDatabase.Query(
        "SELECT categoryId, parentId, name, sortOrder, enabled FROM guildhouse_category"))
    {
        do
        {
            Field* fields = result->Fetch();

            GHCategory category;

            category.Id = fields[0].Get<uint32_t>();
            category.ParentId = fields[1].Get<uint32_t>();
            category.Name = fields[2].Get<std::string>();
            category.SortOrder = fields[3].Get<uint16_t>();
            category.Enabled = fields[4].Get<bool>();

            _categories.emplace(category.Id, category);

        } while (result->NextRow());
    }


    //
    // Catalog Items
    //
    if (QueryResult result = WorldDatabase.Query(
        "SELECT catalogId, categoryId, name, spawnFlags, behaviorFlags, enabled FROM guildhouse_catalog"))
    {
        do
        {
            Field* fields = result->Fetch();

            GHCatalog catalog;

            catalog.CatalogId = fields[0].Get<uint32_t>();
            catalog.CategoryId = fields[1].Get<uint32_t>();
            catalog.Name = fields[2].Get<std::string>();

            catalog.SpawnFlags =
                static_cast<GHSpawnFlags>(fields[3].Get<uint32_t>());

            catalog.BehaviorFlags =
                static_cast<GHBehaviorFlags>(fields[4].Get<uint32_t>());

            catalog.Enabled = fields[5].Get<bool>();

            _catalogs.emplace(catalog.CatalogId, catalog);

        } while (result->NextRow());
    }


    //
    // Catalog Components
    //
    if (QueryResult result = WorldDatabase.Query(
        "SELECT "
        "componentId,"
        "catalogId,"
        "spawnFlags,"
        "behaviorFlags,"
        "entry,"
        "displayId,"
        "scale,"
        "scriptType,"
        "scriptData,"
        "xOffset,"
        "yOffset,"
        "zOffset,"
        "oOffset,"
        "targetMap,"
        "targetX,"
        "targetY,"
        "targetZ,"
        "targetO,"
        "childCatalogId,"
        "sortOrder "
        "FROM guildhouse_catalog_asset "
        "ORDER BY sortOrder"))
    {
        do
        {
            Field* fields = result->Fetch();

            uint32_t catalogId = fields[1].Get<uint32_t>();

            auto catalogItr = _catalogs.find(catalogId);

            if (catalogItr == _catalogs.end())
                continue;


            GHCatalogAsset component;

            component.ComponentId = fields[0].Get<uint32_t>();

            component.CatalogId = catalogId;


            component.SpawnFlags =
                static_cast<GHSpawnFlags>(fields[2].Get<uint32_t>());

            component.BehaviorFlags =
                static_cast<GHBehaviorFlags>(fields[3].Get<uint32_t>());


            component.Entry = fields[4].Get<uint32_t>();

            component.DisplayId = fields[5].Get<uint32_t>();

            component.Scale = fields[6].Get<float>();


            component.ScriptType =
                static_cast<GHScriptType>(fields[7].Get<uint32_t>());


            component.ScriptData =
                fields[8].IsNull()
                ? ""
                : fields[8].Get<std::string>();


            component.XOffset = fields[9].Get<float>();

            component.YOffset = fields[10].Get<float>();

            component.ZOffset = fields[11].Get<float>();

            component.OOffset = fields[12].Get<float>();


            component.TargetMap =
                fields[13].IsNull()
                ? 0
                : fields[13].Get<uint32_t>();


            component.TargetX =
                fields[14].IsNull()
                ? 0.0f
                : fields[14].Get<float>();

            component.TargetY =
                fields[15].IsNull()
                ? 0.0f
                : fields[15].Get<float>();

            component.TargetZ =
                fields[16].IsNull()
                ? 0.0f
                : fields[16].Get<float>();

            component.TargetO =
                fields[17].IsNull()
                ? 0.0f
                : fields[17].Get<float>();


            component.ChildCatalogId =
                fields[18].IsNull()
                ? 0
                : fields[18].Get<uint32_t>();


            component.SortOrder =
                fields[19].Get<uint16_t>();


            //
            // Store direct lookup
            //
            _assets.emplace(component.ComponentId, component);


            //
            // Store component in parent catalog
            //
            catalogItr->second.Components.push_back(component);


        } while (result->NextRow());
    }


    LOG_INFO(
        "server.loading",
        "GuildHouseCatalogMgr loaded {} categories, {} catalogs, {} components",
        _categories.size(),
        _catalogs.size(),
        _assets.size());
}

const GHCatalog* GuildHouseCatalogMgr::GetCatalog(uint32_t catalogId) const
{
    auto itr = _catalogs.find(catalogId);

    if (itr == _catalogs.end())
        return nullptr;

    return &itr->second;
}


const GHCategory* GuildHouseCatalogMgr::GetCategory(uint32_t categoryId) const
{
    auto itr = _categories.find(categoryId);

    if (itr == _categories.end())
        return nullptr;

    return &itr->second;
}


const GHCatalogAsset* GuildHouseCatalogMgr::GetCatalogAsset(uint32_t componentId) const
{
    auto itr = _assets.find(componentId);

    if (itr == _assets.end())
        return nullptr;

    return &itr->second;
}


// =====================================================
// Root category list
// =====================================================

std::vector<const GHCategory*> GuildHouseCatalogMgr::GetRootCategories() const
{
    std::vector<const GHCategory*> result;


    for (auto const& [id, category] : _categories)
    {
        if (category.ParentId == 0 &&
            category.Enabled)
        {
            result.push_back(&category);
        }
    }


    std::sort(
        result.begin(),
        result.end(),
        [](const GHCategory* a, const GHCategory* b)
        {
            return a->SortOrder < b->SortOrder;
        });


    return result;
}


// =====================================================
// Child category list
// =====================================================

std::vector<const GHCategory*> GuildHouseCatalogMgr::GetChildCategories(uint32_t parentId) const
{
    std::vector<const GHCategory*> result;


    for (auto const& [id, category] : _categories)
    {
        if (category.ParentId == parentId &&
            category.Enabled)
        {
            result.push_back(&category);
        }
    }


    std::sort(
        result.begin(),
        result.end(),
        [](const GHCategory* a, const GHCategory* b)
        {
            return a->SortOrder < b->SortOrder;
        });


    return result;
}


// =====================================================
// Catalog list by category
// =====================================================

std::vector<const GHCatalog*> GuildHouseCatalogMgr::GetCatalogs(uint32_t categoryId) const
{
    std::vector<const GHCatalog*> result;


    for (auto const& [id, catalog] : _catalogs)
    {
        if (catalog.CategoryId == categoryId &&
            catalog.Enabled)
        {
            result.push_back(&catalog);
        }
    }


    std::sort(
        result.begin(),
        result.end(),
        [](const GHCatalog* a, const GHCatalog* b)
        {
            return a->Name < b->Name;
        });


    return result;
}


// =====================================================
// All catalogs
// =====================================================

std::vector<const GHCatalog*> GuildHouseCatalogMgr::GetAllCatalogs() const
{
    std::vector<const GHCatalog*> result;


    for (auto const& [id, catalog] : _catalogs)
    {
        if (catalog.Enabled)
            result.push_back(&catalog);
    }


    std::sort(
        result.begin(),
        result.end(),
        [](const GHCatalog* a, const GHCatalog* b)
        {
            return a->Name < b->Name;
        });


    return result;
}
