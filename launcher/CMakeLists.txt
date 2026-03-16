add_executable(game_launcher
    src/launcher.cpp
)

# Output binary should be named "crimsonite" (crimsonite.exe on Windows)
set_target_properties(game_launcher PROPERTIES OUTPUT_NAME "crimsonite")