/*
 * OpenScope Infineon DAS / miniWiggler 驱动模块（das.dll）
 *
 * 独立插件：与 jlink.dll / stlink.dll 相同，由主程序在 dll\ 下按 os_module_get 加载，
 * 只经 module_api.h（OS_CAP_DRIVER）与主程序交互。
 *
 * 后端：Infineon DAS（TAS/UDAS 服务器）MCD API，运行时 LoadLibrary mcdxdas.dll（mcdload.h），
 * 未安装 DAS 时模块仍可加载，扫描为空、连接报错。
 *
 * 能力：
 *  - OS_CMD_SCAN        枚举运行中/已安装的 DAS 服务器（miniWiggler / Lite Kit 板载 DAP）
 *  - OS_CMD_CONNECT     打开服务器 → 系统 → 器件 → 核（默认 Core0，device="core1" 可选核）；
 *                       Attach：不复位、不停核（AURIX DAP 可在运行时访问总线）
 *  - OS_CMD_READ_MEM    内存读：大块拆分为 ≤1KB 事务，一次 txlist 批量执行；SFR 区以 4 字节对齐访问
 *  - OS_CMD_WRITE_MEM   内存写：拒绝 PFLASH/DFLASH/UCB 区（0x8xxxxxxx / 0xAxxxxxxx），永不写 UCB
 *  - HALT / GO / RESET / RESET_RUN / GET_CPU_STATE / GET_INFO / IS_CONNECTED / AI_DESCRIBE
 *  - FLASH_DOWNLOAD / FLASH_ERASE：不支持（返回 OS_ERR_FAIL 并提示用 MemTool / TASKING dbgtc），
 *    以杜绝误写 UCB/保护配置的风险。
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "module_api.h"
#include "mcdload.h"
#include "das_util.h"

#define DAS_SERVER_DEFAULT "UDAS"
#define DAS_MAX_SERVERS 8
#define DAS_RECONNECT_MIN_MS 1000

typedef struct DasCtx {
    McdApi api;
    int bound;
    int mcd_inited;
    CRITICAL_SECTION cs;
    OS_ConnectCfg cfg;
    mcd_server_st* server;
    mcd_core_st* core;
    int core_index;
    int connected;
    OS_DriverInfo info;
    char server_name[64];
    char acc_hw[256];
    char device_name[64];
    char core_name[64];
    ULONGLONG last_reconnect_ms;
    char last_err[256];
} DasCtx;

static DasCtx g_ctx;
static const OS_Framework* g_fw;

#define LOG(lv, ...) do { if (g_fw && g_fw->log) g_fw->log((lv), __VA_ARGS__); } while (0)

static void das_record_err(const char* what)
{
    mcd_error_info_st e;
    memset(&e, 0, sizeof(e));
    if (g_ctx.bound) g_ctx.api.qry_error_info(g_ctx.core, &e);
    _snprintf(g_ctx.last_err, sizeof(g_ctx.last_err), "%s: 0x%x %s", what, e.error_code, e.error_str);
    g_ctx.last_err[sizeof(g_ctx.last_err) - 1] = 0;
}

static int das_ensure_init(void)
{
    mcd_api_version_st v; mcd_impl_version_info_st ii;
    char err[512];
    if (!g_ctx.bound) {
        if (mcd_bind(&g_ctx.api, err, sizeof(err)) != 0) {
            LOG(OS_LOG_WARN, "das 模块: %s", err);
            return OS_ERR_FAIL;
        }
        g_ctx.bound = 1;
        LOG(OS_LOG_INFO, "das 模块: 已绑定 %s", err);
    }
    if (!g_ctx.mcd_inited) {
        memset(&v, 0, sizeof(v)); memset(&ii, 0, sizeof(ii));
        v.v_api_major = MCD_API_VER_MAJOR; v.v_api_minor = MCD_API_VER_MINOR;
        strcpy(v.author, MCD_API_VER_AUTHOR);
        if (g_ctx.api.initialize(&v, &ii) != MCD_RET_ACT_NONE) return OS_ERR_FAIL;
        _snprintf(g_ctx.info.dll_version, sizeof(g_ctx.info.dll_version), "%u.%u.%u",
                  ii.v_imp_major, ii.v_imp_minor, ii.v_imp_build);
        g_ctx.mcd_inited = 1;
    }
    return OS_ERR_OK;
}

/* ---------- 扫描 ---------- */
static int mod_scan(OS_ScanReq* req)
{
    mcd_server_info_st si[DAS_MAX_SERVERS];
    uint32_t n = DAS_MAX_SERVERS, i;
    if (!req || !req->items || req->capacity <= 0) return OS_ERR_INVALID_ARG;
    req->count = 0;
    if (das_ensure_init() != OS_ERR_OK) return OS_ERR_FAIL;
    memset(si, 0, sizeof(si));
    EnterCriticalSection(&g_ctx.cs);
    if (g_ctx.api.qry_servers("localhost", TRUE, 0, &n, si) != MCD_RET_ACT_NONE) n = 0;
    LeaveCriticalSection(&g_ctx.cs);
    for (i = 0; i < n && i < DAS_MAX_SERVERS && req->count < req->capacity; i++) {
        OS_DeviceInfo* d = &req->items[req->count];
        memset(d, 0, sizeof(*d));
        d->index = (int)i;
        _snprintf(d->serial, sizeof(d->serial) - 1, "%s", si[i].server);
        _snprintf(d->name, sizeof(d->name) - 1, "DAS %s: %s", si[i].server,
                  si[i].acc_hw[0] ? si[i].acc_hw : "(no access HW)");
        req->count++;
    }
    return req->count > 0 ? OS_ERR_OK : OS_ERR_NO_DEVICE;
}

