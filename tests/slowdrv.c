/*
 * slowdrv —— Bug4 回归用假探针驱动：GET_CPU_STATE / BP_LIST 每次阻塞 SLOWDRV_MS（默认 3000 ms），
 * 模拟 DAS/ST-Link 持锁重连或驱动 DLL 卡住。仅供 tests\ui_bug4_slowdrv.py 在隔离目录中加载，
 * 不放进 dll\（不会出现在正式软件的驱动列表里）。
 */
#define OPENSCOPE_MODULE_BUILD
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../code/src/module_api.h"

static volatile LONG g_conn;
static int slow_ms(void)
{
    char b[16];
    if (GetEnvironmentVariableA("SLOWDRV_MS", b, sizeof(b)) > 0) return atoi(b);
    return 3000;
}

static int sd_init(const OS_Framework* fw, void** ctx) { (void)fw; *ctx = NULL; return OS_ERR_OK; }
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
    case OS_CMD_CONNECT: InterlockedExchange(&g_conn, 1); if (out) *(int*)out = 0; return OS_ERR_OK;
    case OS_CMD_DISCONNECT: InterlockedExchange(&g_conn, 0); return OS_ERR_OK;
    case OS_CMD_IS_CONNECTED: if (out) *(int*)out = g_conn ? 1 : 0; return OS_ERR_OK;
    case OS_CMD_GET_CPU_STATE: {
        OS_CpuState* st = (OS_CpuState*)out;
        Sleep((DWORD)slow_ms());
        if (!st) return OS_ERR_INVALID_ARG;
        st->halted = 0; st->pc_valid = 0; st->powered = 1; st->under_reset = 0; st->voltage_mv = -1;
        strcpy(st->state, "RUNNING");
        return OS_ERR_OK;
    }
    case OS_CMD_BP_LIST: {
        OS_BpList* l = (OS_BpList*)out;
        Sleep((DWORD)slow_ms());
        if (l) l->count = 0;
        return OS_ERR_OK;
    }
    case OS_CMD_READ_MEM: {
        OS_MemReq* r = (OS_MemReq*)in;
        if (!r || !r->data) return OS_ERR_INVALID_ARG;
        memset(r->data, 0, r->size);
        return (int)r->size;
    }
    default: return OS_ERR_UNSUPPORTED;
    }
}

static const OS_Module g_mod = {
    OS_API_VERSION, OS_CAP_DRIVER, "slowdrv", "1.0", "Bug4 test: blocking probe driver",
    NULL, sd_init, sd_deinit, sd_command, NULL, NULL, NULL, NULL, NULL, NULL
};

OS_MODULE_EXPORT const OS_Module* os_module_get(void) { return &g_mod; }
