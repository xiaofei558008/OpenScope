#include "app.h"
#include "datasrv.h"
#include "datalog.h"
#include "chartwin.h"
#include "numwin.h"
#include "vartree.h"
#include "batchplan.h"
#include "workspace.h"
#include <string.h>

/* Bug10: 边缘目标高速下连接瞬时掉线（mod_read 重连节流 500ms 内读失败累积）。
 * 旧 fail_count>10 硬中断（20ms 周期 × 2 变量 ≈ 100ms 即超限）会把"瞬时掉线（可自愈）"
 * 误判为"连接中断"，在自动重连恢复前退出采集线程。
 * 改为时间基：仅当连续无任何成功样本超过 OS_POLL_STALL_MS 才判定失联停止；
 * 真死连接（重连失败 connected=0）仍由 IS_CONNECTED 即时捕获。 */
#define OS_POLL_STALL_MS 3000

/* F21/Step1 高速采集：自由运行 + 连续地址块读 + UI 刷新节流。
 * 合并预算在 batchplan.h（kHz 时钟，4MHz 空隙 100 字节）。旧公式把 kHz 当成
 * MHz，4MHz 以上一律合并到 4096 字节，空洞读失败会让整块变量一起变成 ret=-5。 */
#define OS_UI_THROTTLE_MS  16

static int os_batch_gap_limit(void)
{
    return os_bp_gap_limit(g_app.speed_khz);
}

static int os_batch_run_limit(void)
{
    return os_bp_run_limit(g_app.speed_khz);
}

/* 观测叶按 (project_id, addr) 排序后的批次项 */
typedef struct {
    int      idx;    /* g_app.leaves 下标 */
    int      project_id;
    uint64_t addr;
    uint32_t size;   /* 实际读取字节（≤64，与单叶读一致） */
} WatchLeaf;

static LARGE_INTEGER g_qpc_freq;
static int           g_qpc_ready;

/* 需求 14 异步传输：专用网络历史环（UI 排空不影响历史完整性，只读不消费） */
#define NET_HIST_CAP 65536
static OS_Sample s_net_hist[NET_HIST_CAP];
static volatile LONG s_net_hist_n; /* 累计样本数（可能 >CAP，用于统计） */

/* 周期计时用 QPC（µs 级、单调），避免 GetSystemTimeAsFileTime 的粗粒度墙钟误差 */
static uint64_t qpc_now_us(void)
{
    LARGE_INTEGER c;
    if (!g_qpc_ready) {
        QueryPerformanceFrequency(&g_qpc_freq);
        g_qpc_ready = 1;
    }
    QueryPerformanceCounter(&c);
    {
        uint64_t f = (uint64_t)g_qpc_freq.QuadPart;
        uint64_t q = (uint64_t)c.QuadPart;
        uint64_t sec = q / f;
        uint64_t rem = q % f;
        return sec * 1000000ULL + rem * 1000000ULL / f;
    }
}

static int cmp_watch_leaf(const void* a, const void* b)
{
    const WatchLeaf* x = (const WatchLeaf*)a;
    const WatchLeaf* y = (const WatchLeaf*)b;
    if (x->project_id < y->project_id) return -1;
    if (x->project_id > y->project_id) return 1;
    if (x->addr < y->addr) return -1;
    if (x->addr > y->addr) return 1;
    return 0;
}

static int acq_is_connected(void)
{
    int i;
    OS_Module* m;
    void* ctx;
    if (g_app.xcp_connected && g_app.xcpmod) return 1;
    if (OS_APP_CONNECTED() && g_app.driver) return 1;
    for (i = 0; i < os_ws()->project_count; i++) {
        if (os_ws_resolve_mod(os_ws_project_at(i), &m, &ctx) == 0)
            return 1;
    }
    return 0;
}

int os_ds_any_connected(void)
{
    return acq_is_connected();
}

#define OS_ACQ_AGAIN_RETRY 3
static unsigned g_acq_again_skips;   /* 采集线程独占使用：驱动忙而跳过的块数（诊断用） */

