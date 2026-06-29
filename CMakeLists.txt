# Guild House Module

AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/GuildHouseModule.cpp")
AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/GuildHouseConfig.cpp")
AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/GuildHouseMgr.cpp")
AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/GuildHouseSpawner.cpp")
AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/GuildHouseBrokerNpc.cpp")
AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/GuildHouseBuilderNpc.cpp")

AC_ADD_CONFIG_FILE("${CMAKE_CURRENT_LIST_DIR}/conf/guildhouse.conf.dist")
