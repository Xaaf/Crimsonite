#pragma once

/**
 * Run a chunk of Lua code in the given `luaState`.
 */
void run_lua(void* luaState, const char* code);

/**
 * Called when we want to inject/execute our initial modloader script.
 * This is currently run once per `luaL_loadbuffer` call.
 */
void inject_lua(void* luaState);
