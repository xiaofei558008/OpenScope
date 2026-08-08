/*
 * OpenScope acquisition core:
 *  - flatten ELF variables into scalar leaves;
 *  - periodic read loop with timestamps, CSV logging;
 *  - variable write-back;
 *  - offline CSV replay;
 *  - ELF load / hot reload.
 */
#include "app.h"

#include <string.h>

/* ------------------------- time helpers --------------------------- */

static int64_t now_us(void)
{
    FILETIME ft;
    ULARGE_INTEGER u;
    GetSystemTimeAsFileTime(&ft);
    u.LowPart = ft.dwLowDateTime;
    u.HighPart = ft.dwHighDateTime;
    /* 1601-01-01 -> 1970-01-01 offset in 100ns units */
    return (int64_t)((u.QuadPart - 116444736000000000ULL) / 10);
}

static ULONGLONG file_mtime_ns(const char* path)
{
    WIN32_FILE_ATTRIBUTE_DATA fd;
    ULARGE_INTEGER u;
    if (!GetFileAttributesExA(path, GetFileExInfoStandard, &fd))
        return 0;
    u.LowPart = fd.ftLastWriteTime.dwLowDateTime;
    u.HighPart = fd.ftLastWriteTime.dwHighDateTime;
    return u.QuadPart;
}

/* ------------------------- value helpers -------------------------- */

static double decode_raw(const uint8_t* raw, int size, int is_float, int is_signed)
{
    uint64_t u = 0;
    int i;
    for (i = size - 1; i >= 0; --i) u = (u << 8) | raw[i];
    if (is_float) {
        if (size == 4) {
            float f;
            uint32_t v = (uint32_t)u;
            memcpy(&f, &v, 4);
            return f;
        }
        if (size == 8) {
            double d;
            memcpy(&d, &u, 8);
            return d;
        }
        return (double)u;
    }
    if (is_signed) {
        int64_t s;
        switch (size) {
        case 1: s = (int64_t)(int8_t)u; break;
        case 2: s = (int64_t)(int16_t)u; break;
        case 4: s = (int64_t)(int32_t)u; break;
        default: s = (int64_t)u; break;
        }
        return (double)s;
    }
    return (double)u;
}

static void encode_raw(double value, int size, int is_float, int is_signed, uint8_t* out)
{
    int i;
    if (is_float && size == 4) {
        float f = (float)value;
        memcpy(out, &f, 4);
        return;
    }
    if (is_float && size == 8) {
        double d = value;
        memcpy(out, &d, 8);
        return;
    }
    {
        uint64_t u;
        if (is_signed) {
            int64_t v = (int64_t)value;
            switch (size) {
            case 1: v = (int8_t)v; break;
            case 2: v = (int16_t)v; break;
            case 4: v = (int32_t)v; break;
            default: break;
            }
            u = (uint64_t)v;
        } else {
            double c = value < 0 ? 0 : value;
            uint64_t v = (uint64_t)c;
            switch (size) {
            case 1: v &= 0xff; break;
            case 2: v &= 0xffff; break;
            case 4: v &= 0xffffffffULL; break;
            default: break;
            }
            u = v;
        }
        for (i = 0; i < size; ++i) {
            out[i] = (uint8_t)(u >> (8 * i));
        }
    }
}

static void sample_text(OS_Sample* s)
{
    double v = s->value;
    if (s->written) {
        _snprintf(s->text, sizeof(s->text), "(写入) %g", v);
    } else {
        _snprintf(s->text, sizeof(s->text), "%g", v);
    }
}

const char* os_leaf_value_text(int id, char* buf, int buflen)
{
    OS_Leaf* L;
    if (id < 0 || id >= g_app.leaf_count) { if (buflen) buf[0] = 0; return buf; }
    L = &g_app.leaves[id];
    if (!L->last.size) { if (buflen) buf[0] = 0; return buf; }
    _snprintf(buf, buflen, "%s", L->last.text[0] ? L->last.text : "?");
    return buf;
}

/* ------------------------- leaf flattening ------------------------ */

static int is_scalar_kind(OS_TypeKind k)
{
    return k == OS_TYPE_INT || k == OS_TYPE_UINT || k == OS_TYPE_FLOAT ||
           k == OS_TYPE_BOOL || k == OS_TYPE_ENUM || k == OS_TYPE_PTR ||
           k == OS_TYPE_STRING || k == OS_TYPE_OTHER;
}