static int acq_read_mem_mod(OS_Module* mod, void* ctx, OS_MemReq* req)
{
    if (!mod || !mod->command || !req) return OS_ERR_NOT_CONNECTED;
    return mod->command(ctx, OS_CMD_READ_MEM, req, NULL);
}

static int acq_read_mem(OS_MemReq* req)
{
    OS_Module* m = NULL;
    void* ctx = NULL;
    OS_Project* pr = os_ws_active();
    if (!req) return OS_ERR_INVALID_ARG;
    if (pr && os_ws_resolve_mod(pr, &m, &ctx) == 0)
        return acq_read_mem_mod(m, ctx, req);
    if (g_app.xcp_connected && g_app.xcpmod && g_app.xcpmod->command)
        return g_app.xcpmod->command(g_app.xcpmod_ctx, OS_CMD_READ_MEM, req, NULL);
    if (g_app.driver && g_app.driver->command)
        return g_app.driver->command(g_app.driver_ctx, OS_CMD_READ_MEM, req, NULL);
    return OS_ERR_NOT_CONNECTED;
}

static int acq_write_mem(OS_MemReq* req)
{
    OS_Module* m = NULL;
    void* ctx = NULL;
    OS_Project* pr = os_ws_active();
    if (!req) return OS_ERR_INVALID_ARG;
    if (pr && os_ws_resolve_mod(pr, &m, &ctx) == 0)
        return m->command(ctx, OS_CMD_WRITE_MEM, req, NULL);
    if (g_app.xcp_connected && g_app.xcpmod && g_app.xcpmod->command)
        return g_app.xcpmod->command(g_app.xcpmod_ctx, OS_CMD_WRITE_MEM, req, NULL);
    if (g_app.driver && g_app.driver->command)
        return g_app.driver->command(g_app.driver_ctx, OS_CMD_WRITE_MEM, req, NULL);
    return OS_ERR_NOT_CONNECTED;
}

/* 一块连续内存一次读回（指定 Project 模块）。t_cycle_us=本轮 poll 共享时间戳（host-aligned）。 */
static void read_run(WatchLeaf* wl, int first, int count, uint8_t* buf,
                     OS_Sample* batch, int* n,
                     int* fail_count, ULONGLONG* last_ok_ms,
                     OS_Module* mod, void* ctx, int64_t t_cycle_us)
{
    uint64_t run_addr = wl[first].addr;
    uint32_t run_sz = 0;
    OS_MemReq req;
    int k, r;
    for (k = first; k < first + count; k++) {
        uint64_t end64;
        if (wl[k].addr < run_addr) continue;
        end64 = wl[k].addr - run_addr + wl[k].size;
        if (end64 > run_sz) run_sz = (uint32_t)end64;
    }
    if (run_sz == 0 || run_sz > OS_BATCH_RUN_MAX) {
        if ((*fail_count)++ < 3)
            os_log(OS_LOG_ERROR, "块读长度非法 @0x%llX (%u 字节, %d 变量)",
                   (unsigned long long)run_addr, run_sz, count);
        return;
    }
    if (!mod) return;
    memset(buf, 0, run_sz);
    req.address = run_addr;
    req.size = run_sz;
    req.data = buf;
    r = acq_read_mem_mod(mod, ctx, &req);
    {   /* 风险3：OS_ERR_AGAIN = 驱动忙（UI 命令正持会话锁 >50ms，或后台重连中），不是读失败。
         * 采集线程就地重试（最多 OS_ACQ_AGAIN_RETRY 次）；仍忙则本轮跳过该块：不计失败、不打错误日志、
         * 不刷新 last_ok（停摆计时照常工作）。 */
        int tries = 0;
        while (r == OS_ERR_AGAIN && tries++ < OS_ACQ_AGAIN_RETRY && !g_app.stop_poll) {
            Sleep(tries == 1 ? 0 : 2);
            r = acq_read_mem_mod(mod, ctx, &req);
        }
        if (r == OS_ERR_AGAIN) { g_acq_again_skips++; return; }
    }
    if (r != (int)run_sz) {
        if ((*fail_count)++ < 3)
            os_log(OS_LOG_ERROR, "块读 @0x%llX 失败 (ret=%d, %d 变量, proj=%d)",
                   (unsigned long long)run_addr, r, count, wl[first].project_id);
        return;
    }
    *fail_count = 0;
    *last_ok_ms = GetTickCount64();
    for (k = first; k < first + count; k++) {
        OS_Leaf* L = &g_app.leaves[wl[k].idx];
        OS_Sample* s = &batch[(*n)++];
        const uint8_t* rawp = buf + (wl[k].addr - run_addr);
        uint32_t sz = wl[k].size;
        memset(s, 0, sizeof(*s));
        s->ts_us = t_cycle_us > 0 ? t_cycle_us : os_time_us();
        s->var_id = L->id;
        s->address = L->address;
        s->project_id = L->project_id;
        memcpy(s->raw, rawp, sz < 8 ? sz : 8);
        s->size = sz;
        os_format_raw(s->text, sizeof(s->text), rawp, sz, L->kind, L->is_signed, L->is_ptr,
                      L->is_bitfield, L->bit_offset, L->bit_size, &s->value,
                      L->enums, L->enum_count);
    }
}

