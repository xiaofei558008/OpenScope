/* 假 mcdxdas.dll：可控延迟/失败，统计 MCD 并发度（验证 MCD 全局锁）与会话泄漏 */
#include "pre.h"
#include <stdio.h>
#include <string.h>
#include "mcd/mcd_api.h"
#include "fake_mcd.h"

volatile LONG g_fake_open_delay_ms, g_fake_tx_delay_ms, g_fake_fail_tx;
volatile LONG g_fake_inside, g_fake_max_inside;
volatile LONG g_fake_open_srv, g_fake_close_srv, g_fake_open_core, g_fake_close_core;
volatile LONG g_fake_stale_use, g_fake_trig_create;
volatile LONG g_fake_free_lib, g_fake_exit;

#define CORE_SLOTS 64
static mcd_core_st   s_cores[CORE_SLOTS];
static volatile LONG s_core_alive[CORE_SLOTS];
static mcd_server_st s_srv[CORE_SLOTS];

static void enter(void)
{
    LONG n = InterlockedIncrement(&g_fake_inside), m;
    while ((m = InterlockedCompareExchange(&g_fake_max_inside, 0, 0)) < n &&
           InterlockedCompareExchange(&g_fake_max_inside, n, m) != m) {}
}
static void leave(void) { InterlockedDecrement(&g_fake_inside); }
static void nap(volatile LONG* ms) { LONG v = InterlockedCompareExchange(ms, 0, 0); if (v > 0) Sleep((DWORD)v); }
static int core_ix(const mcd_core_st* c) { return c ? (int)(c - s_cores) : -1; }

static mcd_return_et f_initialize(const mcd_api_version_st* v, mcd_impl_version_info_st* i) { (void)v; memset(i, 0, sizeof(*i)); return MCD_RET_ACT_NONE; }
static void f_exit(void) { InterlockedIncrement(&g_fake_exit); }
static mcd_return_et f_qry_servers(const mcd_char_t* h, mcd_bool_t r, uint32_t s, uint32_t* n, mcd_server_info_st* si)
{ (void)h; (void)r; (void)s; enter(); if (*n) { memset(si, 0, sizeof(*si)); strcpy(si->server, "UDAS"); *n = 1; } leave(); return MCD_RET_ACT_NONE; }
static mcd_return_et f_open_server(const mcd_char_t* k, const mcd_char_t* c, mcd_server_st** s)
{ LONG i; (void)k; (void)c; enter(); nap(&g_fake_open_delay_ms); i = InterlockedIncrement(&g_fake_open_srv); *s = &s_srv[i % CORE_SLOTS]; leave(); return MCD_RET_ACT_NONE; }
static mcd_return_et f_close_server(const mcd_server_st* s) { (void)s; enter(); InterlockedIncrement(&g_fake_close_srv); leave(); return MCD_RET_ACT_NONE; }
static mcd_return_et f_qry_systems(uint32_t s, uint32_t* n, mcd_core_con_info_st* x) { (void)s; enter(); memset(x, 0, sizeof(*x)); *n = 1; leave(); return MCD_RET_ACT_NONE; }
static mcd_return_et f_qry_devices(const mcd_core_con_info_st* sys, uint32_t s, uint32_t* n, mcd_core_con_info_st* d)
{ (void)sys; (void)s; enter(); memset(d, 0, sizeof(*d)); strcpy(d->device, "TC375"); strcpy(d->acc_hw, "FakeWiggler"); *n = 1; leave(); return MCD_RET_ACT_NONE; }
static mcd_return_et f_qry_cores(const mcd_core_con_info_st* d, uint32_t s, uint32_t* n, mcd_core_con_info_st* c)
{ (void)d; (void)s; enter(); memset(c, 0, sizeof(*c)); strcpy(c->core, "Core0"); *n = 1; leave(); return MCD_RET_ACT_NONE; }
static mcd_return_et f_open_core(const mcd_core_con_info_st* ci, mcd_core_st** c)
{ LONG i; (void)ci; enter(); i = InterlockedIncrement(&g_fake_open_core) % CORE_SLOTS; InterlockedExchange(&s_core_alive[i], 1); *c = &s_cores[i]; leave(); return MCD_RET_ACT_NONE; }
static mcd_return_et f_close_core(const mcd_core_st* c)
{ int i = core_ix(c); enter(); if (i >= 0) InterlockedExchange(&s_core_alive[i], 0); InterlockedIncrement(&g_fake_close_core); leave(); return MCD_RET_ACT_NONE; }
static void f_qry_error_info(const mcd_core_st* c, mcd_error_info_st* e) { (void)c; enter(); memset(e, 0, sizeof(*e)); strcpy(e->error_str, "fake"); leave(); }
static void check_alive(const mcd_core_st* c) { int i = core_ix(c); if (i < 0 || i >= CORE_SLOTS || !InterlockedCompareExchange(&s_core_alive[i], 0, 0)) InterlockedIncrement(&g_fake_stale_use); }
static mcd_return_et f_execute_txlist(const mcd_core_st* c, mcd_txlist_st* tl)
{
    uint32_t k; mcd_return_et r = MCD_RET_ACT_NONE;
    uint32_t bytes = 0;
    for (k = 0; k < tl->num_tx; k++) bytes += tl->tx[k].num_bytes;
    enter(); check_alive(c);
    if (bytes > 256) nap(&g_fake_tx_delay_ms);   /* 只让采集的大块读变慢；UI 自己拿到锁后的小读照常快 */
    if (InterlockedCompareExchange(&g_fake_fail_tx, 0, 0)) { tl->num_tx_ok = 0; r = MCD_RET_ACT_HANDLE_ERROR; }
    else { for (k = 0; k < tl->num_tx; k++) { memset(tl->tx[k].data, 0xAB, tl->tx[k].num_bytes); tl->tx[k].num_bytes_ok = tl->tx[k].num_bytes; } tl->num_tx_ok = tl->num_tx; }
    leave(); return r;
}
static mcd_return_et f_ok_core_bool(const mcd_core_st* c, mcd_bool_t b) { (void)b; enter(); check_alive(c); leave(); return MCD_RET_ACT_NONE; }
static mcd_return_et f_step(const mcd_core_st* c, mcd_bool_t b, mcd_core_step_type_et t, uint32_t n) { (void)b; (void)t; (void)n; enter(); check_alive(c); leave(); return MCD_RET_ACT_NONE; }
static mcd_return_et f_qry_state(const mcd_core_st* c, mcd_core_state_st* st)
{ enter(); check_alive(c); memset(st, 0, sizeof(*st)); st->state = MCD_CORE_STATE_DEBUG; leave(); return MCD_RET_ACT_NONE; }
static mcd_return_et f_qry_rst_classes(const mcd_core_st* c, uint32_t* v) { (void)c; enter(); *v = 1; leave(); return MCD_RET_ACT_NONE; }
static mcd_return_et f_rst(const mcd_core_st* c, uint32_t v, mcd_bool_t h) { (void)c; (void)v; (void)h; enter(); leave(); return MCD_RET_ACT_NONE; }
static mcd_return_et f_unused(void) { return MCD_RET_ACT_HANDLE_ERROR; }
static mcd_return_et f_qry_trig_info(const mcd_core_st* c, mcd_trig_info_st* ti) { (void)c; enter(); memset(ti, 0, sizeof(*ti)); leave(); return MCD_RET_ACT_NONE; }
static mcd_return_et f_create_trig(const mcd_core_st* c, void* t, uint32_t* id) { (void)t; enter(); check_alive(c); *id = (uint32_t)InterlockedIncrement(&g_fake_trig_create); leave(); return MCD_RET_ACT_NONE; }
static mcd_return_et f_remove_trig(const mcd_core_st* c, uint32_t id) { (void)c; (void)id; enter(); leave(); return MCD_RET_ACT_NONE; }
static mcd_return_et f_activate_trig_set(const mcd_core_st* c) { (void)c; enter(); leave(); return MCD_RET_ACT_NONE; }