static int enum_is_signed(OS_Type* t)
{
    int i;
    if (!t || t->kind != OS_TYPE_ENUM) return 0;
    for (i = 0; i < t->child_count; ++i)
        if (t->children[i] && t->children[i]->enum_value < 0) return 1;
    return 0;
}

static void add_leaf(OS_Variable* v, const char* path, uint64_t addr, OS_Type* t)
{
    OS_Leaf* L;
    uint32_t sz;
    if (g_app.leaf_count >= OS_MAX_LEAVES) return;
    if (!t) {
        sz = v->symbol_size;
    } else {
        sz = t->size;
    }
    if (sz == 0) sz = v->symbol_size;
    if (sz == 0 || sz > 8) return;
    L = &g_app.leaves[g_app.leaf_count];
    memset(L, 0, sizeof(*L));
    L->id = g_app.leaf_count;
    _snprintf(L->path, sizeof(L->path), "%s", path);
    L->address = addr;
    L->size = (uint8_t)sz;
    L->valid = 1;
    if (t) {
        L->is_float = (t->kind == OS_TYPE_FLOAT);
        L->is_bool = (t->kind == OS_TYPE_BOOL);
        if (t->kind == OS_TYPE_INT || t->kind == OS_TYPE_ENUM)
            L->is_signed = (t->kind == OS_TYPE_INT && t->is_signed) || enum_is_signed(t);
        else if (t->kind == OS_TYPE_PTR)
            L->is_signed = 0;
    }
    ++g_app.leaf_count;
}

static void flatten_rec(OS_Variable* v, OS_Type* t, const char* prefix, uint64_t addr, int depth)
{
    char path[256];
    int i;
    if (depth > 12 || g_app.leaf_count >= OS_MAX_LEAVES) return;
    if (!t) {
        add_leaf(v, prefix, addr, NULL);
        return;
    }
    if (is_scalar_kind(t->kind)) {
        add_leaf(v, prefix, addr, t);
        return;
    }
    if (t->kind == OS_TYPE_STRUCT || t->kind == OS_TYPE_UNION) {
        for (i = 0; i < t->child_count; ++i) {
            OS_Type* m = t->children[i];
            if (!m) continue;
            _snprintf(path, sizeof(path), "%s.%s", prefix, m->name ? m->name : "?");
            flatten_rec(v, m, path, addr + (uint64_t)m->member_offset, depth + 1);
        }
        return;
    }
    if (t->kind == OS_TYPE_ARRAY) {
        OS_Type* elem = t->child_count > 0 ? t->children[0] : NULL;
        uint64_t elem_size = elem && elem->size ? elem->size : (t->size && t->array_count ? t->size / t->array_count : 1);
        int count = t->array_count > 0 ? t->array_count : 0;
        if (elem && elem->size == 1 && count > 1) {
            /* char 数组：整体作为原始字节叶变量（最多 8 字节） */
            OS_Type raw;
            memset(&raw, 0, sizeof(raw));
            raw.kind = OS_TYPE_OTHER;
            raw.size = count > 8 ? 8 : (uint32_t)count;
            _snprintf(path, sizeof(path), "%s[%d]", prefix, count);
            add_leaf(v, path, addr, &raw);
            return;
        }
        if (count > OS_MAX_ARRAY_FLAT) count = OS_MAX_ARRAY_FLAT;
        for (i = 0; i < count; ++i) {
            _snprintf(path, sizeof(path), "%s[%d]", prefix, i);
            flatten_rec(v, elem, path, addr + (uint64_t)i * elem_size, depth + 1);
        }
        return;
    }
    add_leaf(v, prefix, addr, t);
}

void os_rebuild_leaves(void)
{
    int i;
    g_app.leaf_count = 0;
    if (!g_app.elf) return;
    for (i = 0; i < os_elf_var_count(g_app.elf); ++i) {
        const OS_Variable* v = os_elf_var_at(g_app.elf, i);
        if (!v || !v->name || !v->name[0]) continue;
        flatten_rec((OS_Variable*)v, v->type, v->name, v->address, 0);
    }
}

/* ------------------------- ELF load / reload ---------------------- */

