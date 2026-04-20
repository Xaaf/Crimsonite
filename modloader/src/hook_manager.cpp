#include "hook_manager.h"
#include "lua_utils.h"
#include "log.h"
#include "modloader.h"

#include "MinHook.h"

#include <windows.h>

luaL_loadbuffer_t g_originalLoadbuffer = nullptr;
lua_pcall_t g_originalPcall = nullptr;

// TODO: Find a better way of handling the loading of mods. On occasion, this still likes to crash
//          the game for whatever reason. Sometimes the mod doesn't load at all, sometimes it's skipped.
void load_all_mods(void* luaState) {
    std::string folder = "mods";
    auto modFiles = list_mod_folders(folder);
    for (const auto& mod : modFiles) {
        std::string modLua = mod + "\\mod.lua";
        std::string initLua = mod + "\\init.lua";

        LOG_TRACE("=== Loading Mod: {} ===", mod);
    
        char originalDir[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, originalDir);

        SetCurrentDirectoryA(mod.c_str());

        try {
            run_lua_file(luaState, modLua.c_str());
            run_lua_file(luaState, initLua.c_str());
        } catch (const std::exception& e) {
            LOG_ERROR("Error occurred while loading mod: {}", e.what());
        }


        SetCurrentDirectoryA(originalDir);
    }

    LOG_TRACE("=== Done Loading Mods ===");
}

int __cdecl hooked_loadbuffer(void* luaState, const char* buff, size_t size, const char* name) {
    static bool inHook = false;
    if (inHook) {
        return g_originalLoadbuffer(luaState, buff, size, name);
    }

    inHook = true;

    LOG_DEBUG(">>> {}", name ? name : "null");
    LOG_DEBUG(">>>      Buffer: {}, Size: {}", buff ? buff : "null", size);

    int result = g_originalLoadbuffer(luaState, buff, size, name);

    // Only load mods after `main.lu` to prevent the game from crashing
    if (name && strcmp(name, "main.lu") == 0) {
        shouldLoadMods = true;
    }

    if (shouldLoadMods && !modsLoaded) {
        modsLoaded = true;
        shouldLoadMods = false;

        load_all_mods(luaState);
    }

    inHook = false;
    return result;
}

bool initialise_hook_manager() {
    if (MH_Initialize() != MH_OK) {
        LOG_ERROR("Failed to initialize MinHook.");
        return false;
    }

    uintptr_t luaBase = 0;
    while (!luaBase) {
        luaBase = (uintptr_t)GetModuleHandleA("lua.dll");
        Sleep(100);
    }

    HMODULE luaModule = GetModuleHandleA("lua.dll");
    if (!luaModule) {
        LOG_ERROR("Failed to get handle for lua.dll.");
        return false;
    }

    //
    //  Hook for `luaL_loadbuffer`
    //
    void* loadbufferAddr = GetProcAddress(luaModule, "luaL_loadbuffer");
    if (!loadbufferAddr) {
        LOG_ERROR("Failed to get address for luaL_loadbuffer.");
        return false;
    }

    if (MH_CreateHook(loadbufferAddr, &hooked_loadbuffer, reinterpret_cast<LPVOID*>(&g_originalLoadbuffer)) != MH_OK) {
        LOG_ERROR("Failed to create hook for luaL_loadbuffer.");
        return false;
    }

    if (MH_EnableHook(loadbufferAddr) != MH_OK) {
        LOG_ERROR("Failed to enable hook for luaL_loadbuffer.");
        return false;
    }

    LOG_INFO("Hook for luaL_loadbuffer set up successfully at {:p}!", loadbufferAddr);

    //
    //  Hook for `lua_pcall`
    //
    void* pcallAddr = GetProcAddress(luaModule, "lua_pcall");
    if (!pcallAddr) {
        LOG_ERROR("Failed to get address for lua_pcall.");
        return false;
    }
    g_originalPcall = reinterpret_cast<lua_pcall_t>(pcallAddr);

    return true;
}

void shutdown_hook_manager() {
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
}
