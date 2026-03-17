#include "console.h"
#include "hook_manager.h"
#include "log.h"

#include <windows.h>
#include <string>
#include <vector>

/**
 * Check whether the file at `path` exists.
 */
bool file_exists(const std::string& path) {
    DWORD attribs = GetFileAttributesA(path.c_str());
    return (attribs != INVALID_FILE_ATTRIBUTES && !(attribs & FILE_ATTRIBUTE_DIRECTORY));
}

/**
 * List all valid mod folders in the given `folder` path. A valid mod folder contains
 * - `mod.lua` with metadata about the mod
 * - `init.lua` with the mod's entry point
 * 
 * Any mods that do not meet the validity criteria are skipped, though a warning is
 * logged for each invalid mod folder, showing which file is missing.
 */
std::vector<std::string> list_mod_folders(const std::string& folder) {
    std::vector<std::string> modFolders;
    std::string searchPath = folder + "\\*";

    char fullPath[MAX_PATH];
    GetFullPathNameA(folder.c_str(), MAX_PATH, fullPath, nullptr);

    std::string basePath(fullPath);
    LOG_DEBUG("Scanning for mod folders in folder: {}", basePath);

    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        LOG_ERROR("Failed to list mod folders in folder: {}", basePath);
        return modFolders;
    }

    do {
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            std::string folderName = findData.cFileName;

            if (folderName != "." && folderName != "..") {
                std::string modPath = std::string(basePath) + "\\" + folderName;
                modFolders.push_back(modPath);
            }
        }
    } while (FindNextFileA(hFind, &findData) != 0);

    FindClose(hFind);

    std::vector<std::string> validMods;
    for (const auto& modFolder : modFolders) {
        std::string modLua = modFolder + "\\mod.lua";
        std::string initLua = modFolder + "\\init.lua";

        LOG_TRACE("Checking mod: {}", modFolder);
        if (!file_exists(modLua)) {
            LOG_WARN("Mod folder '{}' is missing 'mod.lua', skipping...", modFolder);
            continue;
        }

        if (!file_exists(initLua)) {
            LOG_WARN("Mod folder '{}' is missing 'init.lua', skipping...", modFolder);
            continue;
        }

        LOG_TRACE("Mod folder '{}' is valid!", modFolder);
        validMods.push_back(modFolder);
    }

    
    LOG_INFO("Found mod {} folders: ", validMods.size());
    for(auto& folder : validMods) {
        LOG_INFO("- {}", folder);
    }

    return validMods;
}

/**
 * Main thread func for the modloader. It's done on a new thread to avoid blocking the game's main thread.
 */
DWORD WINAPI MainThread(LPVOID param) {
    setup_console();

    if (!initialise_hook_manager()) {
        return 1;
    }

    auto modFiles = list_mod_folders("mods");

    return 0;
}

/**
 * @brief Entry point for the DLL. Creates a new thread to run our main logic in.
 */
BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        CreateThread(nullptr, 0, MainThread, nullptr, 0, nullptr);
    }

    return TRUE;
}
