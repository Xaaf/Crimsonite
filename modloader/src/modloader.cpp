#include <windows.h>
#include <iostream>

#include "log.h"

DWORD WINAPI MainThread(LPVOID param) {
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

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
    }

    return TRUE;
}