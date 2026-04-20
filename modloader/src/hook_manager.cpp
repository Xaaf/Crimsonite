#include "hook_manager.h"
#include "lua_utils.h"
#include "log.h"
#include "modloader.h"

#include "MinHook.h"

#include <windows.h>

extern "C" {
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

/**
 * Lua state for the modloader. This allows us to "sandbox" the mods to some extent, as they'll run
 * on their own Lua state instead of the game's main one. This also helps us prevent crashes, and gives
 * us more fine-grained control over how mods are loaded, what they can access, etc. This will also allow
 * for more control for modders in a user-friendly way!
 */
lua_State* g_modState = nullptr;

luaL_loadbuffer_t g_originalLoadbuffer = nullptr;
lua_pcall_t g_originalPcall = nullptr;

void load_all_mods(void* luaState) {
    std::string folder = "mods";
    auto modFiles = list_mod_folders(folder);

    g_modState = luaL_newstate();
    if (!g_modState) {
        LOG_ERROR("Failed to create new Lua state for mods! Mods will not be loaded.");
        return;
    }
    luaL_openlibs(g_modState);

    // TODO: Extract this to a helper function
    lua_pushcfunction(g_modState, [](lua_State* L) -> int {
        const char* msg = lua_tostring(L, 1);
        if (msg) {
            LOG_INFO("[Mod]: {}", msg);
        }
        return 0;
    });
    lua_setglobal(g_modState, "log");

    for (const auto& mod : modFiles) {
        std::string modLua = mod + "\\mod.lua";
        std::string initLua = mod + "\\init.lua";

        LOG_TRACE("=== Loading Mod: {} ===", mod);
    
        // Change the current directory, to allow for relative pathing in the mod
        char originalDir[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, originalDir);
        SetCurrentDirectoryA(mod.c_str());

        if (luaL_loadfile(g_modState, initLua.c_str()) == 0) {
            if (lua_pcall(g_modState, 0, 0, 0) != 0) {
                const char* error = lua_tostring(g_modState, -1);
                LOG_ERROR("Error running init.lua for mod {}: {}", mod, error ? error : "Unknown error");
                lua_pop(g_modState, 1);
            }
        } else {
            const char* error = lua_tostring(g_modState, -1);
            LOG_ERROR("Error loading init.lua for mod {}: {}", mod, error ? error : "Unknown error");
            lua_pop(g_modState, 1);
        }

        // Reset the current directory to prevent issues with the next mod
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

    static bool modsLoaded = false;
    if (!modsLoaded) {
        load_all_mods(luaState);
        modsLoaded = true;
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

    if (g_modState) {
        lua_close(g_modState);
        g_modState = nullptr;
    }
}
