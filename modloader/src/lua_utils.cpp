#include "lua_utils.h"
#include "hook_manager.h"
#include "log.h"

#include <cstring>

void run_lua(void* luaState, const char* code) {
    if (!g_originalLoadbuffer) {
        LOG_ERROR("luaL_loadbuffer function pointer is null!");
        throw std::runtime_error("luaL_loadbuffer function pointer is null");
    }

    if (!g_originalPcall) {
        LOG_ERROR("lua_pcall function pointer is null!");
        throw std::runtime_error("lua_pcall function pointer is null");
    }

    int loadStatus = g_originalLoadbuffer(luaState, code, strlen(code), "crimsonite_injected_chunk");
    if (loadStatus != 0) {
        LOG_ERROR("Failed to load Lua code. Status: {}", loadStatus);
        throw std::runtime_error("Failed to load Lua code");
    }

    int pcallStatus = g_originalPcall(luaState, 0, 0, 0);
    if (pcallStatus != 0) {
        LOG_ERROR("Failed to execute Lua code. Status: {}", pcallStatus);
        throw std::runtime_error("Failed to execute Lua code");
    }
}

void run_lua_file(void* luaState, const char* filePath) {
    // TODO: might be a good idea to add some form of sandboxing?
    // TODO: this implementation is quite barebones, chunking might be a good idea for larger files

    FILE* file = fopen(filePath, "r");
    if (!file) {
        LOG_ERROR("Failed to open Lua file: {}", filePath);
        LOG_ERROR("> Reason: {}", strerror(errno));
        throw std::runtime_error("Failed to open Lua file");
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    std::string buffer(fileSize, '\0');
    fread(buffer.data(), 1, fileSize, file);
    fclose(file);

    if (buffer.empty()) {
        LOG_ERROR("Lua file is empty: {}", filePath);
        throw std::runtime_error("Lua file is empty");
    }

    try {
        run_lua(luaState, buffer.c_str());
    } catch (const std::exception& e) {
        LOG_ERROR("Error occurred while running Lua file: {}", e.what());
        throw;
    }
}

void inject_lua(void* luaState) {
    static bool injected = false;
    if (injected) {
        return;
    }
    injected = true;

    LOG_INFO("Injecting Crimsonite into the Lua environment!");
}
