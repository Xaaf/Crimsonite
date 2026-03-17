add_executable(game_launcher
    src/launcher.cpp
)

target_include_directories(game_launcher PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Output binary should be named "crimsonite" (crimsonite.exe on Windows)
set_target_properties(game_launcher PROPERTIES OUTPUT_NAME "crimsonite")