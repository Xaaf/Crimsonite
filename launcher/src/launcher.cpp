#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif
#include <iostream>

int main() {
    std::cout << "Crimsonite Launcher starting..." << std::endl;

#if defined(_WIN32) || defined(_WIN64)
    STARTUPINFOA startupInfo{};
    PROCESS_INFORMATION processInfo{};

    startupInfo.cb = sizeof(startupInfo);
    const char* gamePath = "Coromon.exe";

    BOOL success = CreateProcessA(
        gamePath,       // application name
        nullptr,        // command line
        nullptr,        // process attributes
        nullptr,        // thread attributes
        FALSE,          // inherit handles
        0,              // creation flags
        nullptr,        // environment
        nullptr,        // working directory
        &startupInfo,
        &processInfo    
    );

    if (!success) {
        std::cerr << "Failed to launch the game. Error code: " << GetLastError() << std::endl;
        return 1;
    }

    std::cout << "Game launched successfully!" << std::endl;

    // Clean-up handles
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);

    return 0;
#else
    std::cerr << "Currently, Crimsonite Launcher is only supported on Windows." << std::endl;
    return 1;
#endif
}