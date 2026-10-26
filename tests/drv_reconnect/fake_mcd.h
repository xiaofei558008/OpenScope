#ifndef FAKE_MCD_H
#define FAKE_MCD_H
extern volatile LONG g_fake_open_delay_ms, g_fake_tx_delay_ms, g_fake_fail_tx;
extern volatile LONG g_fake_inside, g_fake_max_inside;
extern volatile LONG g_fake_open_srv, g_fake_close_srv, g_fake_open_core, g_fake_close_core;
extern volatile LONG g_fake_stale_use, g_fake_trig_create;
#define FAKE_SET(v, x) InterlockedExchange(&(v), (LONG)(x))
#define FAKE_GET(v)    InterlockedCompareExchange(&(v), 0, 0)
#endif
