#include "lua_utils.h"
#include "hook_manager.h"
#include "log.h"

#include <cstring>

void run_lua(void* luaState, const char* code) {
    if (!g_originalLoadbuffer) {
        LOG_ERROR("luaL_loadbuffer function pointer is null!");
        return;
    }

    if (!g_originalPcall) {
        LOG_ERROR("lua_pcall function pointer is null!");
        return;
    }

    int loadStatus = g_originalLoadbuffer(luaState, code, strlen(code), "crimsonite_injected_chunk");
    if (loadStatus != 0) {
        LOG_ERROR("Failed to load Lua code. Status: {}", loadStatus);
        return;
    }

    int pcallStatus = g_originalPcall(luaState, 0, 0, 0);
    if (pcallStatus != 0) {
        LOG_ERROR("Failed to execute Lua code. Status: {}", pcallStatus);
        return;
    }

    LOG_INFO("Lua code executed successfully!");
}

void inject_lua(void* luaState) {
    static bool injected = false;
    if (injected) {
        return;
    }
    injected = true;

    LOG_INFO("Injecting Crimsonite into the Lua environment!");
    run_lua(luaState, "print('Hello from Crimsonite!')");
}
