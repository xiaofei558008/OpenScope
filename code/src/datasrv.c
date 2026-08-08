#include "app.h"
#include "datasrv.h"
#include "datalog.h"
#include "chartwin.h"
#include "numwin.h"
#include "vartree.h"
#include <string.h>

static DWORD WINAPI poll_thread(LPVOID p)
{
    OS_Sample batch[OS_MAX_LEAVES];
    int fail_count = 0;
    (void)p;
    while (!g_app.stop_poll) {
        int n = 0, i;
        int connected = 0;
        if (g_app.driver && g_app.driver->command) {
            g_app.driver->command(g_app.driver_ctx, OS_CMD_IS_CONNECTED, NULL, &connected);
        }
        if (!connected) {
            os_log(OS_LOG_ERROR, "采集停止：MCU 连接已断开");
            break;
        }
        for (i = 0; i < g_app.leaf_count && n < OS_MAX_LEAVES; i++) {
            OS_Leaf* L = &g_app.leaves[i];
            uint8_t buf[64];
            uint32_t sz;
            OS_MemReq req;
            int r;
            OS_Sample* s;
            if (!L->watched) continue;
            sz = L->size;
            if (sz > 64) sz = 64;
            if (sz < 1) sz = 1;
            memset(buf, 0, sizeof(buf));
            req.address = L->address;
            req.size = sz;
            req.data = buf;
            r = g_app.driver->command(g_app.driver_ctx, OS_CMD_READ_MEM, &req, NULL);
            if (r != (int)sz) {
                fail_count++;
                if (fail_count <= 3)
                    os_log(OS_LOG_ERROR, "读取 %s @0x%llX 失败 (ret=%d)",
                           L->name, (unsigned long long)L->address, r);
                if (fail_count > 10) break;
                continue;
            }
            fail_count = 0;
            s = &batch[n++];
            memset(s, 0, sizeof(*s));
            s->ts_us = os_time_us();
            s->var_id = L->id;
            s->address = L->address;
            memcpy(s->raw, buf, sz < 8 ? sz : 8);
            s->size = sz;
            os_format_raw(s->text, sizeof(s->text), buf, sz, L->kind, L->is_signed, L->is_ptr,
                          L->is_bitfield, L->bit_offset, L->bit_size, &s->value,
                          L->enums, L->enum_count);
        }
        if (n > 0) {
            os_ds_push_batch(batch, n);
        }
        if (fail_count > 10) break;
        Sleep((DWORD)g_app.poll_interval_ms);
    }
    g_app.stop_poll = 0;
    g_app.acq_state = OS_ACQ_STOPPED;
    os_log(OS_LOG_WARN, "采集线程已退出");
    if (g_app.hMain) PostMessage(g_app.hMain, WM_OS_ACQ_STATE, 0, 0);
    return 0;
}

int os_ds_start(void)
{
    if (g_app.acq_state == OS_ACQ_RUNNING) return 0;
    if (!g_app.driver || !g_app.driver->command) {
        os_log(OS_LOG_ERROR, "无驱动模块，无法采集");
        return -1;
    }
    if (g_app.watch_count <= 0) {
        os_log(OS_LOG_WARN, "没有观测变量，请先在左侧变量树勾选");
        return -1;
    }
    g_app.stop_poll = 0;
    g_app.acq_state = OS_ACQ_RUNNING;
    g_app.hPoll = CreateThread(NULL, 0, poll_thread, NULL, 0, NULL);
    if (!g_app.hPoll) {
        g_app.acq_state = OS_ACQ_STOPPED;
        os_log(OS_LOG_ERROR, "创建采集线程失败");
        return -1;
    }
    os_log(OS_LOG_INFO, "采集已开始（周期 %d ms，%d 个变量）",
           g_app.poll_interval_ms, g_app.watch_count);
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
    os_log(OS_LOG_INFO, "采集已停止");
}

void os_ds_push_batch(OS_Sample* samples, int n)
{
    int i;
    if (n <= 0 || !samples) return;
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
    }
    InterlockedAdd(&g_app.total_samples, n);
    if (g_app.log_csv) os_datalog_append();
    if (g_app.hMain) PostMessage(g_app.hMain, WM_OS_SAMPLES, (WPARAM)n, 0);
}

void os_ds_drain(void)
{
    OS_Sample batch[OS_MAX_LEAVES];
    int n = 0, i, j;
    EnterCriticalSection(&g_app.ring_cs);
    while (g_app.ring_tail != g_app.ring_head && n < OS_MAX_LEAVES) {
        batch[n++] = g_app.ring[g_app.ring_tail];
        g_app.ring_tail = (g_app.ring_tail + 1) % OS_RING_CAP;
    }
    LeaveCriticalSection(&g_app.ring_cs);
    if (n <= 0) return;
    for (i = 0; i < g_app.win_count; i++) {
        HWND w = g_app.wins[i].hwnd;
        if (!w || g_app.wins[i].is_module || !IsWindow(w)) continue;
        for (j = 0; j < n; j++) {
            os_chart_push(w, &batch[j]);
            os_num_push(w, &batch[j]);
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
