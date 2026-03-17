#include "console.h"
#include "hook_manager.h"

#include "log.h"

#include <windows.h>

/**
 * Main thread func for the modloader. It's done on a new thread to avoid blocking the game's main thread.
 */
DWORD WINAPI MainThread(LPVOID param) {
    setup_console();

    if (!initialise_hook_manager()) {
        return 1;
    }

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
