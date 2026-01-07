#include <Windows.h>
#include <thread>
#include <chrono>
#include "Hooks.h"

static HANDLE g_MainThread = nullptr;

DWORD WINAPI MainThread(LPVOID module)
{
#ifdef _DEBUG
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
#endif

    Hooks::Setup();

    while (!(GetAsyncKeyState(VK_END) & 1))
        std::this_thread::sleep_for(std::chrono::milliseconds(150));

    Hooks::Destroy();

#ifdef _DEBUG
    if (f) fclose(f);
    FreeConsole();
#endif

    FreeLibraryAndExitThread(static_cast<HMODULE>(module), 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
        g_MainThread = CreateThread(
            nullptr,
            0,
            MainThread,
            hModule,
            0,
            nullptr
        );
    }

    return TRUE;
}
