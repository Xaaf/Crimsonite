add_library(game_modloader SHARED
    src/modloader.cpp
)

target_include_directories(game_modloader PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Output library should be named "crimsonite" (crimsonite.dll on Windows)
set_target_properties(game_modloader PROPERTIES OUTPUT_NAME "crimsonite")