static DWORD WINAPI poll_thread(LPVOID p)
{
    /* Bug1: 大数组用堆，避免耗尽 1MB 线程栈导致无日志崩溃 */
    OS_Sample* batch = (OS_Sample*)malloc(OS_MAX_LEAVES * sizeof(OS_Sample));
    WatchLeaf* wl = (WatchLeaf*)malloc(OS_MAX_LEAVES * sizeof(WatchLeaf));
    uint8_t* buf = (uint8_t*)malloc(OS_BATCH_RUN_MAX + 1);
    int fail_count = 0;          /* Bug10: 仅用于失败日志节流（前 3 次），不再用于中断 */
    ULONGLONG last_ok_ms = GetTickCount64(); /* 最近一次成功读取时间戳（停摆判定起点） */
    ULONGLONG last_net_push_ms = 0;          /* 需求 14：网络广播节流计时 */
    ULONGLONG cycle_us = qpc_now_us();       /* 自计时：本周期起点 */
    ULONGLONG rate_win_us = cycle_us;        /* 速率统计窗口起点（约每秒一条日志） */
    LONGLONG  cycle_sum_us = 0;
    int       cycle_cnt = 0;
    LONG      rate_samples = 0;
    LONG      rate_tx = 0;                   /* 速率窗口内累计读事务数 */
    (void)p;
    if (!batch || !wl || !buf) {
        os_log(OS_LOG_ERROR, "采集线程内存分配失败");
        os_spool_end(); /* 落盘生命周期归采集线程，失败路径也要收尾 */
        free(batch); free(wl); free(buf);
        g_app.stop_poll = 0;
        g_app.acq_state = OS_ACQ_STOPPED;
        if (g_app.hMain) PostMessage(g_app.hMain, WM_OS_ACQ_STATE, 0, 0);
        return 1;
    }
    while (!g_app.stop_poll) {
        int n = 0, i, nw = 0, run_first;
        int connected = acq_is_connected();
        int tx_count = 0;   /* 本周期读事务次数（合并效果诊断） */
        uint64_t run_addr, run_end;
        if (!connected && GetTickCount64() - last_ok_ms > OS_POLL_STALL_MS) {
            os_log(OS_LOG_ERROR, "采集停止：MCU/XCP 连接已断开");
            break;
        }
        if (GetTickCount64() - last_ok_ms > OS_POLL_STALL_MS) {
            os_log(OS_LOG_ERROR, "采集停止：长时间读取失败（%d 秒无成功样本）",
                   OS_POLL_STALL_MS / 1000);
            break;
        }
        /* 收集观测叶并按地址排序，把连续叶合并成块读（一次读事务）。
         * 合并判定用速度感知成本模型（batchplan.h）：
         * 4MHz 空隙约 100B 就拆分，50MHz 约 1250B。连接暂时丢失时仍走读路径，
         * 驱动会按节流重连；只有连续 OS_POLL_STALL_MS 无成功样本才停线程。 */
        {
            int lc = g_app.leaf_count;
            if (lc < 0) lc = 0;
            if (lc > OS_MAX_LEAVES) lc = OS_MAX_LEAVES;
        for (i = 0; i < lc; i++) {
            OS_Leaf* L = &g_app.leaves[i];
            uint32_t sz;
            if (!L->watched) continue;
            sz = L->size;
            if (sz > 64) sz = 64;
            if (sz < 1) sz = 1;
            wl[nw].idx = i;
            wl[nw].project_id = L->project_id;
            wl[nw].addr = L->address;
            wl[nw].size = sz;
            nw++;
        }
        }
        tx_count = 0;
        if (nw > 0) {
            int gap_lim = os_batch_gap_limit();
            int run_lim = os_batch_run_limit();
            int seg0, seg1;
            int64_t t_cycle = os_time_us(); /* 本周期共享时间戳，便于跨 MCU 对齐对比 */
            qsort(wl, nw, sizeof(WatchLeaf), cmp_watch_leaf);
            /* 按 project_id 分段；段内再按地址合并块读（多 MCU 并行源） */
            seg0 = 0;
            while (seg0 < nw) {
                OS_Module* mod = NULL;
                void* ctx = NULL;
                OS_Project* pr = os_ws_project(wl[seg0].project_id);
                int pid = wl[seg0].project_id;
                for (seg1 = seg0 + 1; seg1 < nw && wl[seg1].project_id == pid; seg1++) {}
                if (!pr || os_ws_resolve_mod(pr, &mod, &ctx) != 0) {
                    /* 禁止回退到其它 MCU，避免串读；跳过本段 */
                    static int skip_log;
                    if (skip_log++ < 8)
                        os_log(OS_LOG_WARN, "采集跳过 project_id=%d（未连接）", pid);
                    seg0 = seg1;
                    continue;
                }
                if (mod) {
                    run_first = seg0;
                    run_addr = wl[seg0].addr;
                    run_end = run_addr + wl[seg0].size;
                    for (i = seg0 + 1; i < seg1; i++) {
                        uint64_t a = wl[i].addr;
                        uint64_t end = a + wl[i].size;
                        if (os_bp_should_split(run_addr, run_end, a, wl[i].size, gap_lim, run_lim)) {
                            read_run(wl, run_first, i - run_first, buf, batch, &n,
                                     &fail_count, &last_ok_ms, mod, ctx, t_cycle);
                            tx_count++;
                            run_first = i;
                            run_addr = a;
                            run_end = end;
                        } else if (end > run_end) {
                            run_end = end;
                        }
                    }
                    read_run(wl, run_first, seg1 - run_first, buf, batch, &n,
                             &fail_count, &last_ok_ms, mod, ctx, t_cycle);
                    tx_count++;
                }
                seg0 = seg1;
            }
        }
        if (n > 0) {
            os_ds_push_batch(batch, n);
            /* 需求 14：网络广播节流 ~30ms（本线程直接调网络模块，避免跨线程消息开销） */
            {
                ULONGLONG now_push = GetTickCount64();
                if (g_app.netmod && g_app.netmod->command &&
                    now_push - last_net_push_ms >= 30) {
                    g_app.netmod->command(g_app.netmod_ctx, OS_CMD_NET_PUSH, NULL, NULL);
                    last_net_push_ms = now_push;
                }
            }
        }
        /* 自计时：poll_interval_ms>0 保持定时补睡，否则自由运行（周期=实际耗时） */
        if (g_app.poll_interval_ms > 0) {
            ULONGLONG now = qpc_now_us();
            ULONGLONG want = (ULONGLONG)g_app.poll_interval_ms * 1000;
            if (now - cycle_us < want)
                Sleep((DWORD)((want - (now - cycle_us)) / 1000));
        } else if (nw == 0 || (n == 0 && !connected)) {
            /* 无观测变量，或本轮没读到样本且连接仍掉：避免忙等空转。
             * 重连节流在驱动里（约 500ms），这里只让出 CPU。 */
            Sleep(20);
        }
        /* 速率统计（约每秒一条） */
        {
            ULONGLONG now = qpc_now_us();
            cycle_sum_us += (LONGLONG)(now - cycle_us);
            cycle_cnt++;
            rate_samples += n;
            rate_tx += tx_count;
            if (now - rate_win_us >= 1000000ULL) {
                os_log(OS_LOG_INFO, "采集速率: %ld 样本/s，周期 %.0f µs（%d 个变量，%ld 次读/周期）",
                       rate_samples,
                       cycle_cnt ? (double)cycle_sum_us / cycle_cnt : 0.0,
                       g_app.watch_count,
                       cycle_cnt ? rate_tx / cycle_cnt : 0);
                rate_win_us = now;
                cycle_sum_us = 0;
                cycle_cnt = 0;
                rate_samples = 0;
                rate_tx = 0;
            }
            cycle_us = now;
        }
    }
    /* Bug11: 正常停止（stop_poll 置位）为信息级；异常退出（断连/停摆）才是警告 */
    if (g_app.stop_poll)
        os_log(OS_LOG_INFO, "采集线程已退出");
    else
        os_log(OS_LOG_WARN, "采集线程已退出");
    /* 落盘收尾在本线程执行（采集线程独占 spool）：避免 UI 线程 os_spool_end 与
     * 本线程在途 os_spool_push 并发 free/写盘导致的 use-after-free 闪退 */
    os_spool_end();
    free(batch); free(wl); free(buf);
    g_app.stop_poll = 0;
    g_app.acq_state = OS_ACQ_STOPPED;
    if (g_app.hMain) PostMessage(g_app.hMain, WM_OS_ACQ_STATE, 0, 0);
    return 0;
}