int os_load_elf(const char* path)
{
    char errbuf[256];
    OS_ElfFile* elf;
    if (!path || !path[0]) return -1;
    elf = os_elf_open(path, errbuf, sizeof(errbuf));
    if (!elf) {
        os_log(OS_LOG_ERROR, "加载 ELF 失败: %s", errbuf);
        return -1;
    }
    os_unload_elf();
    g_app.elf = elf;
    _snprintf(g_app.elf_path, sizeof(g_app.elf_path), "%s", path);
    g_app.elf_mtime = file_mtime_ns(path);
    os_rebuild_leaves();
    os_refresh_tree();
    os_log(OS_LOG_INFO, "已加载 ELF: %s", path);
    os_log(OS_LOG_INFO, "  架构=%s 位=%d 入口=0x%llX 全局变量=%d 叶变量=%d",
           os_elf_arch_name(elf), os_elf_bits(elf),
           (unsigned long long)os_elf_entry(elf),
           os_elf_var_count(elf), g_app.leaf_count);
    os_status("ELF: %s (%d 个叶变量)", path, g_app.leaf_count);
    return 0;
}

void os_unload_elf(void)
{
    if (g_app.elf) {
        os_elf_close(g_app.elf);
        g_app.elf = NULL;
    }
    g_app.elf_path[0] = 0;
    g_app.leaf_count = 0;
}

/* 快照旧的叶路径，用于重载后提示“找不到的变量” */
typedef struct OldLeaf {
    char path[256];
    uint8_t size;
} OldLeaf;

static OldLeaf g_old_leaves[OS_MAX_LEAVES];
static int g_old_count;

void os_reload_elf(void)
{
    int i;
    int missing[OS_MAX_LEAVES];
    int n_missing = 0;
    char msg[2048];
    int keep = 0;
    int was_running = g_app.acq_running;

    if (was_running) os_stop_acq();
    /* 快照 */
    g_old_count = g_app.leaf_count < OS_MAX_LEAVES ? g_app.leaf_count : OS_MAX_LEAVES;
    for (i = 0; i < g_old_count; ++i) {
        _snprintf(g_old_leaves[i].path, sizeof(g_old_leaves[i].path), "%s", g_app.leaves[i].path);
        g_old_leaves[i].size = g_app.leaves[i].size;
    }
    if (os_load_elf(g_app.elf_path) != 0) {
        if (was_running) os_start_acq();
        return;
    }
    /* 找出缺失变量 */
    for (i = 0; i < g_old_count; ++i) {
        int j, found = 0;
        for (j = 0; j < g_app.leaf_count; ++j)
            if (strcmp(g_app.leaves[j].path, g_old_leaves[i].path) == 0) { found = 1; break; }
        if (!found && n_missing < (int)(sizeof(missing) / sizeof(missing[0])))
            missing[n_missing++] = i;
    }
    if (n_missing > 0) {
        char list[1024];
        int shown = n_missing > 8 ? 8 : n_missing;
        list[0] = 0;
        for (i = 0; i < shown; ++i) {
            if (i) strcat(list, ", ");
            strncat(list, g_old_leaves[missing[i]].path,
                    sizeof(list) - strlen(list) - 1);
        }
        if (n_missing > shown) {
            char tmp[64];
            _snprintf(tmp, sizeof(tmp), " ... 等 %d 个", n_missing);
            strncat(list, tmp, sizeof(list) - strlen(list) - 1);
        }
        _snprintf(msg, sizeof(msg),
                  "以下 %d 个变量在新 ELF 中未找到：\n%s\n\n是否忽略这些变量？\n"
                  "是 = 从窗口中移除；否 = 保留为无效（地址失效，暂停读取）。",
                  n_missing, list);
        keep = (ui_confirm("ELF 已更新", msg, MB_YESNO | MB_ICONQUESTION) == IDNO);
        if (keep) {
            for (i = 0; i < n_missing; ++i) {
                OS_Leaf* L;
                if (g_app.leaf_count >= OS_MAX_LEAVES) break;
                L = &g_app.leaves[g_app.leaf_count];
                memset(L, 0, sizeof(*L));
                L->id = g_app.leaf_count;
                _snprintf(L->path, sizeof(L->path), "%s", g_old_leaves[missing[i]].path);
                L->size = g_old_leaves[missing[i]].size;
                L->valid = 0;
                ++g_app.leaf_count;
            }
        }
    }
    os_notify_modules_reload();
    os_refresh_tree();
    if (was_running) os_start_acq();
}

