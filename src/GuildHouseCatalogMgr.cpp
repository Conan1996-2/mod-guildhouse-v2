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
        "SELECT categoryId, parentId, name, sortOrder, enabled "
        "FROM guildhouse_category"))
    {
        do
        {
            Field* fields = result->Fetch();

            GHCategory cat;
            cat.Id        = fields[0].Get<uint32_t>();
            cat.ParentId  = fields[1].Get<uint32_t>();
            cat.Name      = fields[2].Get<std::string>();
            cat.SortOrder = fields[3].Get<uint16_t>();
            cat.Enabled   = fields[4].Get<bool>();

            _categories.emplace(cat.Id, cat);

        } while (result->NextRow());
    }

    // =====================================================
    // Load catalog entries
    // =====================================================
    if (QueryResult result = WorldDatabase.Query(
        "SELECT catalogId, categoryId, name, spawnFlags, behaviorFlags, enabled "
        "FROM guildhouse_catalog"))
    {
        do
        {
            Field* fields = result->Fetch();

            GHCatalog cat;
            cat.CatalogId     = fields[0].Get<uint32_t>();
            cat.CategoryId    = fields[1].Get<uint32_t>();
            cat.Name          = fields[2].Get<std::string>();
            cat.SpawnFlags    = static_cast<GHSpawnFlags>(fields[3].Get<uint32_t>());
            cat.BehaviorFlags = static_cast<GHBehaviorFlags>(fields[4].Get<uint32_t>());
            cat.Enabled       = fields[5].Get<bool>();

            _catalogs.emplace(cat.CatalogId, cat);

        } while (result->NextRow());
    }

    // =====================================================
    // Load catalog components
    // =====================================================
    if (QueryResult result = WorldDatabase.Query(
        "SELECT componentId, catalogId, entry, displayId, scale, "
        "spawnFlags, behaviorFlags, scriptType, scriptData, "
        "xOffset, yOffset, zOffset, oOffset, "
        "targetMap, targetX, targetY, targetZ, targetO, "
        "childCatalogId, sortOrder "
        "FROM guildhouse_catalog_asset"))
    {
        do
        {
            Field* fields = result->Fetch();

            uint32_t catalogId = fields[1].Get<uint32_t>();

            auto it = _catalogs.find(catalogId);
            if (it == _catalogs.end())
                continue;

            GHCatalogAsset comp;

            comp.ComponentId   = fields[0].Get<uint32_t>();
            comp.CatalogId     = catalogId;

            comp.Entry         = fields[2].Get<uint32_t>();
            comp.DisplayId     = fields[3].Get<uint32_t>();
            comp.Scale         = fields[4].Get<float>();

            comp.SpawnFlags    = static_cast<GHSpawnFlags>(fields[5].Get<uint32_t>());
            comp.BehaviorFlags = static_cast<GHBehaviorFlags>(fields[6].Get<uint32_t>());
            comp.ScriptType    = static_cast<GHScriptType>(fields[7].Get<uint32_t>());

            comp.ScriptData    = fields[8].Get<std::string>();

            comp.XOffset       = fields[9].Get<float>();
            comp.YOffset       = fields[10].Get<float>();
            comp.ZOffset       = fields[11].Get<float>();
            comp.OOffset       = fields[12].Get<float>();

            comp.TargetMap     = fields[13].IsNull() ? 0 : fields[13].Get<uint32_t>();
            comp.TargetX       = fields[14].IsNull() ? 0.f : fields[14].Get<float>();
            comp.TargetY       = fields[15].IsNull() ? 0.f : fields[15].Get<float>();
            comp.TargetZ       = fields[16].IsNull() ? 0.f : fields[16].Get<float>();
            comp.TargetO       = fields[17].IsNull() ? 0.f : fields[17].Get<float>();

            comp.ChildCatalogId= fields[18].IsNull() ? 0 : fields[18].Get<uint32_t>();
            comp.SortOrder     = fields[19].Get<uint16_t>();

            it->second.Components.push_back(comp);

        } while (result->NextRow());
    }

    LOG_INFO("module", "GuildHouseCatalogMgr loaded {} categories, {} catalogs",
        _categories.size(),
        _catalogs.size());
}

const GHCatalog* GuildHouseCatalogMgr::GetCatalog(uint32 id) const
{
    auto itr = _catalogs.find(id);
    if (itr == _catalogs.end())
        return nullptr;

    return &itr->second;
}
