AC_ADD_SCRIPT_LOADER("GuildHouse" "${CMAKE_CURRENT_LIST_DIR}/src/loader.h")

AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/GuildHouseNpc.cpp")
AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/GuildHouseMgr.cpp")
AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/GuildHouseSpawner.cpp")
AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/GuildHouseConfig.cpp")
AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/GuildHouseCatalogMgr.cpp")
AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/GuildHouseInstanceMgr.cpp")
AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/GuildHouseCommands.cpp")

AC_ADD_CONFIG_FILE("${CMAKE_CURRENT_LIST_DIR}/conf/guildhouse.conf.dist")
