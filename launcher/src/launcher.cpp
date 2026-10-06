#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#endif

#include "log.h"

// TODO: See if we can do this more dynamically as oppposed to this hardcoded way
const char* executable_name = "coromon.exe";

void launch_coromon()
{
    // See https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/ns-processthreadsapi-startupinfoa
    STARTUPINFO startupInfo;
    // See https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/ns-processthreadsapi-process_information
    PROCESS_INFORMATION processInfo;

    // Fill structs' memory areas with zeroes
    ZeroMemory(&startupInfo, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo); // STARTUPINFO requires us to set its size (DWORD)
    ZeroMemory(&processInfo, sizeof(processInfo));

    // See https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessa
    bool success = CreateProcessA(
        executable_name,    // application name
        nullptr,            // command line args to pass on
        nullptr,            // returned handle cannot be inherited
        nullptr,            // use default security descriptor on the thread
        false,              // returned handle cannot be inherited
        0,                  // we do not use any creation flags
        nullptr,            // use the environment of the calling process
        nullptr,            // use the same current directory as the calling process
        &startupInfo,       // reference to `startupInfo` to store information
        &processInfo        // reference to `processInfo` to store information
    );

    if (!success)
    {
        LOG_ERROR("Failed to start up Coromon! Is Crimsonite placed correctly?");
        return;
    }
}

int main(int argc, const char* argv[])
{
    LOG_INFO("Crimsonite is spreading...");

#if defined(_WIN32) || defined(_WIN64)
    launch_coromon();
#else
    LOG_ERROR("Crimsonite is only support on Windows!");
    return 1;
#endif

    return 0;
}