/* ---------- 连接 ---------- */
static int mod_disconnect_locked(void)
{
    if (g_ctx.core) { g_ctx.api.close_core(g_ctx.core); g_ctx.core = NULL; }
    if (g_ctx.server) { g_ctx.api.close_server(g_ctx.server); g_ctx.server = NULL; }
    g_ctx.connected = 0;
    g_ctx.info.connected = 0;
    return OS_ERR_OK;
}

static int mod_disconnect(void)
{
    if (!g_ctx.bound) return OS_ERR_OK;
    EnterCriticalSection(&g_ctx.cs);
    mod_disconnect_locked();
    LeaveCriticalSection(&g_ctx.cs);
    return OS_ERR_OK;
}

static int mod_connect_locked(void)
{
    char cfg[256];
    mcd_core_con_info_st sys, dev[4], cores[8];
    uint32_t n;
    const char* srv = g_ctx.cfg.serial[0] ? g_ctx.cfg.serial : DAS_SERVER_DEFAULT;
    mod_disconnect_locked();
    g_ctx.server = NULL;
    _snprintf(cfg, sizeof(cfg), "McdHostName=\"localhost\"\nMcdServerName=\"%s\"\n", srv);
    /* system_key 必须是 ""：传 NULL 时 mcdxdas.dll 访问冲突 */
    if ((g_ctx.api.open_server("", cfg, &g_ctx.server) != MCD_RET_ACT_NONE || !g_ctx.server) &&
        strcmp(srv, DAS_SERVER_DEFAULT) != 0) {
        /* serial 不是有效 DAS 服务器名（如残留的其它探针序列号）：回退默认 UDAS */
        srv = DAS_SERVER_DEFAULT;
        _snprintf(cfg, sizeof(cfg), "McdHostName=\"localhost\"\nMcdServerName=\"%s\"\n", srv);
        g_ctx.server = NULL;
        g_ctx.api.open_server("", cfg, &g_ctx.server);
    }
    if (!g_ctx.server) { das_record_err("open_server"); return OS_ERR_NO_DEVICE; }
    memset(&sys, 0, sizeof(sys)); n = 1;
    if (g_ctx.api.qry_systems(0, &n, &sys) != MCD_RET_ACT_NONE || n == 0) { das_record_err("qry_systems"); goto fail; }
    memset(dev, 0, sizeof(dev)); n = 4;
    if (g_ctx.api.qry_devices(&sys, 0, &n, dev) != MCD_RET_ACT_NONE || n == 0) { das_record_err("qry_devices(目标未上电/未连接?)"); goto fail; }
    memset(cores, 0, sizeof(cores)); n = 8;
    if (g_ctx.api.qry_cores(&dev[0], 0, &n, cores) != MCD_RET_ACT_NONE || n == 0) { das_record_err("qry_cores"); goto fail; }
    g_ctx.core_index = das_parse_core_index(g_ctx.cfg.device);
    if ((uint32_t)g_ctx.core_index >= n) g_ctx.core_index = 0;
    if (g_ctx.api.open_core(&cores[g_ctx.core_index], &g_ctx.core) != MCD_RET_ACT_NONE || !g_ctx.core) {
        g_ctx.core = NULL; das_record_err("open_core"); goto fail;
    }
    _snprintf(g_ctx.server_name, sizeof(g_ctx.server_name), "%s", srv);
    _snprintf(g_ctx.acc_hw, sizeof(g_ctx.acc_hw), "%s", dev[0].acc_hw);
    _snprintf(g_ctx.device_name, sizeof(g_ctx.device_name), "%s", dev[0].device);
    _snprintf(g_ctx.core_name, sizeof(g_ctx.core_name), "%s", cores[g_ctx.core_index].core);
    _snprintf(g_ctx.info.emulator, sizeof(g_ctx.info.emulator), "%s / %s / %s", dev[0].acc_hw, dev[0].device, cores[g_ctx.core_index].core);
    g_ctx.connected = 1;
    g_ctx.info.connected = 1;
    return OS_ERR_OK;
fail:
    mod_disconnect_locked();
    return OS_ERR_NO_DEVICE;
}

