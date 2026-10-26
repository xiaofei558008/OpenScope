#include "pre_probe.h"
#include "fake_probe.h"

volatile LONG g_fk_open_delay_ms, g_fk_read_delay_ms, g_fk_fail;
volatile LONG g_fk_inside, g_fk_max_inside, g_fk_open, g_fk_close, g_fk_stale;
volatile LONG g_fk_load, g_fk_free, g_fk_bp_set, g_fk_run_ctl;
volatile LONG g_fk_session, g_fk_loaded;

void fk_enter(int need_session)
{
    LONG n = InterlockedIncrement(&g_fk_inside), m;
    while ((m = InterlockedCompareExchange(&g_fk_max_inside, 0, 0)) < n &&
           InterlockedCompareExchange(&g_fk_max_inside, n, m) != m) {}
    if (!InterlockedCompareExchange(&g_fk_loaded, 0, 0) || (need_session && !InterlockedCompareExchange(&g_fk_session, 0, 0)))
        InterlockedIncrement(&g_fk_stale);
}
void fk_leave(void) { InterlockedDecrement(&g_fk_inside); }
void fk_nap(volatile LONG* ms) { LONG v = InterlockedCompareExchange(ms, 0, 0); if (v > 0) Sleep((DWORD)v); }
