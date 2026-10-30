/* dpi.c - see dpi.h */
#include "dpi.h"

typedef BOOL (WINAPI *PFN_SetCtx)(HANDLE);
typedef UINT (WINAPI *PFN_GetDpiWnd)(HWND);
typedef UINT (WINAPI *PFN_GetDpiSys)(void);
typedef int  (WINAPI *PFN_GetSMDpi)(int, UINT);

static UINT g_dpi = 96;
static PFN_GetDpiWnd p_get_dpi_wnd;
static PFN_GetDpiSys p_get_dpi_sys;
static PFN_GetSMDpi  p_get_sm_dpi;

int os_dpi_init(void)
{
    HMODULE u = GetModuleHandleW(L"user32.dll");
    int mode = 0;
    if (!u) return 0;
    p_get_dpi_wnd = (PFN_GetDpiWnd)GetProcAddress(u, "GetDpiForWindow");
    p_get_dpi_sys = (PFN_GetDpiSys)GetProcAddress(u, "GetDpiForSystem");
    p_get_sm_dpi  = (PFN_GetSMDpi)GetProcAddress(u, "GetSystemMetricsForDpi");
    {
        PFN_SetCtx set = (PFN_SetCtx)GetProcAddress(u, "SetProcessDpiAwarenessContext");
        /* DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 == (HANDLE)-4 */
        if (set && set((HANDLE)(INT_PTR)-4)) mode = 2;
        else if (set && GetLastError() == ERROR_ACCESS_DENIED) mode = 2; /* already set by manifest */
        else if (SetProcessDPIAware()) mode = 1;
    }
    if (p_get_dpi_sys) g_dpi = p_get_dpi_sys();
    if (g_dpi < 48) g_dpi = 96;
    return mode;
}

UINT os_dpi_of(HWND hwnd)
{
    UINT d = 0;
    if (hwnd && p_get_dpi_wnd) d = p_get_dpi_wnd(hwnd);
    if (!d && p_get_dpi_sys) d = p_get_dpi_sys();
    return d ? d : 96;
}

UINT os_dpi_current(void) { return g_dpi; }
void os_dpi_set_current(UINT dpi) { if (dpi >= 48 && dpi <= 960) g_dpi = dpi; }
int  os_px_dpi(int v96, UINT dpi) { return MulDiv(v96, (int)dpi, 96); }
int  os_px(int v96) { return MulDiv(v96, (int)g_dpi, 96); }

int os_gap(void)
{
    int g = os_px(OS_GAP_VISUAL);
    return g < 1 ? 1 : g;
}

int os_gap_hit_pad(void)
{
    int pad = (os_px(OS_GAP_HIT) - os_gap() + 1) / 2;
    return pad < 0 ? 0 : pad;
}

int os_metric(int index)
{
    if (p_get_sm_dpi) return p_get_sm_dpi(index, g_dpi);
    return GetSystemMetrics(index);
}