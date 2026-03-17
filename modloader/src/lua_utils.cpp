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
}

void run_lua_file(void* luaState, const char* filePath) {
    // TODO: might be a good idea to add some form of sandboxing?
    // TODO: this implementation is quite barebones, chunking might be a good idea for larger files

    FILE* file = fopen(filePath, "r");
    if (!file) {
        LOG_ERROR("Failed to open Lua file: {}", filePath);
        LOG_ERROR("> Reason: {}", strerror(errno));
        return;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    std::string buffer(fileSize, '\0');
    fread(buffer.data(), 1, fileSize, file);
    fclose(file);

    run_lua(luaState, buffer.c_str());
}

void inject_lua(void* luaState) {
    static bool injected = false;
    if (injected) {
        return;
    }
    injected = true;

    LOG_INFO("Injecting Crimsonite into the Lua environment!");
}
