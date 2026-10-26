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
 *  - REG_READ：停核时经 CSFR 读 D0-15/A0-15/PC/PSW/PCXI（局部变量求值用）
 *  - BP_SET / BP_CLEAR / BP_LIST：MCD 触发器硬件断点（最多 8，按 qry_trig_info 收紧）；
 *    STEP/STEP_IN/STEP_OVER：指令单步；STEP_OUT：临时断点于 A11 返回地址
 *  - FLASH_DOWNLOAD / FLASH_ERASE：不支持（返回 OS_ERR_FAIL 并提示用 MemTool / TASKING dbgtc），
 *    以杜绝误写 UCB/保护配置的风险。
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#if !defined(_WIN32) && !defined(_vsnprintf)
#define _vsnprintf vsnprintf /* 非 Windows 测试桩 */
#endif
#include "module_api.h"
#include "mcdload.h"
#include "das_util.h"
#include "drv_guard.h"

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
    volatile LONG connected;       /* Bug4：写入一律 InterlockedExchange（持锁）；锁外读用 DAS_LOAD */
    volatile LONG want_connected;  /* Bug4：锁外读写，一律原子访问 */   /* 用户已连接且未主动断开：访问失败/掉线后按节流自动重连（B4/B5） */
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

/* ---------- Bug4：锁规则 ----------
 * 顺序固定：g_ctx.cs → g_mcd_cs（MCD 全局锁，串行化对 mcdxdas.dll / tas_server 的调用）。
 * 任何路径都不得先拿 g_mcd_cs 再拿 g_ctx.cs。
 * 重连线程不持 g_ctx.cs；它只按“单次 MCD 调用”短暂持 g_mcd_cs（不跨越整个 open 序列），
 * 换入句柄时才 das_lock()。 */
#define DAS_LOAD(x)     InterlockedCompareExchange(&(x), 0, 0)
#define DAS_STORE(x, v) InterlockedExchange(&(x), (LONG)(v))
static OS_DrvGuard g_guard;
static CRITICAL_SECTION g_mcd_cs;

static void das_lock(void)   { EnterCriticalSection(&g_ctx.cs); EnterCriticalSection(&g_mcd_cs); }
static void das_unlock(void) { LeaveCriticalSection(&g_mcd_cs); LeaveCriticalSection(&g_ctx.cs); }
static void das_mcd_enter(void) { EnterCriticalSection(&g_mcd_cs); }
static void das_mcd_leave(void) { LeaveCriticalSection(&g_mcd_cs); }

/* UI 路径：重连中或 ~50ms 内拿不到 cs+MCD 锁 → OS_ERR_AGAIN。
 * 两把锁共用一个 QPC 截止时刻（原先各自计时 + Sleep(1)，Windows 上最坏 50+15.6+… ms）。 */
static int das_lock_ui(void)
{
    LONGLONG dl = drv_deadline((DWORD)g_guard.ui_lock_ms);
    if (drv_lock_until(&g_guard, dl) != OS_ERR_OK) return OS_ERR_AGAIN;
    if (drv_try_cs_until(NULL, &g_mcd_cs, dl, 0)) return OS_ERR_OK;
    LeaveCriticalSection(&g_ctx.cs);
    return OS_ERR_AGAIN;
}

#define DAS_BP_HW_MAX 8
typedef struct { uint32_t addr; uint32_t trig_id; int armed; int temp; } DasBp;
static DasBp g_bp[DAS_BP_HW_MAX];
static int   g_bp_limit = DAS_BP_HW_MAX;
static void das_bp_rearm_locked(void);
static void das_bp_del_locked(int i);
static int  das_bp_supported(void);

/* MSVC _snprintf 截断时不写 NUL：统一用 das_fmt（cap-1 + 手动终止）（B7） */
static void das_fmt(char* dst, size_t cap, const char* fmt, ...)
{
    va_list ap;
    if (!dst || !cap) return;
    va_start(ap, fmt);
    _vsnprintf(dst, cap - 1, fmt, ap);
    va_end(ap);
    dst[cap - 1] = 0;
}

#define LOG(lv, ...) do { if (g_fw && g_fw->log) g_fw->log((lv), __VA_ARGS__); } while (0)

