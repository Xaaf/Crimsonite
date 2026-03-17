#pragma once

#include <cstdint>

using luaL_loadbuffer_t = int(__cdecl*)(void* luaState, const char* buff, size_t size, const char* name);
using lua_pcall_t = int(__cdecl*)(void* luaState, int nargs, int nresults, int errfunc);

extern luaL_loadbuffer_t g_originalLoadbuffer;
extern lua_pcall_t g_originalPcall;

static bool modsLoaded = false;
static bool shouldLoadMods = false;

/**
 * Initialise the hook system, returning true on success.
 */
bool initialise_hook_manager();

/**
 * Uninitialise the hook system.
 */
void shutdown_hook_manager();

/**
 * Called by the hooked `luaL_loadbuffer`. This is where our initial injection happens.
 */
int __cdecl hooked_loadbuffer(void* luaState, const char* buff, size_t size, const char* name);
