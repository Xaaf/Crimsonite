#include <windows.h>
#include <iostream>

DWORD WINAPI MainThread(LPVOID param) {
    AllocConsole();
    freopen("CONOUT$", "w", stdout);

    std::cout << "Crimsonite modloader loaded successfully!" << std::endl;

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
    }

    return TRUE;
}