add_library(game_modloader SHARED
    src/modloader.cpp
)

target_link_libraries(game_modloader PRIVATE
    minhook # Link against MinHook
)

target_include_directories(game_modloader PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include

    external/minhook/include # MinHook lib
)

# Output library should be named "crimsonite" (crimsonite.dll on Windows)
set_target_properties(game_modloader PROPERTIES OUTPUT_NAME "crimsonite")