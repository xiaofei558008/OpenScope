#include "app.h"
#include "datasrv.h"
#include "datalog.h"
#include "chartwin.h"
#include "numwin.h"
#include "vartree.h"
#include <string.h>

/* Bug10: 边缘目标高速下连接瞬时掉线（mod_read 重连节流 500ms 内读失败累积）。
 * 旧 fail_count>10 硬中断（20ms 周期 × 2 变量 ≈ 100ms 即超限）会把"瞬时掉线（可自愈）"
 * 误判为"连接中断"，在自动重连恢复前退出采集线程。
 * 改为时间基：仅当连续无任何成功样本超过 OS_POLL_STALL_MS 才判定失联停止；
 * 真死连接（重连失败 connected=0）仍由 IS_CONNECTED 即时捕获。 */
#define OS_POLL_STALL_MS 3000

/* F21/Step1 高速采集：自由运行 + 连续地址块读 + UI 刷新节流。
 * 瓶颈分析（J-Link Pro V4 实测）：每次 JLINKARM_ReadMem 事务固定开销 ~240µs（USB 往返
 * + DLL 协议），线上传输时间 = 字节数×10bits/速度。SWD 时钟只影响后者——小尺寸读时
 * 线上时间仅几 µs，所以 4MHz→12MHz 对单事务几乎无感；真正决定周期的是【事务次数】。
 *
 * 速度感知合并（成本模型）：两叶间隔 G 字节，合并 = 1 次事务 + 多读 G 字节垃圾；
 * 拆分 = 2 次事务。合并划算条件：G×10/speed_khz ≤ 事务开销。
 *   gap_limit = speed_khz × 25（事务开销按 250µs 计）
 *   4MHz → 100B；12MHz → 300B；50MHz → 1250B（上限 4096）。
 * 块读总长上限同理按线上时间预算：run_limit = speed_khz × 25（250µs 线上预算，下限 256）。 */
#define OS_TX_OVERHEAD_US  250    /* 实测单事务开销（USB 往返 + DLL 协议） */
#define OS_MERGE_WIRE_BIT  10     /* 每字节线上位成本（含协议开销，约 10bit/字节） */
#define OS_BATCH_RUN_MIN   256    /* 块读长度下限（过小失去合并意义） */
#define OS_BATCH_RUN_MAX   4096   /* 块读长度硬上限（跨无效内存风险） */
#define OS_UI_THROTTLE_MS  16

static int os_batch_gap_limit(void)
{
    int s = g_app.speed_khz > 0 ? g_app.speed_khz : 4000;
    int lim = s * OS_TX_OVERHEAD_US / OS_MERGE_WIRE_BIT;
    if (lim < 64) lim = 64;
    if (lim > OS_BATCH_RUN_MAX) lim = OS_BATCH_RUN_MAX;
    return lim;
}

static int os_batch_run_limit(void)
{
    int s = g_app.speed_khz > 0 ? g_app.speed_khz : 4000;
    int lim = s * OS_TX_OVERHEAD_US / OS_MERGE_WIRE_BIT;
    if (lim < OS_BATCH_RUN_MIN) lim = OS_BATCH_RUN_MIN;
    if (lim > OS_BATCH_RUN_MAX) lim = OS_BATCH_RUN_MAX;
    return lim;
}

/* 观测叶按地址排序后的批次项（高速模式收集表） */
typedef struct {
    int      idx;    /* g_app.leaves 下标 */
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
    if (x->addr < y->addr) return -1;
    if (x->addr > y->addr) return 1;
    return 0; /* 同址叶（位域/别名/联合成员）：共享字节一次读出 */
}

/* 一块连续内存一次读回，按叶偏移切分产出样本。
 * Bug10: 块读瞬时失败（mod_read 自动重连可自愈）只节流记日志，不中断采集；
 * 成功时更新 last_ok_ms 供停摆判定。 */
