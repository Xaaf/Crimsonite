#include "hook_manager.h"
#include "lua_utils.h"
#include "log.h"

#include "MinHook.h"

#include <windows.h>

luaL_loadbuffer_t g_originalLoadbuffer = nullptr;
lua_pcall_t g_originalPcall = nullptr;

int __cdecl hooked_loadbuffer(void* luaState, const char* buff, size_t size, const char* name) {
    if (name) {
        LOG_TRACE("luaL_loadbuffer called with chunk name: {}", name);
    }

    inject_lua(luaState);

    return g_originalLoadbuffer(luaState, buff, size, name);
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

    void* loadbufferAddr = GetProcAddress(luaModule, "luaL_loadbuffer");
    if (!loadbufferAddr) {
        LOG_ERROR("Failed to get address for luaL_loadbuffer.");
        return false;
    }

    void* pcallAddr = GetProcAddress(luaModule, "lua_pcall");
    if (!pcallAddr) {
        LOG_ERROR("Failed to get address for lua_pcall.");
        return false;
    }
    g_originalPcall = reinterpret_cast<lua_pcall_t>(pcallAddr);

    if (MH_CreateHook(loadbufferAddr, &hooked_loadbuffer, reinterpret_cast<LPVOID*>(&g_originalLoadbuffer)) != MH_OK) {
        LOG_ERROR("Failed to create hook for luaL_loadbuffer.");
        return false;
    }

    if (MH_EnableHook(loadbufferAddr) != MH_OK) {
        LOG_ERROR("Failed to enable hook for luaL_loadbuffer.");
        return false;
    }

    LOG_INFO("Hook for luaL_loadbuffer set up successfully at {:p}!", loadbufferAddr);
    return true;
}

void shutdown_hook_manager() {
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
}
