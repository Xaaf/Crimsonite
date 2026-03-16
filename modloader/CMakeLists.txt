add_library(game_modloader SHARED
    src/modloader.cpp
)

# Output library should be named "crimsonite" (crimsonite.dll on Windows)
set_target_properties(game_modloader PROPERTIES OUTPUT_NAME "crimsonite")