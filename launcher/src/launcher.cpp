#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif
#include <filesystem>
#include <iostream>

#include "log.h"

bool ensure_folder_exists(const char* folderName) {
    try {
        if (!std::filesystem::exists(folderName)) {
            std::filesystem::create_directory(folderName);
            LOG_INFO("Created '{}' folder!", folderName);
            
            return true;
        } else {
            LOG_INFO("'{}' folder already exists.", folderName);
            return true;
        }
    } catch (const std::filesystem::filesystem_error& ex) {
        LOG_ERROR("Error occurred while checking/modifying '{}' folder: {}", folderName, ex.what());
        return false;
    }

    return true;
}

int main() {
    LOG_INFO("Crimsonite starting...");

#if defined(_WIN32) || defined(_WIN64)
    if (!ensure_folder_exists("mods")) { 
        LOG_ERROR("Failed to ensure 'mods' folder exists. Exiting...");
        return 1;
    }

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
        LOG_ERROR("Failed to launch the game. Error code: {}", GetLastError());
        return 1;
    }

    LOG_INFO("Game launched successfully in suspension!");

    char dllPath[MAX_PATH];
    GetFullPathNameA("crimsonite.dll", MAX_PATH, dllPath, NULL);
    size_t pathSize = strlen(dllPath) + 1;

    LOG_INFO(">> Full path to DLL: {}", dllPath);

    // https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualallocex
    LPVOID remoteMemory = VirtualAllocEx(
        gameProcessInfo.hProcess,   // process to allocate memory in
        nullptr,                    // desired starting address
        pathSize,                   // size of the allocation
        MEM_COMMIT | MEM_RESERVE,   // allocation type
        PAGE_READWRITE              // protection
    );

    if (!remoteMemory) {
        LOG_ERROR("Failed to allocate memory in the target process. Error code: {}", GetLastError());
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

    LOG_INFO("DLL path written to target process memory! (wrote {} bytes)", bytesWritten);

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
        LOG_ERROR("Failed to inject the DLL. LoadLibraryA returned NULL.");
        TerminateProcess(gameProcessInfo.hProcess, 1);
        return 1;
    }

    LOG_INFO("DLL injected successfully! (Module handle: {})", exitCode);
    CloseHandle(remoteThread); // No longer in use, so close the handle

    ResumeThread(gameProcessInfo.hThread);

    CloseHandle(gameProcessInfo.hProcess); // No longer in use, so close the handle
    CloseHandle(gameProcessInfo.hThread); // No longer in use, so close the handle

    return 0;
#else
    LOG_ERROR("Currently, Crimsonite is only supported on Windows.");
    return 1;
#endif
}