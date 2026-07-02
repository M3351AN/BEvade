#include <windows.h>
#include "bypass.h"

static HANDLE g_thread = NULL;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    (void)lpvReserved;
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hinstDLL);
        g_thread = CreateThread(NULL, 0, be_thread, NULL, 0, NULL);
        return g_thread != NULL ? TRUE : FALSE;
    case DLL_PROCESS_DETACH:
        g_running = false;
        if (g_thread) {
            WaitForSingleObject(g_thread, 5000);
            CloseHandle(g_thread);
            g_thread = NULL;
        }
        break;
    }
    return TRUE;
}