/* Bug4/C3: os_ds_stop 等待超时后采集线程可能仍卡在驱动调用里；不再 CloseHandle 后假装已停，
 * 而是记住它，未退出前拒绝重新开始（否则两个采集线程并发读、并发写 spool/状态）。 */
static HANDLE g_poll_stale;
#define OS_DS_STOP_WAIT_MS 200

/* Bug4（续）：有界等待残留采集线程（退出程序 / 卸载驱动前调用）。返回 1 = 已无残留线程 */
int os_ds_join(DWORD wait_ms)
{
    if (g_poll_stale) WaitForSingleObject(g_poll_stale, wait_ms);
    return !os_ds_poll_busy();
}

int os_ds_poll_busy(void)
{
    if (!g_poll_stale) return 0;
    if (WaitForSingleObject(g_poll_stale, 0) == WAIT_OBJECT_0) {
        CloseHandle(g_poll_stale);
        g_poll_stale = NULL;
        return 0;
    }
    return 1;
}

int os_ds_start(void)
{
    int conn;
    if (g_app.acq_state == OS_ACQ_RUNNING) return 0;
    if (os_ds_poll_busy()) {
        os_log(OS_LOG_WARN, "上一次采集线程仍阻塞在驱动调用中（探针无响应/重连中），请稍后再开始采集");
        return -1;
    }
    conn = acq_is_connected();
    if (!conn) {
        if (!g_app.driver && !g_app.xcpmod)
            os_log(OS_LOG_ERROR, "无探针驱动 / XCP 模块，无法采集");
        else
            os_log(OS_LOG_WARN, "请先连接探针或 XCP，再开始采集");
        return -1;
    }
    if (g_app.watch_count <= 0) {
        os_log(OS_LOG_WARN, "没有观测变量，请先将变量添加到波形/数值窗口");
        return -1;
    }
    g_app.stop_poll = 0;
    g_app.acq_state = OS_ACQ_RUNNING;
    /* 每次重新开始采集先清上次临时缓存；仅在录制开启时建立新缓存 */
    os_spool_clear();
    if (g_app.recording) {
        if (os_spool_begin() != 0) {
            g_app.acq_state = OS_ACQ_STOPPED;
            os_log(OS_LOG_ERROR, "录制缓存建立失败");
            return -1;
        }
    }
    g_app.hPoll = CreateThread(NULL, 0, poll_thread, NULL, 0, NULL);
    if (!g_app.hPoll) {
        g_app.acq_state = OS_ACQ_STOPPED;
        os_spool_end();
        os_log(OS_LOG_ERROR, "创建采集线程失败");
        return -1;
    }
    if (g_app.poll_interval_ms > 0)
        os_log(OS_LOG_INFO, "采集已开始（Workspace 多源，周期 %d ms，%d 个变量）",
               g_app.poll_interval_ms, g_app.watch_count);
    else
        os_log(OS_LOG_INFO, "采集已开始（Workspace 多源，自由运行，%d 个变量）",
               g_app.watch_count);
    return 0;
}

