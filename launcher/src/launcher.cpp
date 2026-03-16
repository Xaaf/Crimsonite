#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif
#include <iostream>

int main() {
    // TODO: Consider logging more properly, as opposed to calling `std::cout` manually each time
    std::cout << "Crimsonite starting..." << std::endl;

#if defined(_WIN32) || defined(_WIN64)
    STARTUPINFOA gameStartupInfo{};
    PROCESS_INFORMATION gameProcessInfo{};

    gameStartupInfo.cb = sizeof(gameStartupInfo);
    // TODO: Consider changing this, since the game may not always be called exactly like this
    const char* gamePath = "coromon.exe";

    // https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessa
    BOOL success = CreateProcessA(
        gamePath,           // application name
        nullptr,            // command line
        nullptr,            // process attributes
        nullptr,            // thread attributes
        FALSE,              // inherit handles
        CREATE_SUSPENDED,   // creation flags
        nullptr,            // environment
        nullptr,            // working directory
        &gameStartupInfo,
        &gameProcessInfo
    );

    if (!success) {
        std::cerr << "Failed to launch the game. Error code: " << GetLastError() << std::endl;
        return 1;
    }

    std::cout << "Game launched successfully in suspension!" << std::endl;

    char dllPath[MAX_PATH];
    GetFullPathNameA("crimsonite.dll", MAX_PATH, dllPath, NULL);
    size_t pathSize = strlen(dllPath) + 1;

    std::cout << ">> Full path to DLL: " << dllPath << std::endl;

    // https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualallocex
    LPVOID remoteMemory = VirtualAllocEx(
        gameProcessInfo.hProcess,   // process to allocate memory in
        nullptr,                    // desired starting address
        pathSize,                   // size of the allocation
        MEM_COMMIT | MEM_RESERVE,   // allocation type
        PAGE_READWRITE              // protection
    );

    if (!remoteMemory) {
        std::cerr << "Failed to allocate memory in the target process. Error code: " << GetLastError() << std::endl;
        TerminateProcess(gameProcessInfo.hProcess, 1);
        return 1;
    }

    SIZE_T bytesWritten;
    // https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-writeprocessmemory
    WriteProcessMemory(
        gameProcessInfo.hProcess,   // process to write to
        remoteMemory,               // base address to write to
        dllPath,                    // buffer to write
        pathSize,                   // number of bytes to write
        &bytesWritten               // number of bytes written
    );

    std::cout << "DLL path written to target process memory! (wrote " << bytesWritten << " bytes)" << std::endl;

    HMODULE kernel32Module = GetModuleHandleA("kernel32.dll");
    LPVOID loadLibraryAddress = GetProcAddress(kernel32Module, "LoadLibraryA");

    // https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createremotethread
    HANDLE remoteThread = CreateRemoteThread(
        gameProcessInfo.hProcess,   // process to create the thread in
        nullptr,                    // thread attributes
        0,                          // stack size (0 = default)
        (LPTHREAD_START_ROUTINE)loadLibraryAddress, // start address (`LoadLibraryA`); we fetch thsi from kernel32.dll
        remoteMemory,               // pointer to the DLL path in the target process's memory
        0,                          // creation flags
        nullptr                     // thread ID (not needed)
    );

    // Wait for the remote thread to finish executing, basically wait for `LoadLibraryA` to return
    WaitForSingleObject(remoteThread, INFINITE);

    // Use the exit code to check whether we succeed or not
    DWORD exitCode;
    GetExitCodeThread(remoteThread, &exitCode);

    if (exitCode == 0) {
        std::cerr << "Failed to inject the DLL. LoadLibraryA returned NULL." << std::endl;
        TerminateProcess(gameProcessInfo.hProcess, 1);
        return 1;
    }

    std::cout << "DLL injected successfully! (Module handle: " << exitCode << ")" << std::endl;
    CloseHandle(remoteThread); // No longer in use, so close the handle

    ResumeThread(gameProcessInfo.hThread);

    CloseHandle(gameProcessInfo.hProcess); // No longer in use, so close the handle
    CloseHandle(gameProcessInfo.hThread); // No longer in use, so close the handle

    return 0;
#else
    std::cerr << "Currently, Crimsonite is only supported on Windows." << std::endl;
    return 1;
#endif
}