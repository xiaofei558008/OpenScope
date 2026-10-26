/* 假 CubeProgrammer_API.dll：导出表按名字查，可控卡住/掉线，统计并发与会话 */
#include "pre_probe.h"
#include <string.h>
#include <stdlib.h>
#include "stlink.h"
#include "fake_probe.h"

extern volatile LONG g_fk_session, g_fk_loaded;
static OS_StLinkDcp s_list[1];
static OS_StLinkGenInf s_gi;

static void s_noop_cb(OS_StLinkCallbacks c) { (void)c; fk_enter(0); fk_leave(); }
static void s_noop_int(int v) { (void)v; fk_enter(0); fk_leave(); }
static void s_noop_path(const char* p) { (void)p; fk_enter(0); fk_leave(); }
static int s_get_list(OS_StLinkDcp** l, int sh)
{
    (void)sh; fk_enter(0);
    memset(s_list, 0, sizeof(s_list));
    strcpy(s_list[0].serialNumber, "FAKE0001"); strcpy(s_list[0].board, "Fake ST-Link");
    s_list[0].freq.swdFreqNumber = 2; s_list[0].freq.swdFreq[0] = 4000; s_list[0].freq.swdFreq[1] = 1000;
    *l = s_list; fk_leave(); return 1;
}
static void s_del_list(void) { fk_enter(0); fk_leave(); }
static int s_connect(OS_StLinkDcp d) { (void)d; fk_enter(0); fk_nap(&g_fk_open_delay_ms); InterlockedIncrement(&g_fk_open); InterlockedExchange(&g_fk_session, 1); fk_leave(); return 0; }
static int s_check(void) { int r; fk_enter(0); r = FK_GET(g_fk_session) && !FK_GET(g_fk_fail); fk_leave(); return r; }
static void s_disconnect(void) { fk_enter(0); if (InterlockedExchange(&g_fk_session, 0)) InterlockedIncrement(&g_fk_close); fk_leave(); }
static int s_read(unsigned int a, unsigned char** p, unsigned int n)
{
    int r = 0;
    fk_enter(1);
    if (n > 256) fk_nap(&g_fk_read_delay_ms);
    if (FK_GET(g_fk_fail)) { *p = NULL; r = -1; }
    else {
        *p = (unsigned char*)malloc(n ? n : 1);
        memset(*p, 0xAB, n);
        if (a == 0xE000EDF0u && n >= 4) { uint32_t v = (1u << 17) | 1u; memcpy(*p, &v, 4); }   /* S_HALT | C_DEBUGEN */
    }
    fk_leave();
    return r;
}
static int s_write(unsigned int a, char* d, unsigned int n) { (void)a; (void)d; (void)n; fk_enter(1); fk_leave(); return 0; }
static int s_reset(int m) { (void)m; fk_enter(1); InterlockedIncrement(&g_fk_run_ctl); fk_leave(); return 0; }
static int s_execute(unsigned int a) { (void)a; fk_enter(1); fk_leave(); return 0; }
static OS_StLinkGenInf* s_gen(void) { fk_enter(1); memset(&s_gi, 0, sizeof(s_gi)); strcpy(s_gi.name, "STM32F4FAKE"); strcpy(s_gi.cpu, "Cortex-M4"); fk_leave(); return &s_gi; }
static int s_runctl(void) { fk_enter(1); InterlockedIncrement(&g_fk_run_ctl); fk_leave(); return 0; }
static int s_reg(unsigned int r, unsigned int* v) { fk_enter(1); *v = 0x08000000u + r * 4u; fk_leave(); return 0; }
static int s_core_state(void) { fk_enter(1); fk_leave(); return 0; }

static const struct { const char* n; void* f; } k_tab[] = {
    {"setDisplayCallbacks", (void*)s_noop_cb}, {"setVerbosityLevel", (void*)s_noop_int}, {"setLoadersPath", (void*)s_noop_path},
    {"getStLinkList", (void*)s_get_list}, {"deleteInterfaceList", (void*)s_del_list}, {"connectStLink", (void*)s_connect},
    {"checkDeviceConnection", (void*)s_check}, {"disconnect", (void*)s_disconnect}, {"readMemory", (void*)s_read},
    {"writeMemory", (void*)s_write}, {"reset", (void*)s_reset}, {"execute", (void*)s_execute},
    {"getDeviceGeneralInf", (void*)s_gen}, {"Halt", (void*)s_runctl}, {"Run", (void*)s_runctl}, {"Step", (void*)s_runctl},
    {"CoreReset", (void*)s_runctl}, {"SystemReset", (void*)s_runctl}, {"readCortexReg", (void*)s_reg}, {"getCoreState", (void*)s_core_state},
};
static int s_lib;
HMODULE fake_LoadLibraryA(const char* p) { (void)p; InterlockedIncrement(&g_fk_load); InterlockedExchange(&g_fk_loaded, 1); return (HMODULE)&s_lib; }
FARPROC fake_GetProcAddress(HMODULE h, const char* n)
{
    size_t i;
    if (!strcmp(n, "free")) return (FARPROC)free;   /* stlink.c 用 msvcrt 的 free 释放 readMemory 缓冲 */
    if (h != (HMODULE)&s_lib) return NULL;
    for (i = 0; i < sizeof(k_tab) / sizeof(k_tab[0]); i++) if (!strcmp(k_tab[i].n, n)) return (FARPROC)k_tab[i].f;
    return NULL;
}
BOOL fake_FreeLibrary(HMODULE h)
{ if (h == (HMODULE)&s_lib) { InterlockedIncrement(&g_fk_free); InterlockedExchange(&g_fk_loaded, 0); } return TRUE; }
