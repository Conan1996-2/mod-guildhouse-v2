#include "GuildHouseCatalogMgr.h"

#include "DatabaseEnv.h"
#include "QueryResult.h"
#include "Log.h"

GuildHouseCatalogMgr& GuildHouseCatalogMgr::Instance()
{
    static GuildHouseCatalogMgr instance;
    return instance;
}

void GuildHouseCatalogMgr::Load()
{
    _catalogs.clear();
    _categories.clear();

    // =====================================================
    // Load categories
    // =====================================================
    if (QueryResult result = WorldDatabase.Query(
        "SELECT categoryId, parentId, name, sortOrder FROM guildhouse_category"))
    {
        do
        {
            Field* fields = result->Fetch();

            GHCategory cat;
            cat.Id        = fields[0].Get<uint32_t>();
            cat.ParentId  = fields[1].Get<uint32_t>();
            cat.Name      = fields[2].Get<std::string>();
            cat.SortOrder = fields[3].Get<uint16_t>();

            _categories.emplace(cat.Id, cat);

        } while (result->NextRow());
    }

    // =====================================================
    // Load catalog entries
    // =====================================================
    if (QueryResult result = WorldDatabase.Query(
        "SELECT catalogId, categoryId, name FROM guildhouse_catalog"))
    {
        do
        {
            Field* fields = result->Fetch();

            GHCatalog cat;
            cat.CatalogId  = fields[0].Get<uint32_t>();
            cat.CategoryId = fields[1].Get<uint32_t>();
            cat.Name       = fields[2].Get<std::string>();

            _catalogs.emplace(cat.CatalogId, cat);

        } while (result->NextRow());
    }

    // =====================================================
    // Load components
    // =====================================================
    if (QueryResult result = WorldDatabase.Query(
        "SELECT componentId, catalogId, entry, displayId, scale, "
        "spawnFlags, behaviorFlags, scriptType, "
        "x, y, z, o "
        "FROM guildhouse_catalog_asset"))
    {
        do
        {
            Field* fields = result->Fetch();

            uint32_t catalogId = fields[1].Get<uint32_t>();

            auto it = _catalogs.find(catalogId);
            if (it == _catalogs.end())
                continue;

            GHComponent comp;
            comp.ComponentId  = fields[0].Get<uint32_t>();
            comp.CatalogId    = catalogId;
            comp.Entry        = fields[2].Get<uint32_t>();
            comp.DisplayId    = fields[3].Get<uint32_t>();
            comp.Scale        = fields[4].Get<float>();
            comp.SpawnFlags   = static_cast<GHSpawnFlags>(fields[5].Get<uint32_t>());
            comp.BehaviorFlags= static_cast<GHBehaviorFlags>(fields[6].Get<uint32_t>());
            comp.ScriptType   = static_cast<GHScriptType>(fields[7].Get<uint32_t>());
            comp.X            = fields[8].Get<float>();
            comp.Y            = fields[9].Get<float>();
            comp.Z            = fields[10].Get<float>();
            comp.O            = fields[11].Get<float>();

            it->second.Components.push_back(comp);

        } while (result->NextRow());
    }

    LOG_INFO("module", "GuildHouseCatalogMgr loaded {} catalogs", _catalogs.size());
}

const GHCatalog* GuildHouseCatalogMgr::GetCatalog(uint32 id) const
{
    auto itr = _catalogs.find(id);
    if (itr == _catalogs.end())
        return nullptr;

    return &itr->second;
}