void os_prompt_elf_changed(void)
{
    ULONGLONG now_mtime = file_mtime_ns(g_app.elf_path);
    if (!g_app.elf || g_app.elf_path[0] == 0) return;
    if (now_mtime == g_app.elf_mtime || now_mtime == 0) return;
    if (ui_confirm("ELF 已更新",
                   "检测到 ELF 文件已被重新编译，是否立即重新加载并刷新所有变量地址？",
                   MB_YESNO | MB_ICONQUESTION) == IDYES) {
        g_app.elf_mtime = now_mtime;
        os_reload_elf();
    } else {
        g_app.elf_mtime = now_mtime;
    }
}

/* ------------------------- acquisition ---------------------------- */

static DWORD WINAPI acq_thread_fn(LPVOID param)
{
    (void)param;
    int hz = g_app.sample_hz > 0 ? g_app.sample_hz : OS_DEFAULT_SAMPLE_HZ;
    int interval = 1000000 / hz;
    while (g_app.acq_running) {
        ULONGLONG t0 = GetTickCount64();
        if (g_app.connected && g_app.leaf_count > 0) {
            OS_Sample burst[OS_MAX_SAMPLE_BURST];
            int n = 0, i;
            int64_t ts = now_us();
            for (i = 0; i < g_app.leaf_count && n < OS_MAX_SAMPLE_BURST; ++i) {
                OS_Leaf* L = &g_app.leaves[i];
                uint8_t buf[8];
                int r;
                if (!L->valid || L->size == 0 || L->size > 8) continue;
                r = os_driver_read(L->address, L->size, buf);
                if (r == (int)L->size) {
                    OS_Sample s;
                    memset(&s, 0, sizeof(s));
                    s.ts_us = ts;
                    s.var_id = L->id;
                    s.address = L->address;
                    memcpy(s.raw, buf, L->size);
                    s.size = L->size;
                    s.value = decode_raw(buf, L->size, L->is_float, L->is_signed);
                    sample_text(&s);
                    L->last = s;
                    burst[n++] = s;
                }
            }
            if (n > 0) os_dispatch_samples(burst, n);
            if (g_app.csv && n > 0) {
                char line[16384];
                int off = 0;
                off += _snprintf(line + off, sizeof(line) - off, "%lld", (long long)ts);
                for (i = 0; i < g_app.leaf_count; ++i) {
                    OS_Leaf* L = &g_app.leaves[i];
                    double v = 0;
                    if (!L->valid || L->size == 0 || L->size > 8) continue;
                    if (L->last.size) v = L->last.value;
                    off += _snprintf(line + off, sizeof(line) - off, ",%.10g", v);
                    if (off >= (int)sizeof(line) - 32) break;
                }
                off += _snprintf(line + off, sizeof(line) - off, "\n");
                fwrite(line, 1, (size_t)off, g_app.csv);
                ++g_app.acq_cycles;
                if ((g_app.acq_cycles & 15) == 0) fflush(g_app.csv);
            }
        } else if (g_app.csv) {
            fflush(g_app.csv);
        }
        {
            int elapsed = (int)(GetTickCount64() - t0);
            int wait = interval - elapsed;
            if (wait < 1) wait = 1;
            if (wait > 5000) wait = 5000;
            if (WaitForSingleObject(g_app.hAcqStop, wait) == WAIT_OBJECT_0) break;
        }
    }
    InterlockedExchange(&g_app.acq_running, 0);
    if (g_app.hMain) PostMessageW(g_app.hMain, WM_APP_ACQ_END, 0, 0);
    return 0;
}

int ui_choose_csv_path(char* out, int cap);