void os_ds_stop(void)
{
    if (g_app.acq_state != OS_ACQ_RUNNING) return;
    g_app.stop_poll = 1;
    if (g_app.hPoll) {
        /* Bug4（续）：驱动侧已改为不在调用线程重连、UI 命令限时，采集线程正常应在一个读周期内退出；
         * UI 线程最多等 OS_DS_STOP_WAIT_MS，超时不关句柄，交给 os_ds_poll_busy/os_ds_join 回收 */
        if (WaitForSingleObject(g_app.hPoll, OS_DS_STOP_WAIT_MS) == WAIT_TIMEOUT) {
            os_log(OS_LOG_WARN, "采集线程未立即退出（驱动调用中），后台继续等待其退出");
            if (g_poll_stale) CloseHandle(g_poll_stale);
            g_poll_stale = g_app.hPoll;
        } else {
            CloseHandle(g_app.hPoll);
        }
        g_app.hPoll = NULL;
    }
    g_app.acq_state = OS_ACQ_STOPPED;
    os_ds_drain(); /* F21/Step1: UI 节流后环内可能残留末批样本，停止时一次性排空 */
    /* os_spool_end 已移到采集线程自身（poll_thread 退出前收尾），此处不再调用，
     * 避免与本线程 3s 等待超时后仍可能在途的写盘竞态 */
    os_log(OS_LOG_INFO, "采集已停止");
}

