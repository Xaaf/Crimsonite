#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif
#include <iostream>

int main() {
    // TODO: Consider logging more properly, as opposed to calling `std::cout` manually each time
    std::cout << "Crimsonite starting..." << std::endl;

#if defined(_WIN32) || defined(_WIN64)
    STARTUPINFOA startupInfo{};
    PROCESS_INFORMATION processInfo{};

    startupInfo.cb = sizeof(startupInfo);
    // TODO: Consider changing this, since the game may not always be called exactly like this
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
    std::cerr << "Currently, Crimsonite is only supported on Windows." << std::endl;
    return 1;
#endif
}