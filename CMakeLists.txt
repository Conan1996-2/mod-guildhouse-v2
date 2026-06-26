define_property(GLOBAL PROPERTY AC_MODULE_LIST BRIEF_DOCS "AzerothCore Module List" FULL_DOCS "List of AzerothCore modules")

set(mod_guildhouse_v2_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/guildhouse_v2_loader.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/guildhouse_v2_script.cpp"
)

message(STATUS "  -> Prepared: mod-guildhouse-v2")

set(mod_guildhouse_v2_INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_library(mod-guildhouse-v2 OBJECT ${mod_guildhouse_v2_SOURCES})

target_include_directories(mod-guildhouse-v2 PRIVATE ${mod_guildhouse_v2_INCLUDES})
target_compile_definitions(mod-guildhouse-v2 PRIVATE -DMODULE_NAME="mod-guildhouse-v2")
set_property(GLOBAL APPEND PROPERTY AC_MODULE_LIST "mod-guildhouse-v2")
