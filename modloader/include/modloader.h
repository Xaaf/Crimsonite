#pragma once

#include <windows.h>

#include <string>
#include <vector>

/**
 * Check whether the file at `path` exists.
 */
bool file_exists(const std::string& path);

/**
 * List all valid mod folders in the given `folder` path. A valid mod folder contains
 * - `mod.lua` with metadata about the mod
 * - `init.lua` with the mod's entry point
 * 
 * Any mods that do not meet the validity criteria are skipped, though a warning is
 * logged for each invalid mod folder, showing which file is missing.
 */
std::vector<std::string> list_mod_folders(const std::string& folder);