int os_start_acq(void)
{
    if (!g_app.connected) {
        os_log(OS_LOG_ERROR, "尚未连接 MCU，无法开始采集");
        return OS_ERR_NOT_CONNECTED;
    }
    if (g_app.acq_running) return OS_ERR_OK;
    if (g_app.leaf_count == 0) {
        os_log(OS_LOG_WARN, "没有可采集的变量（请先加载 ELF）");
    }
    if (!g_app.csv) {
        char path[MAX_PATH];
        if (ui_choose_csv_path(path, sizeof(path)) == 1) {
            g_app.csv = fopen(path, "wb");
            if (g_app.csv) {
                int i;
                int off = 0;
                char hdr[16384];
                _snprintf(g_app.csv_path, sizeof(g_app.csv_path), "%s", path);
                off += _snprintf(hdr + off, sizeof(hdr) - off, "timestamp_us");
                for (i = 0; i < g_app.leaf_count; ++i) {
                    if (!g_app.leaves[i].valid) continue;
                    off += _snprintf(hdr + off, sizeof(hdr) - off, ",%s", g_app.leaves[i].path);
                    if (off >= (int)sizeof(hdr) - 64) break;
                }
                off += _snprintf(hdr + off, sizeof(hdr) - off, "\n");
                fwrite(hdr, 1, (size_t)off, g_app.csv);
                fflush(g_app.csv);
                os_log(OS_LOG_INFO, "日志文件: %s", path);
            }
        } else {
            os_log(OS_LOG_INFO, "未选择日志文件，本次采集不写 CSV");
        }
    }
    if (!g_app.hAcqStop) g_app.hAcqStop = CreateEventW(NULL, TRUE, FALSE, NULL);
    else ResetEvent(g_app.hAcqStop);
    g_app.acq_cycles = 0;
    g_app.acq_start_ms = GetTickCount64();
    InterlockedExchange(&g_app.acq_running, 1);
    g_app.hAcqThread = CreateThread(NULL, 0, acq_thread_fn, NULL, 0, NULL);
    os_status("采集中 @ %d Hz", g_app.sample_hz);
    os_log(OS_LOG_INFO, "开始采集（%d Hz）", g_app.sample_hz);
    return OS_ERR_OK;
}

void os_stop_acq(void)
{
    if (!g_app.acq_running) {
        if (g_app.csv) { fclose(g_app.csv); g_app.csv = NULL; g_app.csv_path[0] = 0; }
        return;
    }
    if (g_app.hAcqStop) SetEvent(g_app.hAcqStop);
    if (g_app.hAcqThread) {
        WaitForSingleObject(g_app.hAcqThread, 3000);
        CloseHandle(g_app.hAcqThread);
        g_app.hAcqThread = NULL;
    }
    if (g_app.csv) {
        fflush(g_app.csv);
        fclose(g_app.csv);
        g_app.csv = NULL;
    }
    os_status("已停止采集");
    os_log(OS_LOG_INFO, "停止采集");
}

/* ------------------------- variable write ------------------------- */

int os_write_leaf(int id, double value, char* err, int errlen)
{
    OS_Leaf* L;
    uint8_t buf[8];
    int r;
    OS_Sample s;
    if (errlen > 0) err[0] = 0;
    if (id < 0 || id >= g_app.leaf_count) {
        if (err) _snprintf(err, errlen, "变量不存在");
        return OS_ERR_INVALID_ARG;
    }
    L = &g_app.leaves[id];
    if (!L->valid || L->size == 0 || L->size > 8) {
        if (err) _snprintf(err, errlen, "变量地址无效");
        return OS_ERR_INVALID_ARG;
    }
    if (!g_app.connected) {
        if (err) _snprintf(err, errlen, "未连接 MCU");
        return OS_ERR_NOT_CONNECTED;
    }
    encode_raw(value, L->size, L->is_float, L->is_signed, buf);
    r = os_driver_write(L->address, L->size, buf);
    if (r != (int)L->size) {
        if (err) _snprintf(err, errlen, "写入失败 (err=%d)", r);
        return OS_ERR_FAIL;
    }
    memset(&s, 0, sizeof(s));
    s.ts_us = now_us();
    s.var_id = L->id;
    s.address = L->address;
    memcpy(s.raw, buf, L->size);
    s.size = L->size;
    s.value = value;
    s.written = 1;
    sample_text(&s);
    L->last = s;
    os_dispatch_samples(&s, 1);
    os_log(OS_LOG_INFO, "写入 %s = %g @0x%llX", L->path, value,
           (unsigned long long)L->address);
    return OS_ERR_OK;
}

/* ------------------------- CSV replay ----------------------------- */

typedef struct ReplayCol {
    int leaf_id;
    int col;
} ReplayCol;

static int split_line(char* line, char** fields, int max)
{
    int n = 0;
    char* p = line;
    while (*p && n < max) {
        fields[n++] = p;
        while (*p && *p != ',') ++p;
        if (*p == ',') *p++ = 0;
    }
    return n;
}

