#include "console.h"
#include "log.h"

#include <cstdio>
#include <iostream>

#include <windows.h>

void setup_console() {
    static bool initialized = false;
    if (initialized) {
        return;
    }
    initialized = true;

    AllocConsole();

    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    freopen_s(&f, "CONOUT$", "w", stderr);
    freopen_s(&f, "CONIN$", "r", stdin);

    std::ios::sync_with_stdio(true);

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        }
    }

    LOG_INFO("Crimsonite modloader loaded successfully!");
}