static const struct { const char* n; void* f; } k_tab[] = {
    {"mcd_initialize_f", (void*)f_initialize}, {"mcd_exit_f", (void*)f_exit}, {"mcd_qry_servers_f", (void*)f_qry_servers},
    {"mcd_open_server_f", (void*)f_open_server}, {"mcd_close_server_f", (void*)f_close_server}, {"mcd_qry_systems_f", (void*)f_qry_systems},
    {"mcd_qry_devices_f", (void*)f_qry_devices}, {"mcd_qry_cores_f", (void*)f_qry_cores}, {"mcd_open_core_f", (void*)f_open_core},
    {"mcd_close_core_f", (void*)f_close_core}, {"mcd_qry_error_info_f", (void*)f_qry_error_info}, {"mcd_execute_txlist_f", (void*)f_execute_txlist},
    {"mcd_run_f", (void*)f_ok_core_bool}, {"mcd_stop_f", (void*)f_ok_core_bool}, {"mcd_step_f", (void*)f_step}, {"mcd_qry_state_f", (void*)f_qry_state},
    {"mcd_qry_rst_classes_f", (void*)f_qry_rst_classes}, {"mcd_rst_f", (void*)f_rst}, {"mcd_qry_reg_groups_f", (void*)f_unused},
    {"mcd_qry_reg_map_f", (void*)f_unused}, {"mcd_qry_mem_spaces_f", (void*)f_unused}, {"mcd_qry_server_config_f", (void*)f_unused},
    {"mcd_qry_trig_info_f", (void*)f_qry_trig_info}, {"mcd_create_trig_f", (void*)f_create_trig},
    {"mcd_remove_trig_f", (void*)f_remove_trig}, {"mcd_activate_trig_set_f", (void*)f_activate_trig_set},
};
static int s_lib;
HMODULE fake_LoadLibraryExA(const char* p, HANDLE f, DWORD fl) { (void)p; (void)f; (void)fl; return (HMODULE)&s_lib; }
FARPROC fake_GetProcAddress(HMODULE h, const char* n)
{ size_t i; (void)h; for (i = 0; i < sizeof(k_tab) / sizeof(k_tab[0]); i++) if (!strcmp(k_tab[i].n, n)) return (FARPROC)k_tab[i].f; return NULL; }
BOOL fake_FreeLibrary(HMODULE h) { if (h == (HMODULE)&s_lib) InterlockedIncrement(&g_fake_free_lib); return TRUE; }