static DWORD WINAPI replay_thread_fn(LPVOID param)
{
    (void)param;
    FILE* fp = fopen(g_app.replay_path, "r");
    char line[16384];
    int hdr_cols = 0;
    char* hdr[512];
    ReplayCol cols[512];
    int ncols = 0;
    int64_t prev_ts = -1;

    if (!fp) {
        os_log(OS_LOG_ERROR, "无法打开回放文件: %s", g_app.replay_path);
        InterlockedExchange(&g_app.replay_running, 0);
        PostMessageW(g_app.hMain, WM_APP_REPLAY_END, 0, 0);
        return 0;
    }
    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        InterlockedExchange(&g_app.replay_running, 0);
        PostMessageW(g_app.hMain, WM_APP_REPLAY_END, 0, 0);
        return 0;
    }
    hdr_cols = split_line(line, hdr, 512);
    {
        int i;
        for (i = 1; i < hdr_cols; ++i) {
            char* nm = hdr[i];
            int j;
            /* 去空格 */
            while (*nm == ' ' || *nm == '\t') ++nm;
            for (j = 0; j < g_app.leaf_count; ++j) {
                if (strcmp(g_app.leaves[j].path, nm) == 0) {
                    if (ncols < 512) {
                        cols[ncols].leaf_id = j;
                        cols[ncols].col = i;
                        ++ncols;
                    }
                    break;
                }
            }
        }
    }
    os_log(OS_LOG_INFO, "开始回放: %s（%d 列映射）", g_app.replay_path, ncols);
    os_status("回放中: %s", g_app.replay_path);
    while (fgets(line, sizeof(line), fp) && g_app.replay_running) {
        char* fields[512];
        int nf = split_line(line, fields, 512);
        int64_t ts;
        int i;
        if (nf < 1) continue;
        ts = _strtoi64(fields[0], NULL, 10);
        if (prev_ts >= 0 && ts > prev_ts) {
            int64_t dt = ts - prev_ts;
            if (dt > 5000000) dt = 5000000; /* 最多 5 秒 */
            {
                DWORD wait_ms = (DWORD)(dt / 1000);
                if (wait_ms < 1) wait_ms = 1;
                if (WaitForSingleObject(g_app.hReplayStop, wait_ms) == WAIT_OBJECT_0) break;
            }
        }
        prev_ts = ts;
        {
            OS_Sample burst[512];
            int n = 0;
            for (i = 0; i < ncols && n < 512; ++i) {
                OS_Leaf* L = &g_app.leaves[cols[i].leaf_id];
                double v = cols[i].col < nf ? atof(fields[cols[i].col]) : 0;
                OS_Sample s;
                memset(&s, 0, sizeof(s));
                s.ts_us = ts;
                s.var_id = L->id;
                s.address = L->address;
                encode_raw(v, L->size, L->is_float, L->is_signed, s.raw);
                s.size = L->size;
                s.value = v;
                sample_text(&s);
                L->last = s;
                burst[n++] = s;
            }
            if (n > 0) os_dispatch_samples(burst, n);
        }
    }
    fclose(fp);
    os_log(OS_LOG_INFO, "回放结束");
    InterlockedExchange(&g_app.replay_running, 0);
    if (g_app.hMain) PostMessageW(g_app.hMain, WM_APP_REPLAY_END, 0, 0);
    return 0;
}

int os_start_replay(const char* path)
{
    if (g_app.replay_running) return OS_ERR_BUSY;
    if (!path || !path[0]) return OS_ERR_INVALID_ARG;
    _snprintf(g_app.replay_path, sizeof(g_app.replay_path), "%s", path);
    if (!g_app.hReplayStop) g_app.hReplayStop = CreateEventW(NULL, TRUE, FALSE, NULL);
    else ResetEvent(g_app.hReplayStop);
    InterlockedExchange(&g_app.replay_running, 1);
    g_app.hReplayThread = CreateThread(NULL, 0, replay_thread_fn, NULL, 0, NULL);
    return OS_ERR_OK;
}

void os_stop_replay(void)
{
    if (!g_app.replay_running) return;
    if (g_app.hReplayStop) SetEvent(g_app.hReplayStop);
    if (g_app.hReplayThread) {
        WaitForSingleObject(g_app.hReplayThread, 3000);
        CloseHandle(g_app.hReplayThread);
        g_app.hReplayThread = NULL;
    }
    InterlockedExchange(&g_app.replay_running, 0);
}
