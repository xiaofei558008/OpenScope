/*
 * ELF file monitor: watches the loaded ELF for recompilation and posts
 * WM_APP_ELF_CHANGED to the main window when its mtime changes.
 */
#include "app.h"

#include <string.h>

static ULONGLONG mtime_of(const char* path)
{
    WIN32_FILE_ATTRIBUTE_DATA fd;
    ULARGE_INTEGER u;
    if (!GetFileAttributesExA(path, GetFileExInfoStandard, &fd))
        return 0;
    u.LowPart = fd.ftLastWriteTime.dwLowDateTime;
    u.HighPart = fd.ftLastWriteTime.dwHighDateTime;
    return u.QuadPart;
}

static DWORD WINAPI monitor_thread_fn(LPVOID param)
{
    (void)param;
    ULONGLONG last = 0;
    while (g_app.mon_running) {
        ULONGLONG now;
        if (g_app.elf && g_app.elf_path[0]) {
            now = mtime_of(g_app.elf_path);
            if (now && now != last) {
                if (last != 0 && now != g_app.elf_mtime) {
                    InterlockedExchange(&g_app.elf_reload_pending, 1);
                    if (g_app.hMain)
                        PostMessageW(g_app.hMain, WM_APP_ELF_CHANGED, 0, 0);
                }
                last = now;
            }
        } else {
            last = 0;
        }
        if (WaitForSingleObject(g_app.hMonStop, 1000) == WAIT_OBJECT_0) break;
    }
    InterlockedExchange(&g_app.mon_running, 0);
    return 0;
}

void os_start_monitor(void)
{
    if (g_app.mon_running) return;
    if (!g_app.hMonStop) g_app.hMonStop = CreateEventW(NULL, TRUE, FALSE, NULL);
    else ResetEvent(g_app.hMonStop);
    InterlockedExchange(&g_app.mon_running, 1);
    g_app.hMonThread = CreateThread(NULL, 0, monitor_thread_fn, NULL, 0, NULL);
}

void os_stop_monitor(void)
{
    if (!g_app.mon_running) return;
    if (g_app.hMonStop) SetEvent(g_app.hMonStop);
    if (g_app.hMonThread) {
        WaitForSingleObject(g_app.hMonThread, 2000);
        CloseHandle(g_app.hMonThread);
        g_app.hMonThread = NULL;
    }
    InterlockedExchange(&g_app.mon_running, 0);
}