static int mod_connect(void)
{
    int rc;
    if (das_ensure_init() != OS_ERR_OK) return OS_ERR_FAIL;
    EnterCriticalSection(&g_ctx.cs);
    rc = mod_connect_locked();
    LeaveCriticalSection(&g_ctx.cs);
    if (rc == OS_ERR_OK)
        LOG(OS_LOG_INFO, "das 模块: 已连接 %s（%s，Attach 不停核）", g_ctx.info.emulator, g_ctx.server_name);
    else
        LOG(OS_LOG_ERROR, "das 模块: 连接失败 %s", g_ctx.last_err);
    return rc;
}

/* ---------- 内存访问 ---------- */
/* 执行一次 txlist（num ≤ DAS_TXLIST_MAX），全部成功返回 0 */
static int das_exec(mcd_tx_st* tx, uint32_t num)
{
    mcd_txlist_st tl;
    tl.tx = tx; tl.num_tx = num; tl.num_tx_ok = 0;
    if (g_ctx.api.execute_txlist(g_ctx.core, &tl) != MCD_RET_ACT_NONE || tl.num_tx_ok != num) {
        das_record_err("txlist");
        return -1;
    }
    return 0;
}

static int das_xfer_locked(uint64_t addr, uint8_t* buf, uint32_t size, int write)
{
    uint64_t ca[DAS_TXLIST_MAX]; uint32_t cl[DAS_TXLIST_MAX];
    mcd_tx_st tx[DAS_TXLIST_MAX];
    int n, i, sfr = das_addr_is_sfr(addr);
    uint32_t done = 0;
    while (done < size) {
        uint32_t remain = size - done;
        uint32_t span = remain > DAS_TX_MAX_BYTES * DAS_TXLIST_MAX ? DAS_TX_MAX_BYTES * DAS_TXLIST_MAX : remain;
        n = das_split_chunks(addr + done, span, DAS_TX_MAX_BYTES, ca, cl, (int)DAS_TXLIST_MAX);
        if (n <= 0) return -1;
        memset(tx, 0, sizeof(tx[0]) * (size_t)n);
        for (i = 0; i < n; i++) {
            tx[i].addr.address = ca[i];
            tx[i].access_type = write ? MCD_TX_AT_W : MCD_TX_AT_R;
            tx[i].access_width = (uint8_t)((sfr && !(ca[i] & 3) && !(cl[i] & 3)) ? 4 : 0);
            tx[i].data = buf + done + (ca[i] - (addr + done));
            tx[i].num_bytes = cl[i];
        }
        if (das_exec(tx, (uint32_t)n) != 0) return -1;
        done += span;
    }
    return 0;
}

/* SFR 非对齐读：扩成 4 字节对齐窗口再截取 */
static int das_read_locked(uint64_t addr, uint8_t* buf, uint32_t size)
{
    if (das_addr_is_sfr(addr) && ((addr & 3) || (size & 3))) {
        uint64_t a; uint32_t s; uint8_t* tmp; int r;
        das_align4(addr, size, &a, &s);
        tmp = (uint8_t*)malloc(s);
        if (!tmp) return -1;
        r = das_xfer_locked(a, tmp, s, 0);
        if (r == 0) memcpy(buf, tmp + (addr - a), size);
        free(tmp);
        return r;
    }
    return das_xfer_locked(addr, buf, size, 0);
}

static int das_try_reconnect(void)
{
    ULONGLONG now = GetTickCount64();
    if (now - g_ctx.last_reconnect_ms < DAS_RECONNECT_MIN_MS) return -1;
    g_ctx.last_reconnect_ms = now;
    LOG(OS_LOG_WARN, "das 模块: 访问失败（%s），尝试重连", g_ctx.last_err);
    return mod_connect_locked() == OS_ERR_OK ? 0 : -1;
}

