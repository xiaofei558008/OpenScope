/* 假 JLink_x64.dll：导出表按名字查（JLINKARM_*），可控卡住/掉线，统计并发与会话 */
#include "pre_probe.h"
#include <string.h>
#include <stdlib.h>
#include "jlink.h"
#include "fake_probe.h"

extern volatile LONG g_fk_session, g_fk_loaded;
static volatile LONG s_target, s_bp_handle;

static int j_open(void* p) { (void)p; fk_enter(0); fk_nap(&g_fk_open_delay_ms); InterlockedIncrement(&g_fk_open); InterlockedExchange(&g_fk_session, 1); fk_leave(); return 0; }
static void j_close(void) { fk_enter(0); if (InterlockedExchange(&g_fk_session, 0)) InterlockedIncrement(&g_fk_close); InterlockedExchange(&s_target, 0); fk_leave(); }
static int j_is_connected(void) { int r; fk_enter(1); r = FK_GET(s_target) && !FK_GET(g_fk_fail); fk_leave(); return r; }
static int j_exec(const char* c, char* r, int n) { (void)c; fk_enter(1); if (r && n) r[0] = 0; fk_leave(); return 0; }
static int j_connect(void) { fk_enter(1); InterlockedExchange(&s_target, 1); fk_leave(); return 0; }
static int j_tif(int t) { (void)t; fk_enter(1); fk_leave(); return 0; }
static int j_runctl(void) { fk_enter(1); InterlockedIncrement(&g_fk_run_ctl); fk_leave(); return 0; }
static int j_is_halted(void) { fk_enter(1); fk_leave(); return 1; }
static uint32_t j_read_reg(int i) { fk_enter(1); fk_leave(); return 0x20000000u + (uint32_t)i * 4u; }
static int j_set_bp(uint32_t a, uint32_t t) { (void)a; (void)t; fk_enter(1); InterlockedIncrement(&g_fk_bp_set); fk_leave(); return (int)InterlockedIncrement(&s_bp_handle); }
static int j_clr_bp(int h) { (void)h; fk_enter(1); fk_leave(); return 0; }
static int j_hw_status(void* p) { OS_JLinkHwStatus* s = (OS_JLinkHwStatus*)p; fk_enter(1); memset(s, 0, sizeof(*s)); s->VTarget = 3300; s->tres = 1; fk_leave(); return 0; }
static int j_set_speed(int k) { (void)k; fk_enter(1); fk_leave(); return 0; }
static int j_read_mem(uint32_t a, uint32_t n, uint8_t* d)
{
    int r = 0; (void)a;
    fk_enter(1);
    if (n > 256) fk_nap(&g_fk_read_delay_ms);
    if (FK_GET(g_fk_fail)) r = -1; else memset(d, 0xAB, n);
    fk_leave();
    return r;
}
static int j_write_mem(uint32_t a, uint32_t n, const uint8_t* d) { (void)a; (void)n; (void)d; fk_enter(1); fk_leave(); return 0; }
static int j_ver(void) { fk_enter(0); fk_leave(); return 79000; }
static void j_fw(char* b, int n) { fk_enter(1); strncpy(b, "J-Link FAKE V11", (size_t)n - 1); b[n - 1] = 0; fk_leave(); }
static int j_caps(void) { fk_enter(0); fk_leave(); return 0; }
static uint32_t j_num(void) { fk_enter(0); fk_leave(); return 1; }
static int j_list(int host, void* infos, int n)
{
    (void)host; fk_enter(0);
    if (infos && n > 0) { OS_JLinkEmuInfo* e = (OS_JLinkEmuInfo*)infos; memset(e, 0, sizeof(*e)); e->SerialNumber = 123; strcpy(e->acProduct, "Fake J-Link"); }
    fk_leave(); return 1;
}
static int j_sel(int i) { (void)i; fk_enter(0); fk_leave(); return 0; }
static int j_sel_sn(uint32_t s) { (void)s; fk_enter(0); fk_leave(); return 0; }

static const struct { const char* n; void* f; } k_tab[] = {
    {"JLINKARM_Open", (void*)j_open}, {"JLINKARM_Close", (void*)j_close}, {"JLINKARM_IsConnected", (void*)j_is_connected},
    {"JLINKARM_ExecCommand", (void*)j_exec}, {"JLINKARM_Connect", (void*)j_connect}, {"JLINKARM_TIF_Select", (void*)j_tif},
    {"JLINKARM_Halt", (void*)j_runctl}, {"JLINKARM_Go", (void*)j_runctl}, {"JLINKARM_Reset", (void*)j_runctl},
    {"JLINKARM_Step", (void*)j_runctl}, {"JLINKARM_ResetNoHalt", (void*)j_runctl}, {"JLINKARM_IsHalted", (void*)j_is_halted},
    {"JLINKARM_ReadReg", (void*)j_read_reg}, {"JLINKARM_SetBPEx", (void*)j_set_bp}, {"JLINKARM_ClrBPEx", (void*)j_clr_bp},
    {"JLINKARM_GetHWStatus", (void*)j_hw_status}, {"JLINKARM_SetSpeed", (void*)j_set_speed},
    {"JLINKARM_ReadMem", (void*)j_read_mem}, {"JLINKARM_WriteMem", (void*)j_write_mem},
    {"JLINKARM_GetDLLVersion", (void*)j_ver}, {"JLINKARM_GetHardwareVersion", (void*)j_ver},
    {"JLINKARM_GetFirmwareString", (void*)j_fw}, {"JLINKARM_GetEmuCaps", (void*)j_caps},
    {"JLINKARM_EMU_GetNumDevices", (void*)j_num}, {"JLINKARM_EMU_GetList", (void*)j_list},
    {"JLINKARM_EMU_SelectByIndex", (void*)j_sel}, {"JLINKARM_EMU_SelectByUSBSN", (void*)j_sel_sn},
};
static int s_lib;
HMODULE fake_LoadLibraryA(const char* p) { (void)p; InterlockedIncrement(&g_fk_load); InterlockedExchange(&g_fk_loaded, 1); return (HMODULE)&s_lib; }
FARPROC fake_GetProcAddress(HMODULE h, const char* n)
{ size_t i; if (h != (HMODULE)&s_lib) return NULL; for (i = 0; i < sizeof(k_tab) / sizeof(k_tab[0]); i++) if (!strcmp(k_tab[i].n, n)) return (FARPROC)k_tab[i].f; return NULL; }
BOOL fake_FreeLibrary(HMODULE h)
{ if (h == (HMODULE)&s_lib) { InterlockedIncrement(&g_fk_free); InterlockedExchange(&g_fk_loaded, 0); InterlockedExchange(&g_fk_session, 0); } return TRUE; }
