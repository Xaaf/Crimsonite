cmake_minimum_required(VERSION 3.10)

project(Crimsonite)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add submodules
add_subdirectory(external/minhook)

# Add shared utilities
add_subdirectory(common)

# Add our own modules
add_subdirectory(launcher)
add_subdirectory(modloader)