static int mod_read(OS_MemReq* req)
{
    int r;
    if (!req || !req->data) return OS_ERR_INVALID_ARG;
    if (!g_ctx.connected) return OS_ERR_NOT_CONNECTED;
    if (!req->size) return 0;
    EnterCriticalSection(&g_ctx.cs);
    r = das_read_locked(req->address, (uint8_t*)req->data, req->size);
    if (r != 0 && das_try_reconnect() == 0)
        r = das_read_locked(req->address, (uint8_t*)req->data, req->size);
    LeaveCriticalSection(&g_ctx.cs);
    return r == 0 ? (int)req->size : OS_ERR_TIMEOUT;
}

static int mod_write(OS_MemReq* req)
{
    int r;
    if (!req || !req->data) return OS_ERR_INVALID_ARG;
    if (!g_ctx.connected) return OS_ERR_NOT_CONNECTED;
    if (das_range_is_protected(req->address, req->size)) {
        LOG(OS_LOG_ERROR, "das 模块: 拒绝写 Flash/UCB 区 0x%08llX（%u 字节）", (unsigned long long)req->address, req->size);
        return OS_ERR_INVALID_ARG;
    }
    if (das_addr_is_sfr(req->address) && ((req->address & 3) || (req->size & 3))) {
        LOG(OS_LOG_ERROR, "das 模块: SFR 写需 4 字节对齐 0x%08llX", (unsigned long long)req->address);
        return OS_ERR_INVALID_ARG;
    }
    EnterCriticalSection(&g_ctx.cs);
    r = das_xfer_locked(req->address, (uint8_t*)req->data, req->size, 1);
    LeaveCriticalSection(&g_ctx.cs);
    return r == 0 ? OS_ERR_OK : OS_ERR_TIMEOUT;
}

/* ---------- 运行控制 ---------- */
static int das_run_ctl(int cmd)
{
    mcd_return_et r = MCD_RET_ACT_HANDLE_ERROR;
    uint32_t classes = 0, vec;
    if (!g_ctx.connected) return OS_ERR_NOT_CONNECTED;
    EnterCriticalSection(&g_ctx.cs);
    switch (cmd) {
    case OS_CMD_HALT: r = g_ctx.api.stop(g_ctx.core, FALSE); break;
    case OS_CMD_GO:   r = g_ctx.api.run(g_ctx.core, FALSE); break;
    case OS_CMD_RESET:
    case OS_CMD_RESET_RUN:
        g_ctx.api.qry_rst_classes(g_ctx.core, &classes);
        vec = (classes & 1u) ? 1u : (classes & (0u - classes)); /* 优先 class 0（系统复位） */
        if (!vec) vec = 1u;
        r = g_ctx.api.rst(g_ctx.core, vec, cmd == OS_CMD_RESET ? TRUE : FALSE);
        if (r == MCD_RET_ACT_NONE && cmd == OS_CMD_RESET_RUN) {
            mcd_core_state_st st; memset(&st, 0, sizeof(st));
            if (g_ctx.api.qry_state(g_ctx.core, &st) == MCD_RET_ACT_NONE && st.state != MCD_CORE_STATE_RUNNING)
                g_ctx.api.run(g_ctx.core, FALSE);
        }
        break;
    default: break;
    }
    if (r != MCD_RET_ACT_NONE) das_record_err("run control");
    LeaveCriticalSection(&g_ctx.cs);
    if (r != MCD_RET_ACT_NONE) LOG(OS_LOG_ERROR, "das 模块: %s", g_ctx.last_err);
    return r == MCD_RET_ACT_NONE ? OS_ERR_OK : OS_ERR_FAIL;
}

/* CPU0 CSFR PC：0xF881FE08（CPUx 基址 0xF8810000 + x*0x20000，偏移 0xFE08）；仅停核时有效 */
#define DAS_CSFR_PC(core) (0xF8810000ull + (uint64_t)(core) * 0x20000ull + 0xFE08ull)

static int mod_cpu_state(OS_CpuState* out)
{
    mcd_core_state_st st;
    if (!out) return OS_ERR_INVALID_ARG;
    memset(out, 0, sizeof(*out));
    out->halted = -1; out->powered = -1; out->under_reset = -1; out->voltage_mv = -1;
    if (!g_ctx.connected) { strcpy(out->state, "\xE2\x80\x94"); return OS_ERR_NOT_CONNECTED; }
    memset(&st, 0, sizeof(st));
    EnterCriticalSection(&g_ctx.cs);
    if (g_ctx.api.qry_state(g_ctx.core, &st) != MCD_RET_ACT_NONE) {
        LeaveCriticalSection(&g_ctx.cs);
        strcpy(out->state, "\xE2\x80\x94");
        return OS_ERR_FAIL;
    }
    _snprintf(out->state, sizeof(out->state) - 1, "%s", das_state_text(st.state, &out->halted));
    if (out->halted == 1) {
        uint32_t pc = 0;
        if (das_xfer_locked(DAS_CSFR_PC(g_ctx.core_index), (uint8_t*)&pc, 4, 0) == 0) { out->pc = pc; out->pc_valid = 1; }
    }
    out->powered = 1;
    out->under_reset = 0;
    LeaveCriticalSection(&g_ctx.cs);
    return OS_ERR_OK;
}