void os_ds_push_batch(OS_Sample* samples, int n)
{
    int i;
    static ULONGLONG s_last_ui_ms; /* F21/Step1: UI 排空节流（~60Hz），环内样本全收 */
    if (n <= 0 || !samples) return;
    if (g_app.recording) os_spool_push(samples, n);
    EnterCriticalSection(&g_app.ring_cs);
    for (i = 0; i < n; i++) {
        g_app.ring[g_app.ring_head] = samples[i];
        g_app.ring_head = (g_app.ring_head + 1) % OS_RING_CAP;
        if (g_app.ring_head == g_app.ring_tail)
            g_app.ring_tail = (g_app.ring_tail + 1) % OS_RING_CAP;
    }
    LeaveCriticalSection(&g_app.ring_cs);
    for (i = 0; i < n; i++) {
        if (samples[i].var_id >= 0 && samples[i].var_id < g_app.leaf_count)
            g_app.leaves[samples[i].var_id].sample = samples[i];
        /* 需求 14：网络历史环（异步回传用，UI 排空不影响） */
        {
            LONG k = InterlockedIncrement(&s_net_hist_n) - 1;
            s_net_hist[k % NET_HIST_CAP] = samples[i];
        }
    }
    InterlockedAdd(&g_app.total_samples, n);
    if (g_app.log_csv) os_datalog_append();
    {
        ULONGLONG now = GetTickCount64();
        if (g_app.hMain && now - s_last_ui_ms >= OS_UI_THROTTLE_MS) {
            s_last_ui_ms = now;
            PostMessage(g_app.hMain, WM_OS_SAMPLES, (WPARAM)n, 0);
        }
    }
}

/* Bug1 同类风险：UI 线程栈上大数组（OS_MAX_LEAVES × ~96B ≈ 384KB），
 * 嵌套在 WM_PAINT/WM_OS_SAMPLES 下可能逼近 1MB 默认栈上限。
 * 每次排空只需 ~256 条即可维持 60Hz UI 刷新；环内剩余样本下个周期排。 */
#define OS_DRAIN_BATCH 256