static void das_record_err(const char* what)
{
    mcd_error_info_st e;
    memset(&e, 0, sizeof(e));
    if (g_ctx.bound) g_ctx.api.qry_error_info(g_ctx.core, &e);
    das_fmt(g_ctx.last_err, sizeof(g_ctx.last_err), "%s: 0x%x %s", what, e.error_code, e.error_str);
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
        das_fmt(g_ctx.info.dll_version, sizeof(g_ctx.info.dll_version), "%u.%u.%u",
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
    memset(si, 0, sizeof(si));
    das_lock();
    if (das_ensure_init() != OS_ERR_OK) { das_unlock(); return OS_ERR_FAIL; }
    das_unlock();
    das_mcd_enter();   /* Bug4：枚举服务器可能较慢，只持 MCD 锁，不持会话锁 */
    if (g_ctx.api.qry_servers("localhost", TRUE, 0, &n, si) != MCD_RET_ACT_NONE) n = 0;
    das_mcd_leave();
    for (i = 0; i < n && i < DAS_MAX_SERVERS && req->count < req->capacity; i++) {
        OS_DeviceInfo* d = &req->items[req->count];
        memset(d, 0, sizeof(*d));
        d->index = (int)i;
        das_fmt(d->serial, sizeof(d->serial), "%s", si[i].server);
        das_fmt(d->name, sizeof(d->name), "DAS %s: %s", si[i].server,
                  si[i].acc_hw[0] ? si[i].acc_hw : "(no access HW)");
        req->count++;
    }
    return req->count > 0 ? OS_ERR_OK : OS_ERR_NO_DEVICE;
}

/* ---------- 连接 ---------- */

/* Bug4：会话在锁外打开（局部变量），成功后再 das_lock() 换入 */
typedef struct DasSess {
    mcd_server_st* server;
    mcd_core_st* core;
    int core_index;
    char server_name[64], acc_hw[256], device_name[64], core_name[64], emulator[256];
    char err[256];
} DasSess;

static void das_sess_err(DasSess* x, const char* what)
{
    mcd_error_info_st e;
    memset(&e, 0, sizeof(e));
    das_mcd_enter();
    g_ctx.api.qry_error_info(x->core, &e);
    das_mcd_leave();
    das_fmt(x->err, sizeof(x->err), "%s: 0x%x %s", what, e.error_code, e.error_str);
}

static void das_sess_close(DasSess* x)
{
    if (x->core)   { das_mcd_enter(); g_ctx.api.close_core(x->core);     das_mcd_leave(); x->core = NULL; }
    if (x->server) { das_mcd_enter(); g_ctx.api.close_server(x->server); das_mcd_leave(); x->server = NULL; }
}

/* 不持 g_ctx.cs；每个 MCD 调用单独持 g_mcd_cs。 */
static int das_sess_open(const OS_ConnectCfg* cfg_in, DasSess* x)
{
    char cfg[256];
    mcd_core_con_info_st sys, dev[4], cores[8];
    uint32_t n;
    mcd_return_et r;
    const char* srv = cfg_in->serial[0] ? cfg_in->serial : DAS_SERVER_DEFAULT;
    memset(x, 0, sizeof(*x));
    das_fmt(cfg, sizeof(cfg), "McdHostName=\"localhost\"\nMcdServerName=\"%s\"\n", srv);
    /* system_key 必须是 ""：传 NULL 时 mcdxdas.dll 访问冲突 */
    das_mcd_enter(); r = g_ctx.api.open_server("", cfg, &x->server); das_mcd_leave();
    if ((r != MCD_RET_ACT_NONE || !x->server) && strcmp(srv, DAS_SERVER_DEFAULT) != 0) {
        /* serial 不是有效 DAS 服务器名（如残留的其它探针序列号）：回退默认 UDAS */
        srv = DAS_SERVER_DEFAULT;
        das_fmt(cfg, sizeof(cfg), "McdHostName=\"localhost\"\nMcdServerName=\"%s\"\n", srv);
        x->server = NULL;
        das_mcd_enter(); g_ctx.api.open_server("", cfg, &x->server); das_mcd_leave();
    }
    if (!x->server) { das_sess_err(x, "open_server"); return OS_ERR_NO_DEVICE; }
    memset(&sys, 0, sizeof(sys)); n = 1;
    das_mcd_enter(); r = g_ctx.api.qry_systems(0, &n, &sys); das_mcd_leave();
    if (r != MCD_RET_ACT_NONE || n == 0) { das_sess_err(x, "qry_systems"); goto fail; }
    memset(dev, 0, sizeof(dev)); n = 4;
    das_mcd_enter(); r = g_ctx.api.qry_devices(&sys, 0, &n, dev); das_mcd_leave();
    if (r != MCD_RET_ACT_NONE || n == 0) { das_sess_err(x, "qry_devices(目标未上电/未连接?)"); goto fail; }
    memset(cores, 0, sizeof(cores)); n = 8;
    das_mcd_enter(); r = g_ctx.api.qry_cores(&dev[0], 0, &n, cores); das_mcd_leave();
    if (r != MCD_RET_ACT_NONE || n == 0) { das_sess_err(x, "qry_cores"); goto fail; }
    x->core_index = das_parse_core_index(cfg_in->device);
    if ((uint32_t)x->core_index >= n) x->core_index = 0;
    das_mcd_enter(); r = g_ctx.api.open_core(&cores[x->core_index], &x->core); das_mcd_leave();
    if (r != MCD_RET_ACT_NONE || !x->core) { x->core = NULL; das_sess_err(x, "open_core"); goto fail; }
    das_fmt(x->server_name, sizeof(x->server_name), "%s", srv);
    das_fmt(x->acc_hw, sizeof(x->acc_hw), "%s", dev[0].acc_hw);
    das_fmt(x->device_name, sizeof(x->device_name), "%s", dev[0].device);
    das_fmt(x->core_name, sizeof(x->core_name), "%s", cores[x->core_index].core);
    das_fmt(x->emulator, sizeof(x->emulator), "%s / %s / %s", dev[0].acc_hw, dev[0].device, cores[x->core_index].core);
    return OS_ERR_OK;
fail:
    das_sess_close(x);
    return OS_ERR_NO_DEVICE;
}

/* 持锁（das_lock）调用：换入新会话；旧句柄由调用方事先摘下 */
static void das_sess_install_locked(DasSess* x)
{
    g_ctx.server = x->server; g_ctx.core = x->core; g_ctx.core_index = x->core_index;
    x->server = NULL; x->core = NULL;
    das_fmt(g_ctx.server_name, sizeof(g_ctx.server_name), "%s", x->server_name);
    das_fmt(g_ctx.acc_hw, sizeof(g_ctx.acc_hw), "%s", x->acc_hw);
    das_fmt(g_ctx.device_name, sizeof(g_ctx.device_name), "%s", x->device_name);
    das_fmt(g_ctx.core_name, sizeof(g_ctx.core_name), "%s", x->core_name);
    das_fmt(g_ctx.info.emulator, sizeof(g_ctx.info.emulator), "%s", x->emulator);
    DAS_STORE(g_ctx.connected, 1);
    g_ctx.info.connected = 1;
    das_bp_rearm_locked();   /* 少量触发器调用，持锁时间短 */
}

/* 持锁调用：把当前会话句柄摘下来（之后在锁外关闭） */
static void das_sess_detach_locked(DasSess* old)
{
    memset(old, 0, sizeof(*old));
    old->server = g_ctx.server; old->core = g_ctx.core;
    g_ctx.server = NULL; g_ctx.core = NULL;
    DAS_STORE(g_ctx.connected, 0);
    g_ctx.info.connected = 0;
    { int k; for (k = 0; k < DAS_BP_HW_MAX; k++) g_bp[k].armed = 0; }  /* 旧 core 的 trig_id 随之失效 */
}

/* 打开新会话并换入（不持锁执行慢操作）。调用方已是 g_guard 独占者。 */
static int das_reopen(const char* why)
{
    DasSess old, x;
    OS_ConnectCfg cfg;
    int rc, want;
    das_lock();
    cfg = g_ctx.cfg;
    das_sess_detach_locked(&old);
    das_unlock();
    das_sess_close(&old);                      /* 锁外关闭旧句柄 */
    if (why) LOG(OS_LOG_WARN, "das 模块: %s，后台重连（不持会话锁）", why);
    rc = das_sess_open(&cfg, &x);              /* 慢：open_server/qry/open_core，锁外 */
    das_lock();
    want = (int)DAS_LOAD(g_ctx.want_connected);
    if (rc == OS_ERR_OK && want) das_sess_install_locked(&x);
    else if (rc != OS_ERR_OK) das_fmt(g_ctx.last_err, sizeof(g_ctx.last_err), "%s", x.err);
    das_unlock();
    if (rc == OS_ERR_OK && !want) { das_sess_close(&x); rc = OS_ERR_CANCELED; }  /* 期间用户已断开 */
    return rc;
}

/* 后台重连线程入口（drv_guard 调用；不持任何锁进入） */
static int das_reconnect_worker(void)
{
    int rc;
    if (!DAS_LOAD(g_ctx.want_connected)) return OS_ERR_CANCELED;
    rc = das_reopen("访问失败");
    if (rc == OS_ERR_OK && !DAS_LOAD(g_ctx.want_connected)) {
        /* 换入后用户恰好断开（mod_disconnect 拿不到锁时只置了标志）：在此收尾 */
        DasSess old;
        das_lock(); das_sess_detach_locked(&old); das_unlock();
        das_sess_close(&old);
        return OS_ERR_CANCELED;
    }
    if (rc == OS_ERR_OK) LOG(OS_LOG_INFO, "das 模块: 已重连 %s", g_ctx.info.emulator);
    return rc;
}

/* 访问失败/未连接时调用（不持锁）：want_connected 才请求后台重连。总是立即返回。 */
static int das_kick(void)
{
    if (!DAS_LOAD(g_ctx.want_connected)) return OS_ERR_NOT_CONNECTED;
    return drv_kick_reconnect(&g_guard);   /* OS_ERR_AGAIN = 重连进行中 */
}

static int mod_disconnect(void)
{
    DasSess old;
    DAS_STORE(g_ctx.want_connected, 0);            /* 先置位：后台重连换入前会看到并放弃 */
    if (!g_ctx.bound) return OS_ERR_OK;
    if (drv_lock_timed(&g_guard, 2000) != OS_ERR_OK) {
        /* 重连进行中或会话卡死：不等。重连线程看到 want_connected=0 会自行关闭新会话 */
        LOG(OS_LOG_WARN, "das 模块: 断开时会话忙，已标记断开（后台收尾）");
        return OS_ERR_OK;
    }
    EnterCriticalSection(&g_mcd_cs);
    if (g_ctx.connected && das_bp_supported()) { int k; for (k = 0; k < DAS_BP_HW_MAX; k++) if (g_bp[k].addr) das_bp_del_locked(k); }
    das_sess_detach_locked(&old);
    das_unlock();
    das_sess_close(&old);                /* 锁外关闭 */
    return OS_ERR_OK;
}

static int mod_connect(void)
{
    int rc;
    if (drv_begin_exclusive(&g_guard) != OS_ERR_OK) {
        LOG(OS_LOG_WARN, "das 模块: 正在后台重连，请稍后再连接");
        return OS_ERR_AGAIN;
    }
    das_lock();
    rc = das_ensure_init();
    if (rc == OS_ERR_OK) DAS_STORE(g_ctx.want_connected, 1);
    das_unlock();
    if (rc == OS_ERR_OK) {
        rc = das_reopen(NULL);
        if (rc != OS_ERR_OK) { das_lock(); DAS_STORE(g_ctx.want_connected, 0); das_unlock(); }
    } else rc = OS_ERR_FAIL;
    drv_end_exclusive(&g_guard);
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
    int n, i;
    uint32_t done = 0;
    if (!das_range_fits32(addr, size)) return -1;
    while (done < size) {
        uint32_t remain = size - done;
        uint32_t span = remain > DAS_TX_MAX_BYTES * DAS_TXLIST_MAX ? DAS_TX_MAX_BYTES * DAS_TXLIST_MAX : remain;
        n = das_split_chunks(addr + done, span, DAS_TX_MAX_BYTES, ca, cl, (int)DAS_TXLIST_MAX);
        if (n <= 0) return -1;
        memset(tx, 0, sizeof(tx[0]) * (size_t)n);
        for (i = 0; i < n; i++) {
            tx[i].addr.address = ca[i];
            tx[i].access_type = write ? MCD_TX_AT_W : MCD_TX_AT_R;
            /* SFR 判定按块（B8）：块可能从 RAM 跨入 0xF0000000 以上 */
            int sfr = das_addr_is_sfr(ca[i]) || das_addr_is_sfr(ca[i] + cl[i] - 1);
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
    if (!das_range_fits32(addr, size)) return -1;
    if (das_addr_is_sfr(addr + size - 1) && ((addr & 3) || (size & 3))) {
        uint64_t a, s64; uint32_t s; uint8_t* tmp; int r;
        das_align4(addr, size, &a, &s64);
        if (s64 > 0xFFFFFFFFull || s64 < size || !das_range_fits32(a, (uint32_t)s64)) return -1;
        s = (uint32_t)s64;
        tmp = (uint8_t*)malloc(s);
        if (!tmp) return -1;
        r = das_xfer_locked(a, tmp, s, 0);
        if (r == 0) memcpy(buf, tmp + (addr - a), size);
        free(tmp);
        return r;
    }
    return das_xfer_locked(addr, buf, size, 0);
}

/* Bug4：读路径永不在调用线程里重连。≤OS_DRV_SMALL_IO 的读（UI 局部变量等）走限时锁。 */
static int das_io_lock(uint32_t size)
{
    if (size <= OS_DRV_SMALL_IO) return das_lock_ui();
    if (drv_lock_worker(&g_guard) != OS_ERR_OK) return OS_ERR_AGAIN;
    EnterCriticalSection(&g_mcd_cs);
    return OS_ERR_OK;
}

static int mod_read(OS_MemReq* req)
{
    int r;
    if (!req || !req->data) return OS_ERR_INVALID_ARG;
    if (!req->size) return 0;
    if (req->size > (uint32_t)INT_MAX) return OS_ERR_INVALID_ARG; /* 返回值为 int 字节数 */
    if (!das_range_fits32(req->address, req->size)) return OS_ERR_INVALID_ARG; /* B1 */
    if (das_io_lock(req->size) != OS_ERR_OK) return OS_ERR_AGAIN;
    if (!g_ctx.connected) { das_unlock(); return das_kick(); }
    r = das_read_locked(req->address, (uint8_t*)req->data, req->size);
    das_unlock();
    if (r != 0) {
        /* 原先这里持锁重连（open_server 可达数秒）→ UI 卡死。改为请求后台重连，本次返回失败 */
        if (das_kick() == OS_ERR_AGAIN) return OS_ERR_AGAIN;
        return OS_ERR_TIMEOUT;
    }
    return (int)req->size;
}

/* 寄存器（停核时）：CSFR 映射 D0..D15 @+0xFF00、A0..A15 @+0xFF80、PCXI/PSW/PC @+0xFE00/04/08 */
static int das_reg_read(OS_RegSet* rs)
{
    mcd_core_state_st st;
    uint32_t d[16], a[16], sys[3];
    uint64_t base;
    int i;
    if (!rs) return OS_ERR_INVALID_ARG;
    memset(rs, 0, sizeof(*rs));
    rs->arch = OS_ARCH_TRICORE;
    if (das_lock_ui() != OS_ERR_OK) return OS_ERR_AGAIN;   /* Bug4: 重连中/锁忙 → 不等 */
    if (!g_ctx.connected) { das_unlock(); return OS_ERR_NOT_CONNECTED; }
    memset(&st, 0, sizeof(st));
    if (g_ctx.api.qry_state(g_ctx.core, &st) != MCD_RET_ACT_NONE) { das_unlock(); return OS_ERR_FAIL; }
    if (st.state == MCD_CORE_STATE_RUNNING) { das_unlock(); return OS_ERR_BUSY; }
    base = 0xF8810000ull + (uint64_t)g_ctx.core_index * 0x20000ull;
    if (das_xfer_locked(base + 0xFF00u, (uint8_t*)d, sizeof(d), 0) != 0 ||
        das_xfer_locked(base + 0xFF80u, (uint8_t*)a, sizeof(a), 0) != 0 ||
        das_xfer_locked(base + 0xFE00u, (uint8_t*)sys, sizeof(sys), 0) != 0) {
        das_unlock();
        return OS_ERR_FAIL;
    }
    das_unlock();
    for (i = 0; i < 16; i++) {
        rs->value[i] = d[i]; rs->valid[i] = 1; das_fmt(rs->name[i], sizeof(rs->name[i]), "D%d", i);
        rs->value[16 + i] = a[i]; rs->valid[16 + i] = 1; das_fmt(rs->name[16 + i], sizeof(rs->name[16 + i]), "A%d", i);
    }
    rs->value[40] = sys[2]; rs->valid[40] = 1; das_fmt(rs->name[40], 8, "PC");
    rs->value[41] = sys[1]; rs->valid[41] = 1; das_fmt(rs->name[41], 8, "PSW");
    rs->value[42] = sys[0]; rs->valid[42] = 1; das_fmt(rs->name[42], 8, "PCXI");
    rs->count = 43;
    return OS_ERR_OK;
}

static int mod_write(OS_MemReq* req)
{
    int r;
    if (!req || !req->data) return OS_ERR_INVALID_ARG;
    if (!das_range_fits32(req->address, req->size)) return OS_ERR_INVALID_ARG; /* B1 */
    if (das_range_is_protected(req->address, req->size)) {  /* size=0 也按 1 字节判保护 */
        LOG(OS_LOG_ERROR, "das 模块: 拒绝写 Flash/UCB 区 0x%08llX（%u 字节）", (unsigned long long)req->address, req->size);
        return OS_ERR_INVALID_ARG;
    }
    if (!req->size) return OS_ERR_OK;
    if ((das_addr_is_sfr(req->address) || das_addr_is_sfr(req->address + req->size - 1)) &&
        ((req->address & 3) || (req->size & 3))) {
        LOG(OS_LOG_ERROR, "das 模块: SFR 写需 4 字节对齐 0x%08llX", (unsigned long long)req->address);
        return OS_ERR_INVALID_ARG;
    }
    if (das_io_lock(req->size) != OS_ERR_OK) return OS_ERR_AGAIN;
    if (!g_ctx.connected) { das_unlock(); return das_kick(); }
    r = das_xfer_locked(req->address, (uint8_t*)req->data, req->size, 1);
    das_unlock();
    if (r != 0) {   /* B5：失败 → 后台重连，不在此线程重连 */
        if (das_kick() == OS_ERR_AGAIN) return OS_ERR_AGAIN;
        return OS_ERR_TIMEOUT;
    }
    return OS_ERR_OK;
}


/* ---------- 硬件断点（MCD 触发器，TriCore OCDS TR0..TR7 指令地址比较）---------- */

static int das_bp_supported(void)
{
    return g_ctx.api.create_trig && g_ctx.api.remove_trig && g_ctx.api.activate_trig_set;
}

/* TriCore 指令按半字对齐；PFLASH 0x8xxxxxxx(cached)/0xAxxxxxxx(non-cached) 是同一物理地址，
 * 取指走 ELF 链接地址（通常 0x8xxxxxxx），断点按宿主给的地址原样比较，只清 bit0。 */
static int das_bp_arm_locked(DasBp* b)
{
    mcd_trig_simple_core_st t;
    uint32_t id = 0;
    memset(&t, 0, sizeof(t));
    t.struct_size = sizeof(t);
    t.type = MCD_TRIG_TYPE_IP;
    t.option = MCD_TRIG_OPT_IMPL_HARDWARE;
    t.action = MCD_TRIG_ACTION_DBG_DEBUG;
    t.addr_start.address = b->addr;
    t.addr_range = 0;
    if (g_ctx.api.create_trig(g_ctx.core, &t, &id) != MCD_RET_ACT_NONE) { das_record_err("create_trig"); return -1; }
    b->trig_id = id;
    if (g_ctx.api.activate_trig_set(g_ctx.core) != MCD_RET_ACT_NONE) {
        das_record_err("activate_trig_set");
        g_ctx.api.remove_trig(g_ctx.core, id);
        return -1;
    }
    b->armed = 1;
    return 0;
}

static void das_bp_disarm_locked(DasBp* b)
{
    if (b->armed && g_ctx.core) {
        g_ctx.api.remove_trig(g_ctx.core, b->trig_id);
        g_ctx.api.activate_trig_set(g_ctx.core);
    }
    b->armed = 0;
}

/* 连接/重连后重新布置（重新 open_core 后旧 trig_id 失效） */
static void das_bp_rearm_locked(void)
{
    int i;
    mcd_trig_info_st ti;
    g_bp_limit = DAS_BP_HW_MAX;
    if (!das_bp_supported()) return;
    memset(&ti, 0, sizeof(ti));
    if (g_ctx.api.qry_trig_info && g_ctx.api.qry_trig_info(g_ctx.core, &ti) == MCD_RET_ACT_NONE &&
        ti.trig_number > 0 && ti.trig_number < DAS_BP_HW_MAX)
        g_bp_limit = (int)ti.trig_number;
    for (i = 0; i < DAS_BP_HW_MAX; i++) {
        g_bp[i].armed = 0;
        if (g_bp[i].addr && das_bp_arm_locked(&g_bp[i]) != 0)
            LOG(OS_LOG_WARN, "das 断点: 重连后恢复 0x%08X 失败（%s）", g_bp[i].addr, g_ctx.last_err);
    }
}

static int das_bp_find(uint32_t addr)
{
    int i;
    for (i = 0; i < DAS_BP_HW_MAX; i++) if (g_bp[i].addr && g_bp[i].addr == addr) return i;
    return -1;
}

static int das_bp_count(void)
{
    int i, n = 0;
    for (i = 0; i < DAS_BP_HW_MAX; i++) if (g_bp[i].addr) n++;
    return n;
}

/* 返回槽号；-1 失败（last_err），-2 槽满 */
static int das_bp_add_locked(uint32_t addr, int temp)
{
    int i = das_bp_find(addr), slot = -1;
    if (i >= 0) { if (!temp) g_bp[i].temp = 0; return i; }
    if (das_bp_count() >= g_bp_limit) return -2;
    for (i = 0; i < DAS_BP_HW_MAX; i++) if (!g_bp[i].addr) { slot = i; break; }
    if (slot < 0) return -2;
    g_bp[slot].addr = addr; g_bp[slot].temp = temp;
    if (das_bp_arm_locked(&g_bp[slot]) != 0) { memset(&g_bp[slot], 0, sizeof(g_bp[slot])); return -1; }
    return slot;
}

static void das_bp_del_locked(int i)
{
    das_bp_disarm_locked(&g_bp[i]);
    memset(&g_bp[i], 0, sizeof(g_bp[i]));
}

static int das_bp_cmd(int cmd, void* in, void* out)
{
    int rc = OS_ERR_OK, i;
    const OS_BpReq* req = (const OS_BpReq*)in;
    if (das_lock_ui() != OS_ERR_OK) return OS_ERR_AGAIN;   /* Bug4: 重连中/锁忙 → 不等 */
    if (!g_ctx.connected) { das_unlock(); return OS_ERR_NOT_CONNECTED; }
    if (!das_bp_supported()) {
        das_unlock();
        LOG(OS_LOG_ERROR, "das 断点: mcdxdas.dll 未导出触发器 API，不支持硬件断点");
        return OS_ERR_UNSUPPORTED;
    }
    if (cmd == OS_CMD_BP_SET) {
        uint32_t addr;
        if (!req || !(addr = req->address & ~1u)) { das_unlock(); return OS_ERR_INVALID_ARG; }
        i = das_bp_add_locked(addr, 0);
        if (i == -2) {
            rc = OS_ERR_BUSY;
            LOG(OS_LOG_ERROR, "das 断点: 硬件断点槽已满（最多 %d）", g_bp_limit);
        } else if (i < 0) {
            rc = OS_ERR_FAIL;
            LOG(OS_LOG_ERROR, "das 断点: 设置 0x%08X 失败：%s", addr, g_ctx.last_err);
        } else {
            LOG(OS_LOG_INFO, "das 断点: 设置 0x%08X slot=%d trig=%u（%d/%d）", addr, i, g_bp[i].trig_id, das_bp_count(), g_bp_limit);
        }
    } else if (cmd == OS_CMD_BP_CLEAR) {
        if (!req) rc = OS_ERR_INVALID_ARG;
        else if (req->clear_all) {
            for (i = 0; i < DAS_BP_HW_MAX; i++) if (g_bp[i].addr) das_bp_del_locked(i);
            LOG(OS_LOG_INFO, "das 断点: 已全部清除");
        } else {
            i = das_bp_find(req->address & ~1u);
            if (i < 0) rc = OS_ERR_FAIL;
            else { das_bp_del_locked(i); LOG(OS_LOG_INFO, "das 断点: 清除 0x%08X", req->address & ~1u); }
        }
    } else { /* LIST */
        OS_BpList* list = (OS_BpList*)out;
        if (!list || !list->items || list->capacity <= 0) rc = OS_ERR_INVALID_ARG;
        else {
            list->count = 0;
            for (i = 0; i < DAS_BP_HW_MAX && list->count < list->capacity; i++) {
                if (!g_bp[i].addr || g_bp[i].temp) continue;
                list->items[list->count].address = g_bp[i].addr;
                list->items[list->count].id = i;
                list->items[list->count].enabled = g_bp[i].armed;
                list->count++;
            }
        }
    }
    das_unlock();
    return rc;
}

/* 单步：MCD 指令单步（需已停核）。STEP_OVER 迭代1 等同单步（与 stlink 一致）。 */
static int das_step(void)
{
    mcd_return_et r;
    mcd_core_state_st st;
    if (das_lock_ui() != OS_ERR_OK) return OS_ERR_AGAIN;   /* Bug4: 重连中/锁忙 → 不等 */
    if (!g_ctx.connected) { das_unlock(); return OS_ERR_NOT_CONNECTED; }
    memset(&st, 0, sizeof(st));
    if (g_ctx.api.qry_state(g_ctx.core, &st) == MCD_RET_ACT_NONE && st.state == MCD_CORE_STATE_RUNNING) {
        das_unlock();
        return OS_ERR_BUSY;
    }
    r = g_ctx.api.step(g_ctx.core, FALSE, MCD_CORE_STEP_TYPE_INSTR, 1);
    if (r != MCD_RET_ACT_NONE) das_record_err("step");
    das_unlock();
    if (r != MCD_RET_ACT_NONE) { LOG(OS_LOG_ERROR, "das 单步失败: %s", g_ctx.last_err); return OS_ERR_FAIL; }
    return OS_ERR_OK;
}

/* 步出：TriCore 返回地址在 A11（CSFR 0xFFAC）。设临时断点于 A11 并运行；命中后宿主轮询到停核，
 * 下一次 GO/BP 操作前在 cpu_state 里清临时断点。 */
#define DAS_CSFR_A11(core) (0xF8810000ull + (uint64_t)(core) * 0x20000ull + 0xFFACull)
static int das_step_out(void)
{
    uint32_t ra = 0;
    int i;
    if (das_lock_ui() != OS_ERR_OK) return OS_ERR_AGAIN;   /* Bug4: 重连中/锁忙 → 不等 */
    if (!g_ctx.connected) { das_unlock(); return OS_ERR_NOT_CONNECTED; }
    if (!das_bp_supported()) { das_unlock(); return OS_ERR_UNSUPPORTED; }
    if (das_xfer_locked(DAS_CSFR_A11(g_ctx.core_index), (uint8_t*)&ra, 4, 0) != 0 || !ra) {
        das_unlock();
        return OS_ERR_FAIL;
    }
    i = das_bp_add_locked(ra & ~1u, 1);
    if (i < 0) { das_unlock(); return i == -2 ? OS_ERR_BUSY : OS_ERR_FAIL; }
    g_ctx.api.run(g_ctx.core, FALSE);
    das_unlock();
    LOG(OS_LOG_INFO, "das 步出: 运行到返回地址 0x%08X", ra & ~1u);
    return OS_ERR_OK;
}

/* 停核后清掉临时（步出）断点 */
static void das_bp_drop_temp_locked(void)
{
    int i;
    for (i = 0; i < DAS_BP_HW_MAX; i++) if (g_bp[i].addr && g_bp[i].temp) das_bp_del_locked(i);
}

/* ---------- 运行控制 ---------- */
static int das_run_ctl(int cmd)
{
    mcd_return_et r = MCD_RET_ACT_HANDLE_ERROR;
    uint32_t classes = 0, vec;
    if (das_lock_ui() != OS_ERR_OK) return OS_ERR_AGAIN;   /* Bug4: 重连中/锁忙 → 不等 */
    if (!g_ctx.connected) { das_unlock(); return OS_ERR_NOT_CONNECTED; }
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
    das_unlock();
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
    memset(&st, 0, sizeof(st));
    if (das_lock_ui() != OS_ERR_OK) { strcpy(out->state, "BUSY"); return OS_ERR_AGAIN; }   /* Bug4: 重连中/锁忙 → 不等 */
    if (!g_ctx.connected) {
        das_unlock();
        strcpy(out->state, "\xE2\x80\x94");
        return OS_ERR_NOT_CONNECTED;
    }
    if (g_ctx.api.qry_state(g_ctx.core, &st) != MCD_RET_ACT_NONE) {
        das_unlock();
        strcpy(out->state, "\xE2\x80\x94");
        return OS_ERR_FAIL;
    }
    das_fmt(out->state, sizeof(out->state), "%s", das_state_text(st.state, &out->halted));
    if (out->halted == 1) {
        uint32_t pc = 0;
        das_bp_drop_temp_locked();
        if (das_xfer_locked(DAS_CSFR_PC(g_ctx.core_index), (uint8_t*)&pc, 4, 0) == 0) { out->pc = pc; out->pc_valid = 1; }
    }
    out->powered = 1;
    out->under_reset = 0;
    das_unlock();
    return OS_ERR_OK;
}

/* JSON 字符串转义（B6） */
/* 有效 UTF-8 序列长度（1..4），无效返回 0（含过长编码、代理区、> U+10FFFF、截断） */
static int das_utf8_len(const unsigned char* p)
{
    unsigned char c = p[0];
    if (c < 0x80) return 1;
    if (c >= 0xC2 && c <= 0xDF) return (p[1] & 0xC0) == 0x80 ? 2 : 0;
    if (c >= 0xE0 && c <= 0xEF) {
        if ((p[1] & 0xC0) != 0x80 || (p[2] & 0xC0) != 0x80) return 0;
        if (c == 0xE0 && p[1] < 0xA0) return 0;   /* 过长 */
        if (c == 0xED && p[1] >= 0xA0) return 0;  /* 代理区 */
        return 3;
    }
    if (c >= 0xF0 && c <= 0xF4) {
        if ((p[1] & 0xC0) != 0x80 || (p[2] & 0xC0) != 0x80 || (p[3] & 0xC0) != 0x80) return 0;
        if (c == 0xF0 && p[1] < 0x90) return 0;
        if (c == 0xF4 && p[1] >= 0x90) return 0;
        return 4;
    }
    return 0;
}

/* JSON 字符串转义（B6）：校验 UTF-8，无效字节替换为 U+FFFD；截断时不切断多字节字符 */
static void das_json_esc(const char* in, char* out, size_t cap)
{
    size_t o = 0;
    const unsigned char* p = (const unsigned char*)in;
    if (!out || !cap) return;
    while (p && *p) {
        unsigned char ch = *p;
        char tmp[8]; size_t n; int L;
        if (ch == '"' || ch == '\\') { tmp[0] = '\\'; tmp[1] = (char)ch; n = 2; L = 1; }
        else if (ch < 0x20) { n = (size_t)sprintf(tmp, "\\u%04x", ch); L = 1; }
        else if ((L = das_utf8_len(p)) > 0) { memcpy(tmp, p, (size_t)L); n = (size_t)L; }
        else { tmp[0] = (char)0xEF; tmp[1] = (char)0xBF; tmp[2] = (char)0xBD; n = 3; L = 1; }
        if (o + n >= cap) break;   /* 整个单元放不下就停止，不切半个字符 */
        memcpy(out + o, tmp, n); o += n; p += L;
    }
    out[o] = 0;
}

/* AI_DESCRIBE：调用方缓冲约定 ≥4KB（module_api.h）；按 4096 截断 */
#define DAS_DESCRIBE_CAP 4096
static int mod_describe(char* buf)
{
    char srv[160], dev[160], core[160], err[600];
    if (!buf) return OS_ERR_INVALID_ARG;
    if (das_lock_ui() != OS_ERR_OK) { das_fmt(buf, DAS_DESCRIBE_CAP, "{\"driver\":\"das\",\"busy\":true}"); return OS_ERR_OK; }
    das_json_esc(g_ctx.server_name, srv, sizeof(srv));
    das_json_esc(g_ctx.device_name, dev, sizeof(dev));
    das_json_esc(g_ctx.core_name, core, sizeof(core));
    das_json_esc(g_ctx.last_err, err, sizeof(err));
    das_fmt(buf, DAS_DESCRIBE_CAP,
        "{\"driver\":\"das\",\"backend\":\"Infineon DAS MCD API (mcdxdas.dll)\","
        "\"targets\":\"AURIX TC2xx/TC3xx via miniWiggler / onboard DAP\","
        "\"connected\":%d,\"server\":\"%s\",\"device\":\"%s\",\"core\":\"%s\","
        "\"attach_running\":true,\"flash\":false,"
        "\"write_protect\":\"0x80000000-0x8FFFFFFF,0xA0000000-0xAFFFFFFF (PFLASH/DFLASH/UCB)\","
        "\"connect_device_field\":\"coreN selects core (default core0)\",\"last_error\":\"%s\"}",
        (int)g_ctx.connected, srv, dev, core, err);
    das_unlock();
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
    case OS_CMD_IS_CONNECTED:
        if (out) { *(int*)out = DAS_LOAD(g_ctx.connected) ? 1 : 0; /* Bug4: 原子读，不拿锁 */ }
        return OS_ERR_OK;
    case OS_CMD_READ_MEM:     return mod_read((OS_MemReq*)in);
    case OS_CMD_WRITE_MEM:    return mod_write((OS_MemReq*)in);
    case OS_CMD_GET_INFO:
        if (out) { if (das_lock_ui() != OS_ERR_OK) return OS_ERR_AGAIN; memcpy(out, &g_ctx.info, sizeof(g_ctx.info)); das_unlock(); }
        return OS_ERR_OK;
    case OS_CMD_HALT: case OS_CMD_GO: case OS_CMD_RESET: case OS_CMD_RESET_RUN:
        return das_run_ctl(cmd);
    case OS_CMD_GET_CPU_STATE: return mod_cpu_state((OS_CpuState*)out);
    case OS_CMD_STEP: case OS_CMD_STEP_IN: case OS_CMD_STEP_OVER:
        return das_step();
    case OS_CMD_STEP_OUT:      return das_step_out();
    case OS_CMD_BP_SET: case OS_CMD_BP_CLEAR: case OS_CMD_BP_LIST:
        return das_bp_cmd(cmd, in, out);
    case OS_CMD_REG_READ:      return das_reg_read((OS_RegSet*)out);
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
    InitializeCriticalSection(&g_mcd_cs);
    drv_guard_init(&g_guard, &g_ctx.cs, das_reconnect_worker, DAS_RECONNECT_MIN_MS);
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
    DAS_STORE(g_ctx.want_connected, 0);
    if (!drv_guard_shutdown(&g_guard, 5000)) {
        /* 后台重连卡在 mcdxdas.dll 里：不能 FreeLibrary/删锁（线程仍在用），宁可泄漏 */
        LOG(OS_LOG_WARN, "das 模块: 后台重连 5s 未结束，跳过 MCD 卸载");
        return;
    }
    mod_disconnect();
    if (g_ctx.bound) {
        if (g_ctx.mcd_inited) g_ctx.api.exit_();
        FreeLibrary(g_ctx.api.h);
    }
    g_ctx.bound = g_ctx.mcd_inited = 0;
    DeleteCriticalSection(&g_mcd_cs);
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