static int mod_describe(char* buf)
{
    if (!buf) return OS_ERR_INVALID_ARG;
    sprintf(buf,
        "{\"driver\":\"das\",\"backend\":\"Infineon DAS MCD API (mcdxdas.dll)\","
        "\"targets\":\"AURIX TC2xx/TC3xx via miniWiggler / onboard DAP\","
        "\"connected\":%d,\"server\":\"%s\",\"device\":\"%s\",\"core\":\"%s\","
        "\"attach_running\":true,\"flash\":false,"
        "\"write_protect\":\"0x80000000-0x8FFFFFFF,0xA0000000-0xAFFFFFFF (PFLASH/DFLASH/UCB)\","
        "\"connect_device_field\":\"coreN selects core (default core0)\",\"last_error\":\"%s\"}",
        g_ctx.connected, g_ctx.server_name, g_ctx.device_name, g_ctx.core_name, g_ctx.last_err);
    return OS_ERR_OK;
}

static int mod_command(void* ctx, int cmd, void* in, void* out)
{
    (void)ctx;
    switch (cmd) {
    case OS_CMD_SCAN:         return mod_scan((OS_ScanReq*)in);
    case OS_CMD_CONNECT:
        if (in) memcpy(&g_ctx.cfg, in, sizeof(g_ctx.cfg));
        {
            int rc = mod_connect();
            if (out) *(int*)out = rc;
            return rc;
        }
    case OS_CMD_DISCONNECT:   return mod_disconnect();
    case OS_CMD_IS_CONNECTED: if (out) *(int*)out = g_ctx.connected ? 1 : 0; return OS_ERR_OK;
    case OS_CMD_READ_MEM:     return mod_read((OS_MemReq*)in);
    case OS_CMD_WRITE_MEM:    return mod_write((OS_MemReq*)in);
    case OS_CMD_GET_INFO:
        if (out) memcpy(out, &g_ctx.info, sizeof(g_ctx.info));
        return OS_ERR_OK;
    case OS_CMD_HALT: case OS_CMD_GO: case OS_CMD_RESET: case OS_CMD_RESET_RUN:
        return das_run_ctl(cmd);
    case OS_CMD_GET_CPU_STATE: return mod_cpu_state((OS_CpuState*)out);
    case OS_CMD_AI_DESCRIBE:   return mod_describe((char*)out);
    case OS_CMD_ELF_RELOADED:  return OS_ERR_OK;
    case OS_CMD_FLASH_DOWNLOAD:
    case OS_CMD_FLASH_ERASE:
        LOG(OS_LOG_WARN, "das 模块: 不支持烧录（避免误写 UCB）；请用 Infineon MemTool 或 TASKING dbgtc 烧录 PFLASH");
        return OS_ERR_FAIL;
    default:
        return OS_ERR_FAIL;
    }
}

static int mod_init(const OS_Framework* fw, void** out_ctx)
{
    (void)out_ctx;
    g_fw = fw;
    InitializeCriticalSection(&g_ctx.cs);
    strcpy(g_ctx.info.name, "das");
    strcpy(g_ctx.info.version, "1.0.0");
    g_ctx.info.hw_version = -1;
    g_ctx.info.fw_version = -1;
    /* 延迟绑定：真正 SCAN/CONNECT 时才加载 mcdxdas.dll，主程序启动不受 DAS 影响 */
    return OS_ERR_OK;
}

static void mod_deinit(void* ctx)
{
    (void)ctx;
    mod_disconnect();
    if (g_ctx.bound) {
        if (g_ctx.mcd_inited) g_ctx.api.exit_();
        FreeLibrary(g_ctx.api.h);
    }
    g_ctx.bound = g_ctx.mcd_inited = 0;
    DeleteCriticalSection(&g_ctx.cs);
}

static const OS_Module g_module = {
    OS_API_VERSION,
    OS_CAP_DRIVER,
    "das",
    "1.0.0",
    "Infineon DAS / miniWiggler 驱动模块：AURIX 运行中读写内存（MCD API）",
    NULL,
    mod_init,
    mod_deinit,
    mod_command,
    NULL, NULL, NULL, NULL, NULL, NULL
};

const OS_Module* os_module_get(void)
{
    return &g_module;
}