void os_ds_drain(void)
{
    OS_Sample batch[OS_DRAIN_BATCH];
    int n = 0, i, j;
    EnterCriticalSection(&g_app.ring_cs);
    while (g_app.ring_tail != g_app.ring_head && n < OS_DRAIN_BATCH) {
        batch[n++] = g_app.ring[g_app.ring_tail];
        g_app.ring_tail = (g_app.ring_tail + 1) % OS_RING_CAP;
    }
    LeaveCriticalSection(&g_app.ring_cs);
    if (n <= 0) return;
    /* Bug19: 推送给每个 tab 内的全部窗口（group[]），而不是只有 group[0]——
     * 旧实现同 tab 多窗口时只有第一个窗口更新/绘图 */
    for (i = 0; i < g_app.win_count; i++) {
        int k;
        if (g_app.wins[i].is_module) continue;
        for (k = 0; k < g_app.wins[i].group_count; k++) {
            HWND w = g_app.wins[i].group[k];
            if (!w || !IsWindow(w)) continue;
            for (j = 0; j < n; j++) {
                os_chart_push(w, &batch[j]);
                os_num_push(w, &batch[j]);
            }
        }
    }
    for (i = 0; i < g_app.winmod_count; i++) {
        OS_Module* m = g_app.winmods[i];
        if (m->on_samples) m->on_samples(g_app.winmod_ctx[i], batch, n);
    }
}

int os_ds_write_leaf(int id, const char* text, char* err, int errlen)
{
    const OS_Leaf* L;
    uint8_t raw[64];
    int n = 0;
    OS_MemReq wr;
    int r;
    if (errlen > 0 && err) err[0] = 0;
    L = os_vartree_leaf(id);
    if (!L) { if (err && errlen) _snprintf(err, errlen, "变量 ID 无效"); return -1; }
    if (!acq_is_connected()) {
        /* 需求 14：本机无探针/XCP（远端显示侧）→ 写值经网络转发到探针侧执行 */
        if (g_app.netmod && g_app.netmod->command) {
            OS_NetWriteReq nreq;
            int rc;
            memset(&nreq, 0, sizeof(nreq));
            _snprintf(nreq.name, sizeof(nreq.name), "%s", L->name);
            _snprintf(nreq.value, sizeof(nreq.value), "%s", text);
            rc = g_app.netmod->command(g_app.netmod_ctx, OS_CMD_NET_WRITE, &nreq, NULL);
            if (rc == OS_ERR_OK) {
                os_log(OS_LOG_INFO, "网络写入 %s = %s 成功", L->name, text);
                return 0;
            }
            if (err && errlen) _snprintf(err, errlen, "网络写入失败 (rc=%d)", rc);
            return -1;
        }
        if (err && errlen) _snprintf(err, errlen, "未连接探针或 XCP");
        return -1;
    }
    memset(raw, 0, sizeof(raw));
    if (L->is_bitfield) {
        /* 读-改-写 */
        uint8_t cur[8];
        uint64_t storage, mask, v;
        OS_MemReq rr;
        memset(cur, 0, 8);
        rr.address = L->address;
        rr.size = L->size;
        rr.data = cur;
        r = acq_read_mem(&rr);
        if (r != (int)L->size) {
            if (err && errlen) _snprintf(err, errlen, "位域读回失败 (ret=%d)", r);
            return -1;
        }
        if (!os_parse_text(text, raw, 8, &n, L->kind, L->is_signed, 1,
                           L->bit_offset, L->bit_size, L->enums, L->enum_count)) {
            if (err && errlen) _snprintf(err, errlen, "数值解析失败");
            return -1;
        }
        storage = 0;
        memcpy(&storage, cur, L->size < 8 ? L->size : 8);
        v = 0;
        memcpy(&v, raw, 8);
        mask = ((L->bit_size >= 64) ? ~0ULL : ((1ULL << L->bit_size) - 1)) << L->bit_offset;
        storage = (storage & ~mask) | (v & mask);
        memcpy(raw, &storage, L->size < 8 ? L->size : 8);
        n = (int)L->size;
    } else {
        int maxsz = (int)L->size;
        if (maxsz > 64) maxsz = 64;
        if (!os_parse_text(text, raw, maxsz, &n, L->kind, L->is_signed, 0, 0, 0,
                           L->enums, L->enum_count)) {
            if (err && errlen) _snprintf(err, errlen, "数值解析失败");
            return -1;
        }
    }
    wr.address = L->address;
    wr.size = (uint32_t)n;
    wr.data = raw;
    r = acq_write_mem(&wr);
    if (r != OS_ERR_OK) {
        /* 注：本机有驱动时写失败不转发网络——否则探针侧写失败会与远端
         * 互相回传形成乒乓循环；只有纯远端显示侧（无驱动）才走网络转发。 */
        if (err && errlen) _snprintf(err, errlen, "写入失败 (err=%d)", r);
        return -1;
    }
    /* 回读产生样本 */
    {
        uint8_t rb[64];
        OS_MemReq rr;
        OS_Sample s;
        memset(rb, 0, sizeof(rb));
        rr.address = L->address;
        rr.size = (uint32_t)n;
        rr.data = rb;
        r = acq_read_mem(&rr);
        memset(&s, 0, sizeof(s));
        s.ts_us = os_time_us();
        s.var_id = L->id;
        s.address = L->address;
        s.written = 1;
        if (r == n) {
            memcpy(s.raw, rb, n < 8 ? n : 8);
            s.size = n;
            os_format_raw(s.text, sizeof(s.text), rb, n, L->kind, L->is_signed, L->is_ptr,
                          L->is_bitfield, L->bit_offset, L->bit_size, &s.value,
                          L->enums, L->enum_count);
        } else {
            _snprintf(s.text, sizeof(s.text), "写入完成（回读失败 ret=%d）", r);
        }
        os_ds_push_batch(&s, 1);
    }
    os_log(OS_LOG_INFO, "写入 %s = %s", L->name, text);
    return 0;
}

