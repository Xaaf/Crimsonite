#include <cstring>
#include <windows.h>
#include <iostream>

#include "MinHook.h"
#include "log.h"

void run_lua(void* luaState, const char* code);

#pragma region Function Hooks
typedef int(__cdecl* luaL_loadbuffer_t)(void* luaState, const char* buff, size_t size, const char* name);
typedef int(__cdecl* lua_pcall_t)(void* luaState, int nargs, int nresults, int errfunc);

luaL_loadbuffer_t originalLoadbuffer = nullptr;
lua_pcall_t originalPcall = nullptr;

int __cdecl hookedLoadBuffer(void* luaState, const char* buff, size_t size, const char* name) {
    if (name) {
        LOG_TRACE("luaL_loadbuffer called with chunk name: {}", name);
    }

    static bool injected = false;
    if (!injected) {
        injected = true;

        LOG_INFO("Injecting Crismonite into the Lua environment!");
        run_lua(luaState, "print('Hello from Crimsonite!')");
    }

    return originalLoadbuffer(luaState, buff, size, name);
}
#pragma endregion

void run_lua(void* luaState, const char* code) {
    if (!originalLoadbuffer) {
        LOG_ERROR("luaL_loadbuffer function pointer is null!");
        return;
    }
    
    if (!originalPcall) {
        LOG_ERROR("lua_pcall function pointer is null!");
        return;
    }

    // Load the Lua code as a chunk
    int loadStatus = originalLoadbuffer(luaState, code, strlen(code), "crimsonite_injected_chunk");
    if (loadStatus != 0) {
        LOG_ERROR("Failed to load Lua code. Status: {}", loadStatus);
        return;
    }

    // Call the loaded chunk
    int pcallStatus = originalPcall(luaState, 0, 0, 0);
    if (pcallStatus != 0) {
        LOG_ERROR("Failed to execute Lua code. Status: {}", pcallStatus);
        return;
    }

    LOG_INFO("Lua code executed successfully!");
}

#pragma region DLL Thread
/**
 * Main thread func for the modloader. It's done on a new thread to avoid blocking the game's main thread.
 */
DWORD WINAPI MainThread(LPVOID param) {
    #pragma region Logging Setup
    //
    //      Set up the console for logging
    //
    AllocConsole();

    // Avoid C4996 warnings on MSVC by using `freopen_s` instead of `freopen`
    // We also want to redirect *all* standard streams to accomodate the logger
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    freopen_s(&f, "CONOUT$", "w", stderr);
    freopen_s(&f, "CONIN$", "r", stdin);

    std::ios::sync_with_stdio(true);

    // Make sure the log colors work nicely
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        }
    }

    LOG_INFO("Crimsonite modloader loaded successfully!");
    #pragma endregion

    #pragma region Hook Setup
    //
    //      Initialise MinHook and set up our hook
    //
    if (MH_Initialize() != MH_OK) {
        LOG_ERROR("Failed to initialize MinHook.");
        return 1;
    }

    uintptr_t luaBase = 0;
    while (!luaBase) {
        luaBase = (uintptr_t)GetModuleHandleA("lua.dll");
        Sleep(100);
    }

    HMODULE luaModule = GetModuleHandleA("lua.dll");
    if (!luaModule) {
        LOG_ERROR("Failed to get handle for lua.dll.");
        return 1;
    }

    void* loadbufferAddr = GetProcAddress(luaModule, "luaL_loadbuffer");
    if (!loadbufferAddr) {
        LOG_ERROR("Failed to get address for luaL_loadbuffer.");
        return 1;
    }

    void* pcallAddr = GetProcAddress(luaModule, "lua_pcall");
    if (!pcallAddr) {
        LOG_ERROR("Failed to get address for lua_pcall.");
        return 1;
    }
    originalPcall = reinterpret_cast<lua_pcall_t>(pcallAddr);

    // Set up the hook
    if (MH_CreateHook(loadbufferAddr, &hookedLoadBuffer, reinterpret_cast<LPVOID*>(&originalLoadbuffer)) != MH_OK) {
        LOG_ERROR("Failed to create hook for luaL_loadbuffer.");
        return 1;
    }

    // Enable the hook
    if (MH_EnableHook(loadbufferAddr) != MH_OK) {
        LOG_ERROR("Failed to enable hook for luaL_loadbuffer.");
        return 1;
    }

    LOG_INFO("Hook for luaL_loadbuffer set up successfully at {:p}!", loadbufferAddr);
    #pragma endregion

    return 0;
}

#pragma endregion

#pragma region DLL Entry Point
/**
 * @brief Entry point for the DLL. Creates a new thread to run our main logic in.
 */
BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
    }

    return TRUE;
}
#pragma endregion