static void read_run(WatchLeaf* wl, int first, int count, uint8_t* buf,
                     OS_Sample* batch, int* n,
                     int* fail_count, ULONGLONG* last_ok_ms)
{
    uint64_t run_addr = wl[first].addr;
    uint32_t run_sz = 0;
    OS_MemReq req;
    int k, r;
    for (k = first; k < first + count; k++) {
        uint32_t end = (uint32_t)(wl[k].addr - run_addr + wl[k].size);
        if (end > run_sz) run_sz = end;
    }
    memset(buf, 0, run_sz);
    req.address = run_addr;
    req.size = run_sz;
    req.data = buf;
    r = g_app.driver->command(g_app.driver_ctx, OS_CMD_READ_MEM, &req, NULL);
    if (r != (int)run_sz) {
        if ((*fail_count)++ < 3)
            os_log(OS_LOG_ERROR, "块读 @0x%llX 失败 (ret=%d, %d 变量)",
                   (unsigned long long)run_addr, r, count);
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
        s->ts_us = os_time_us();
        s->var_id = L->id;
        s->address = L->address;
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
        int connected = 0;
        int tx_count = 0;   /* 本周期 J-Link 读事务次数（合并效果诊断） */
        uint64_t run_addr, run_end;
        if (g_app.driver && g_app.driver->command) {
            g_app.driver->command(g_app.driver_ctx, OS_CMD_IS_CONNECTED, NULL, &connected);
        }
        if (!connected) {
            os_log(OS_LOG_ERROR, "采集停止：MCU 连接已断开");
            break;
        }
        if (GetTickCount64() - last_ok_ms > OS_POLL_STALL_MS) {
            os_log(OS_LOG_ERROR, "采集停止：长时间读取失败（%d 秒无成功样本）",
                   OS_POLL_STALL_MS / 1000);
            break;
        }
        /* 收集观测叶并按地址排序，把连续叶合并成块读（一次读事务）。
         * 合并判定用速度感知成本模型（os_batch_gap_limit/run_limit）：
         * 50MHz 时跨 1KB 空隙合并仍划算，4MHz 时 100B 以上就拆分为妙。 */
        for (i = 0; i < g_app.leaf_count; i++) {
            OS_Leaf* L = &g_app.leaves[i];
            uint32_t sz;
            if (!L->watched) continue;
            sz = L->size;
            if (sz > 64) sz = 64;
            if (sz < 1) sz = 1;
            wl[nw].idx = i;
            wl[nw].addr = L->address;
            wl[nw].size = sz;
            nw++;
        }
        tx_count = 0;
        if (nw > 0) {
            int gap_lim = os_batch_gap_limit();
            int run_lim = os_batch_run_limit();
            qsort(wl, nw, sizeof(WatchLeaf), cmp_watch_leaf);
            run_first = 0;
            run_addr = wl[0].addr;
            run_end = run_addr + wl[0].size;
            for (i = 1; i < nw; i++) {
                uint64_t a = wl[i].addr;
                uint64_t end = a + wl[i].size;
                if (a > run_end + (uint64_t)gap_lim ||
                    end - run_addr > (uint64_t)run_lim) {
                    read_run(wl, run_first, i - run_first, buf, batch, &n,
                             &fail_count, &last_ok_ms);
                    tx_count++;
                    run_first = i;
                    run_addr = a;
                    run_end = end;
                } else if (end > run_end) {
                    run_end = end;
                }
            }
            read_run(wl, run_first, nw - run_first, buf, batch, &n,
                     &fail_count, &last_ok_ms);
            tx_count++;
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
        } else if (nw == 0) {
            Sleep(20); /* 全部取消勾选时避免忙等空转 */
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

int os_ds_start(void)
{
    int conn = 0;
    if (g_app.acq_state == OS_ACQ_RUNNING) return 0;
    if (!g_app.driver || !g_app.driver->command) {
        os_log(OS_LOG_ERROR, "无驱动模块，无法采集");
        return -1;
    }
    /* 网络驱动的远端实例可能无硬件连接：快速失败，避免启动注定退出的采集线程 */
    g_app.driver->command(g_app.driver_ctx, OS_CMD_IS_CONNECTED, NULL, &conn);
    if (!conn) {
        os_log(OS_LOG_WARN, "MCU 未连接（%s），无法开始采集", g_app.driver->name);
        return -1;
    }
    if (g_app.watch_count <= 0) {
        os_log(OS_LOG_WARN, "没有观测变量，请先在左侧变量树勾选");
        return -1;
    }
    g_app.stop_poll = 0;
    g_app.acq_state = OS_ACQ_RUNNING;
    os_spool_begin(); /* 长时间采集自动落盘：RAM ≤10MB → 时间戳 CSV */
    g_app.hPoll = CreateThread(NULL, 0, poll_thread, NULL, 0, NULL);
    if (!g_app.hPoll) {
        g_app.acq_state = OS_ACQ_STOPPED;
        os_spool_end();
        os_log(OS_LOG_ERROR, "创建采集线程失败");
        return -1;
    }
    if (g_app.poll_interval_ms > 0)
        os_log(OS_LOG_INFO, "采集已开始（周期 %d ms，%d 个变量）",
               g_app.poll_interval_ms, g_app.watch_count);
    else
        os_log(OS_LOG_INFO, "采集已开始（自由运行高速模式，%d 个变量）",
               g_app.watch_count);
    return 0;
}

void os_ds_stop(void)
{
    if (g_app.acq_state != OS_ACQ_RUNNING) return;
    g_app.stop_poll = 1;
    if (g_app.hPoll) {
        WaitForSingleObject(g_app.hPoll, 3000);
        CloseHandle(g_app.hPoll);
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
    os_spool_push(samples, n); /* 长时间采集自动落盘（RAM ≤10MB → CSV） */
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
    if (!g_app.driver || !g_app.driver->command) {
        /* 需求 14：本机无探针驱动（远端显示侧）→ 写值经网络转发到探针侧执行 */
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
        if (err && errlen) _snprintf(err, errlen, "无驱动模块");
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
        r = g_app.driver->command(g_app.driver_ctx, OS_CMD_READ_MEM, &rr, NULL);
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
    r = g_app.driver->command(g_app.driver_ctx, OS_CMD_WRITE_MEM, &wr, NULL);
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
        r = g_app.driver->command(g_app.driver_ctx, OS_CMD_READ_MEM, &rr, NULL);
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
