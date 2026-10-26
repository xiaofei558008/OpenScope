/*
 * slowdrv —— Bug4 回归用假探针驱动。两种模式（环境变量 SLOWDRV_MODE）：
 *   again（默认）：按小序第二轮驱动设计——驱动加载后与每次 CONNECT 成功后各有一段 SLOWDRV_MS（默认 3000 ms）
 *       的“后台重连”窗口；窗口内 CONNECT 立即返回 OS_ERR_AGAIN，GET_CPU_STATE / BP_LIST 等 50 ms（限时锁）
 *       后返回 OS_ERR_AGAIN；窗口结束后正常返回。
 *   block：旧驱动行为——GET_CPU_STATE / BP_LIST 每次阻塞 SLOWDRV_MS，模拟持锁重连或驱动 DLL 卡住。
 * SLOWDRV_CONNECT_MS（默认 0）：CONNECT 耗时，期间 DISCONNECT 返回取消；CONNECT 成功经 fw->log 打一行（测日志前缀）。
 * SLOWDRV_READ_MS（默认 0）：每次 READ_MEM 阻塞该时长（tests\\ui_bug4_acqstop.py 用）。
 * 统计写到 %SLOWDRV_STATS%（若设置）：connect_again connect_ok cpu_again cpu_ok bp_again bp_ok。仅供 tests\ui_bug4_slowdrv.py 在隔离目录中加载，
 * 不放进 dll\（不会出现在正式软件的驱动列表里）。
 */
#define OPENSCOPE_MODULE_BUILD
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../code/src/module_api.h"

static volatile LONG g_conn, g_cancel;
static int slow_ms(void)
{
    char b[16];
    if (GetEnvironmentVariableA("SLOWDRV_MS", b, sizeof(b)) > 0) return atoi(b);
    return 3000;
}

static int is_block(void)
{
    char b[16];
    return GetEnvironmentVariableA("SLOWDRV_MODE", b, sizeof(b)) > 0 && _stricmp(b, "block") == 0;
}
static volatile LONG g_busy_until;   /* GetTickCount 值；之前处于“后台重连” */
static volatile LONG g_st[6];         /* connect_again connect_ok cpu_again cpu_ok bp_again bp_ok */
static int busy(void) { return (LONG)(GetTickCount() - (DWORD)g_busy_until) < 0; }
static void sd_stat(int i)
{
    char path[MAX_PATH]; FILE* f;
    InterlockedIncrement(&g_st[i]);
    if (GetEnvironmentVariableA("SLOWDRV_STATS", path, sizeof(path)) == 0) return;
    if (fopen_s(&f, path, "w") == 0 && f) {
        fprintf(f, "%ld %ld %ld %ld %ld %ld\n", g_st[0], g_st[1], g_st[2], g_st[3], g_st[4], g_st[5]);
        fclose(f);
    }
}
static void start_window(void) { InterlockedExchange(&g_busy_until, (LONG)(GetTickCount() + (DWORD)slow_ms())); }

static const OS_Framework* g_fw;
static int env_ms(const char* name)
{
    char b[16];
    return GetEnvironmentVariableA(name, b, sizeof(b)) > 0 ? atoi(b) : 0;
}
static int sd_init(const OS_Framework* fw, void** ctx) { g_fw = fw; *ctx = NULL; start_window(); return OS_ERR_OK; }
static void sd_deinit(void* ctx) { (void)ctx; }

static int sd_command(void* ctx, int cmd, void* in, void* out)
{
    (void)ctx;
    switch (cmd) {
    case OS_CMD_SCAN: {
        OS_ScanReq* r = (OS_ScanReq*)in;
        if (!r || !r->items || r->capacity < 1) return OS_ERR_INVALID_ARG;
        memset(&r->items[0], 0, sizeof(r->items[0]));
        strcpy(r->items[0].serial, "SLOW0");
        strcpy(r->items[0].name, "slowdrv fake probe");
        r->count = 1;
        return OS_ERR_OK;
    }
    case OS_CMD_CONNECT:
        if (!is_block() && busy()) { sd_stat(0); if (out) *(int*)out = OS_ERR_AGAIN; return OS_ERR_AGAIN; }
        {   /* SLOWDRV_CONNECT_MS：CONNECT 本身耗时（测异步连接中的置灰/拒绝），期间 DISCONNECT 可取消 */
            int ms = env_ms("SLOWDRV_CONNECT_MS"), t;
            InterlockedExchange(&g_cancel, 0);
            for (t = 0; t < ms; t += 20) {
                if (g_cancel) { if (g_fw && g_fw->log) g_fw->log(OS_LOG_INFO, "slowdrv 模块: 连接已取消"); return OS_ERR_CANCELED; }
                Sleep(20);
            }
        }
        InterlockedExchange(&g_conn, 1); if (out) *(int*)out = 0;
        sd_stat(1);
        if (g_fw && g_fw->log) g_fw->log(OS_LOG_INFO, "slowdrv 模块: 已连接 %s", (in && ((OS_ConnectCfg*)in)->device[0]) ? ((OS_ConnectCfg*)in)->device : "?");
        if (!is_block()) start_window();   /* 连上后再来一段“后台重连”，让 UI 走 AGAIN 分支 */
        return OS_ERR_OK;
    case OS_CMD_DISCONNECT: InterlockedExchange(&g_cancel, 1); InterlockedExchange(&g_conn, 0); return OS_ERR_OK;
    case OS_CMD_IS_CONNECTED: if (out) *(int*)out = g_conn ? 1 : 0; return OS_ERR_OK;
    case OS_CMD_GET_CPU_STATE: {
        OS_CpuState* st = (OS_CpuState*)out;
        if (is_block()) Sleep((DWORD)slow_ms());
        else if (busy()) { Sleep(50); sd_stat(2); return OS_ERR_AGAIN; }
        else sd_stat(3);
        if (!st) return OS_ERR_INVALID_ARG;
        st->halted = 0; st->pc_valid = 0; st->powered = 1; st->under_reset = 0; st->voltage_mv = -1;
        strcpy(st->state, "RUNNING");
        return OS_ERR_OK;
    }
    case OS_CMD_BP_LIST: {
        OS_BpList* l = (OS_BpList*)out;
        if (is_block()) Sleep((DWORD)slow_ms());
        else if (busy()) { Sleep(50); sd_stat(4); return OS_ERR_AGAIN; }
        else sd_stat(5);
        if (l) l->count = 0;
        return OS_ERR_OK;
    }
    case OS_CMD_READ_MEM: {
        OS_MemReq* r = (OS_MemReq*)in;
        if (!r || !r->data) return OS_ERR_INVALID_ARG;
        {   /* SLOWDRV_READ_MS：采集读阻塞（测 os_ds_stop 有界等待） */
            char e[16];
            if (GetEnvironmentVariableA("SLOWDRV_READ_MS", e, sizeof(e)) > 0 && atoi(e) > 0) Sleep((DWORD)atoi(e));
        }
        memset(r->data, 0, r->size);
        return (int)r->size;
    }
    default: return OS_ERR_UNSUPPORTED;
    }
}

static const OS_Module g_mod = {
    OS_API_VERSION, OS_CAP_DRIVER, "slowdrv", "1.0", "Bug4 test: busy/reconnecting probe driver",
    NULL, sd_init, sd_deinit, sd_command, NULL, NULL, NULL, NULL, NULL, NULL
};

OS_MODULE_EXPORT const OS_Module* os_module_get(void) { return &g_mod; }