/* 需求 14：网络模块把远端样本注入采集通道（环 + 窗口显示）。 */
void os_fw_push_sample(const OS_Sample* s)
{
    if (s) os_ds_push_batch((OS_Sample*)s, 1);
}

/* ---- 需求 14：框架 v4 回调（网络模块驱动采集/读取历史） ---- */

int os_fw_leaf_watched(int id)
{
    if (id < 0 || id >= g_app.leaf_count) return 0;
    return g_app.leaves[id].watched ? 1 : 0;
}

int os_fw_set_watch(int id, int on)
{
    LONG prev;
    if (id < 0 || id >= g_app.leaf_count) return -1;
    prev = InterlockedExchange(&g_app.leaves[id].watched, on ? 1 : 0);
    if ((prev ? 1 : 0) != (on ? 1 : 0)) {
        /* 仅状态翻转时重算观测数并记日志（网络 WATCH_LIST 高频重建不再刷屏） */
        int i, wc = 0;
        for (i = 0; i < g_app.leaf_count; i++)
            if (g_app.leaves[i].watched) wc++;
        g_app.watch_count = wc;
        os_log(OS_LOG_INFO, "网络勾选: %s -> %s", g_app.leaves[id].name, on ? "观测" : "取消");
    }
    return 0;
}

int os_fw_acq_start(void)
{
    return os_ds_start();
}

void os_fw_acq_stop(void)
{
    os_ds_stop();
}

/* 复制采集历史（最近 max 个，旧→新），返回实际复制数。异步历史回传用。 */
int os_fw_ring_copy(OS_Sample* out, int max)
{
    LONG total = InterlockedCompareExchange(&s_net_hist_n, 0, 0); /* 只读快照 */
    int start, i, n = 0, cnt;
    if (!out || max <= 0 || total <= 0) return 0;
    cnt = total > NET_HIST_CAP ? NET_HIST_CAP : (int)total;
    if (cnt > max) cnt = max;
    start = (int)((total - cnt) % NET_HIST_CAP);
    if (start < 0) start += NET_HIST_CAP;
    for (i = 0; i < cnt; i++) {
        out[n++] = s_net_hist[(start + i) % NET_HIST_CAP];
    }
    return n;
}
