#ifndef MOD_GUILDHOUSE_INSTANCE_MGR_H
#define MOD_GUILDHOUSE_INSTANCE_MGR_H

#include <cstdint>

/*
    DEPRECATED

    GuildHouse no longer uses runtime instances.

    Permanent objects are stored in:

        guildhouse_asset

    and loaded by:

        GuildHouseSpawner

    This class remains temporarily so older code does not break.
*/


struct GHInstanceRecord
{
    uint32_t GuildId = 0;
    uint32_t AssetId = 0;
    uint32_t CatalogId = 0;

    uint32_t Guid = 0;

    uint8_t Type = 0;

    uint32_t MapId = 0;
    uint32_t Phase = 0;

    float X = 0;
    float Y = 0;
    float Z = 0;
    float O = 0;
};



class GuildHouseInstanceMgr
{

public:

    static GuildHouseInstanceMgr& Instance();


    /*
        Deprecated compatibility functions
    */

    void Load();

    void LoadInstances();


    void Save(
        const GHInstanceRecord& record);


    void AddInstance(
        const GHInstanceRecord& record);


    void RemoveGuild(
        uint32_t guildId);



private:

    GuildHouseInstanceMgr() = default;

};



#define sGuildHouseInstanceMgr GuildHouseInstanceMgr::Instance()


#endif
