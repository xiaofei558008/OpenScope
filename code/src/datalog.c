#include "app.h"
#include "datalog.h"
#include "datasrv.h"
#include "vartree.h"
#include <stdlib.h>
#include <string.h>

typedef struct OS_Replay {
    FILE* f;
    int ncols;
    int* col_leaf;
    int ts_col;
    int64_t base_ts, next_ts;
    int64_t mono_start;
    int started;
    int has_pending;
    char pending[8192];
} OS_Replay;

static void csv_write_field(FILE* f, const char* s)
{
    int quote = 0;
    const char* p;
    if (!s) s = "";
    for (p = s; *p; p++) {
        if (*p == ',' || *p == '"' || *p == '\n' || *p == '\r') { quote = 1; break; }
    }
    if (quote) {
        fputc('"', f);
        for (p = s; *p; p++) {
            if (*p == '"') fputc('"', f);
            fputc(*p, f);
        }
        fputc('"', f);
    } else {
        fputs(s, f);
    }
}

int os_datalog_start(const wchar_t* path)
{
    int i;
    char iso[64];
    if (g_app.log_csv) os_datalog_stop();
    g_app.log_csv = _wfopen(path, L"wb");
    if (!g_app.log_csv) return -1;
    _snwprintf(g_app.log_path, MAX_PATH, L"%s", path);
    fputs("\xEF\xBB\xBF", g_app.log_csv); /* UTF-8 BOM */
    fputs("timestamp_us,time_iso", g_app.log_csv);
    for (i = 0; i < g_app.leaf_count; i++) {
        if (g_app.leaves[i].watched) {
            fputc(',', g_app.log_csv);
            csv_write_field(g_app.log_csv, g_app.leaves[i].name);
        }
    }
    fputc('\n', g_app.log_csv);
    os_time_iso(os_time_us(), iso, sizeof(iso));
    os_log(OS_LOG_INFO, "开始记录: %ls", path);
    return 0;
}

void os_datalog_stop(void)
{
    if (g_app.log_csv) {
        fflush(g_app.log_csv);
        fclose(g_app.log_csv);
        g_app.log_csv = NULL;
        os_log(OS_LOG_INFO, "记录已停止: %ls", g_app.log_path);
    }
}

void os_datalog_append(void)
{
    int i;
    char iso[64];
    int64_t ts = 0;
    if (!g_app.log_csv) return;
    for (i = 0; i < g_app.leaf_count; i++) {
        if (g_app.leaves[i].watched && g_app.leaves[i].sample.size) {
            ts = g_app.leaves[i].sample.ts_us;
            break;
        }
    }
    os_time_iso(ts, iso, sizeof(iso));
    fprintf(g_app.log_csv, "%lld,%s", (long long)ts, iso);
    for (i = 0; i < g_app.leaf_count; i++) {
        OS_Leaf* L = &g_app.leaves[i];
        if (!L->watched) continue;
        fputc(',', g_app.log_csv);
        csv_write_field(g_app.log_csv, L->sample.size ? L->sample.text : "");
    }
    fputc('\n', g_app.log_csv);
    fflush(g_app.log_csv);
}

static char* trim_crlf(char* s)
{
    size_t n = strlen(s);
    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r')) s[--n] = 0;
    return s;
}

static int split_csv(char* line, char** out, int max)
{
    int n = 0;
    char* p = line;
    while (*p && n < max) {
        char* start = p;
        if (*p == '"') {
            /* 带引号字段 */
            p++;
            out[n++] = p;
            while (*p) {
                if (*p == '"') {
                    if (p[1] == '"') { p += 2; continue; }
                    *p = 0;
                    p++;
                    break;
                }
                p++;
            }
            if (*p == ',') p++;
        } else {
            out[n++] = p;
            while (*p && *p != ',') p++;
            if (*p == ',') { *p = 0; p++; }
        }
        (void)start;
    }
    return n;
}

int os_replay_start(const wchar_t* path)
{
    OS_Replay* r;
    char line[8192];
    char* fields[512];
    int nf, i;
    FILE* f = _wfopen(path, L"rb");
    if (!f) return -1;
    r = (OS_Replay*)calloc(1, sizeof(OS_Replay));
    if (!r) { fclose(f); return -1; }
    r->f = f;
    r->ts_col = -1;
    if (fgets(line, sizeof(line), f)) {
        trim_crlf(line);
        nf = split_csv(line, fields, 512);
        r->ncols = nf;
        r->col_leaf = (int*)malloc(sizeof(int) * (nf > 0 ? nf : 1));
        for (i = 0; i < nf; i++) r->col_leaf[i] = -1;
        for (i = 0; i < nf; i++) {
            char* name = fields[i];
            /* 去掉字段名首尾空格 */
            while (*name == ' ') name++;
            if (!strcmp(name, "timestamp_us")) r->ts_col = i;
            else if (strncmp(name, "time_iso", 8) != 0) {
                int id = os_vartree_find_by_name(name);
                if (id >= 0) r->col_leaf[i] = id;
            }
        }
    }
    g_app.replay = r;
    g_app.acq_state = OS_ACQ_REPLAY;
    os_log(OS_LOG_INFO, "开始离线回放: %ls", path);
    return 0;
}

void os_replay_stop(void)
{
    OS_Replay* r = g_app.replay;
    if (!r) return;
    if (r->f) fclose(r->f);
    free(r->col_leaf);
    free(r);
    g_app.replay = NULL;
    g_app.acq_state = OS_ACQ_STOPPED;
    os_log(OS_LOG_INFO, "回放已停止");
}

static void emit_row(OS_Replay* r, char** fields, int nf, int64_t ts)
{
    OS_Sample batch[256];
    int n = 0, i;
    for (i = 0; i < nf && n < 256; i++) {
        int id;
        OS_Leaf* L;
        uint8_t raw[8];
        int sz = 0;
        if (i == r->ts_col || r->col_leaf[i] < 0) continue;
        id = r->col_leaf[i];
        L = &g_app.leaves[id];
        memset(raw, 0, sizeof(raw));
        if (!os_parse_text(fields[i], raw, 8, &sz, L->kind, L->is_signed, 0, 0, 0,
                           L->enums, L->enum_count)) {
            continue;
        }
        memset(&batch[n], 0, sizeof(batch[0]));
        batch[n].ts_us = ts;
        batch[n].var_id = id;
        batch[n].address = L->address;
        memcpy(batch[n].raw, raw, sz < 8 ? sz : 8);
        batch[n].size = sz;
        os_format_raw(batch[n].text, sizeof(batch[n].text), raw, sz, L->kind, L->is_signed, L->is_ptr,
                      L->is_bitfield, L->bit_offset, L->bit_size, &batch[n].value,
                      L->enums, L->enum_count);
        n++;
    }
    if (n > 0) os_ds_push_batch(batch, n);
}

void os_replay_tick(void)
{
    OS_Replay* r = g_app.replay;
    char* fields[512];
    int64_t now;
    if (!r) return;
    now = os_time_us();
    if (!r->started) {
        r->mono_start = now;
        r->started = 1;
    }
    for (;;) {
        char line[8192];
        char splitbuf[8192];
        int nf;
        int64_t ts;
        int64_t expected;
        if (!r->has_pending) {
            if (!fgets(line, sizeof(line), r->f)) {
                os_log(OS_LOG_INFO, "回放结束");
                if (g_app.hMain) PostMessage(g_app.hMain, WM_OS_ACQ_STATE, 0, 0);
                os_replay_stop();
                return;
            }
            trim_crlf(line);
            _snprintf(r->pending, sizeof(r->pending), "%s", line);
        }
        trim_crlf(r->pending);
        /* split_csv mutates its input (commas -> NUL), so split a copy and
         * keep r->pending intact for re-parsing on later ticks. */
        _snprintf(splitbuf, sizeof(splitbuf), "%s", r->pending);
        nf = split_csv(splitbuf, fields, 512);
        r->has_pending = 0;
        ts = r->ts_col >= 0 && r->ts_col < nf ? _strtoi64(fields[r->ts_col], NULL, 10) : 0;
        if (!r->base_ts) r->base_ts = ts;
        expected = r->base_ts + (now - r->mono_start);
        if (ts > expected) {
            /* 还没到时间：保留行，下一 tick 再发 */
            r->has_pending = 1;
            return;
        }
        emit_row(r, fields, nf, ts);
    }
}

/* ============ 长时间采集自动落盘（RAM ≤10MB → 时间戳 CSV，用户需求） ============ */

static void spool_dir(wchar_t* out, int cap)
{
    wchar_t dir[MAX_PATH];
    wchar_t rec[MAX_PATH];
    if (GetEnvironmentVariableW(L"LOCALAPPDATA", dir, MAX_PATH) == 0 ||
        dir[0] == 0) {
        GetModuleFileNameW(NULL, dir, MAX_PATH);
        {
            wchar_t* slash = wcsrchr(dir, L'\\');
            if (slash) slash[1] = 0;
        }
    }
    _snwprintf(rec, MAX_PATH, L"%s\\OpenScope\\records", dir);
    CreateDirectoryW(rec, NULL);
    _snwprintf(out, cap, L"%s", rec);
}

/* 采集开始时建立落盘通道：RAM 缓冲 10MB，文件名按时间戳命名避免重复 */
int os_spool_begin(void)
{
    SYSTEMTIME st;
    wchar_t dir[MAX_PATH], path[MAX_PATH];
    if (g_app.spool_active) return 0;
    GetLocalTime(&st);
    spool_dir(dir, MAX_PATH);
    _snwprintf(path, MAX_PATH, L"%s\\rec_%04d%02d%02d_%02d%02d%02d.csv",
               dir, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
    g_app.spool_cap = OS_SPOOL_RAM_CAP_BYTES / (int)(sizeof(OS_Sample) > 0 ? sizeof(OS_Sample) : 1);
    if (g_app.spool_cap < 1024) g_app.spool_cap = 1024;
    g_app.spool_ram = (OS_Sample*)malloc(sizeof(OS_Sample) * g_app.spool_cap);
    if (!g_app.spool_ram) {
        os_log(OS_LOG_ERROR, "落盘 RAM 缓冲分配失败（%d 样本）", g_app.spool_cap);
        return -1;
    }
    g_app.spool_cnt = 0;
    g_app.spool_f = NULL;
    _snwprintf(g_app.spool_path, MAX_PATH, L"%s", path);
    g_app.spool_active = 1;
    os_log(OS_LOG_INFO, "采集落盘: RAM 缓冲 %d 样本(约10MB)，超出写 %ls",
           g_app.spool_cap, g_app.spool_path);
    return 0;
}

/* 首次写盘：打开文件 + 写表头（与记录格式一致，回放可加载） */
static int spool_open_file(void)
{
    int i;
    if (g_app.spool_f) return 0;
    g_app.spool_f = _wfopen(g_app.spool_path, L"wb");
    if (!g_app.spool_f) return -1;
    fputs("\xEF\xBB\xBF", g_app.spool_f);
    fputs("timestamp_us,time_iso", g_app.spool_f);
    for (i = 0; i < g_app.leaf_count; i++) {
        if (g_app.leaves[i].watched) {
            fputc(',', g_app.spool_f);
            csv_write_field(g_app.spool_f, g_app.leaves[i].name);
        }
    }
    fputc('\n', g_app.spool_f);
    return 0;
}

static void spool_write_sample(FILE* f, const OS_Sample* s)
{
    char iso[64];
    int i;
    os_time_iso(s->ts_us, iso, sizeof(iso));
    fprintf(f, "%lld,%s", (long long)s->ts_us, iso);
    for (i = 0; i < g_app.leaf_count; i++) {
        OS_Leaf* L = &g_app.leaves[i];
        if (!L->watched) continue;
        fputc(',', f);
        csv_write_field(f, (L->id == s->var_id && s->size) ? s->text : "");
    }
    fputc('\n', f);
}

/* 采集线程每批调用：先灌 RAM，超 10MB 后整批转写磁盘（后续直接写文件） */
void os_spool_push(OS_Sample* s, int n)
{
    int i;
    if (!g_app.spool_active || !s || n <= 0) return;
    for (i = 0; i < n; i++) {
        if (g_app.spool_f) {
            /* 已转盘：直接写文件（写失败只停一次，不崩溃） */
            spool_write_sample(g_app.spool_f, &s[i]);
            if (ferror(g_app.spool_f)) {
                os_log(OS_LOG_ERROR, "采集落盘写盘失败，停止落盘");
                g_app.spool_active = 0;
                fclose(g_app.spool_f);
                g_app.spool_f = NULL;
                return;
            }
        } else if (g_app.spool_cnt < g_app.spool_cap) {
            g_app.spool_ram[g_app.spool_cnt++] = s[i];
        } else {
            /* RAM 满：一次性转盘 */
            int k;
            if (spool_open_file() != 0) {
                os_log(OS_LOG_ERROR, "采集落盘文件打开失败: %ls", g_app.spool_path);
                g_app.spool_active = 0;
                return;
            }
            for (k = 0; k < g_app.spool_cnt; k++)
                spool_write_sample(g_app.spool_f, &g_app.spool_ram[k]);
            spool_write_sample(g_app.spool_f, &s[i]);
            if (g_app.spool_f) fflush(g_app.spool_f);
        }
    }
}

/* 采集停止：RAM 残余转盘 + 关闭文件 */
void os_spool_end(void)
{
    if (!g_app.spool_active) {
        free(g_app.spool_ram);
        g_app.spool_ram = NULL;
        if (g_app.spool_f) { fclose(g_app.spool_f); g_app.spool_f = NULL; }
        return;
    }
    if (g_app.spool_cnt > 0) {
        int k;
        if (spool_open_file() == 0) {
            for (k = 0; k < g_app.spool_cnt; k++)
                spool_write_sample(g_app.spool_f, &g_app.spool_ram[k]);
        }
    }
    if (g_app.spool_f) {
        fflush(g_app.spool_f);
        fclose(g_app.spool_f);
        g_app.spool_f = NULL;
    }
    os_log(OS_LOG_INFO, "采集落盘完成: %ls（RAM 缓冲 %d 样本）",
           g_app.spool_path, g_app.spool_cnt);
    free(g_app.spool_ram);
    g_app.spool_ram = NULL;
    g_app.spool_cnt = 0;
    g_app.spool_active = 0;
}

/* ============ 回放全量加载：整文件解析 + min/max 桶缓存，波形全部显示 ============ */

/* 清除旧桶缓存（重载前调用） */
void os_buckets_clear(void)
{
    int i;
    for (i = 0; i < OS_MAX_LEAVES; i++) {
        if (g_app.buckets[i].b) { free(g_app.buckets[i].b); g_app.buckets[i].b = NULL; }
        g_app.buckets[i].nb = 0;
        g_app.buckets[i].name[0] = 0;
        g_app.buckets[i].t0 = g_app.buckets[i].t1 = 0;
    }
    g_app.bucket_count = 0;
}

/* 解析一行：更新该行各列的叶桶缓存 + 推样本（RAM 环尾 + 叶当前值 + 数值窗口）。
 * 桶按行序均分（rows 已知），时间跨度 t0/t1 用首尾行 ts。 */
static void parse_row_all(char* line, char* splitbuf, int* col_leaf, int ts_col,
                          int ncols, int rows, int64_t* row_idx,
                          int64_t* first_ts, int64_t* last_ts)
{
    char* fields[512];
    int nf, i;
    int64_t ts;
    OS_Sample batch[256];
    int n = 0;
    _snprintf(splitbuf, 8192, "%s", line);
    nf = split_csv(splitbuf, fields, 512);
    if (nf <= 0) return;
    ts = ts_col >= 0 && ts_col < nf ? _strtoi64(fields[ts_col], NULL, 10) : 0;
    if (*first_ts == 0) *first_ts = ts;
    *last_ts = ts;
    for (i = 0; i < nf && i < ncols && n < 256; i++) {
        int id = col_leaf[i];
        OS_Leaf* L;
        uint8_t raw[8];
        int sz = 0;
        double v;
        OS_Bucket* bk;
        int nb, bi;
        if (id < 0 || i == ts_col) continue;
        L = &g_app.leaves[id];
        if (!os_parse_text(fields[i], raw, 8, &sz, L->kind, L->is_signed, 0, 0, 0,
                           L->enums, L->enum_count))
            continue;
        memset(&batch[n], 0, sizeof(batch[0]));
        os_format_raw(batch[n].text, sizeof(batch[n].text), raw, sz, L->kind, L->is_signed,
                      L->is_ptr, L->is_bitfield, L->bit_offset, L->bit_size, &v,
                      L->enums, L->enum_count);
        batch[n].ts_us = ts;
        batch[n].var_id = id;
        batch[n].address = L->address;
        memcpy(batch[n].raw, raw, sz < 8 ? sz : 8);
        batch[n].size = sz;
        batch[n].value = v;
        /* 桶：按行序均分（rows 已在第一遍数出） */
        nb = g_app.buckets[id].nb;
        bk = g_app.buckets[id].b;
        if (bk && nb > 0 && rows > 0) {
            bi = (int)((double)(*row_idx) * nb / rows);
            if (bi >= nb) bi = nb - 1;
            if (bi < 0) bi = 0;
            if (bk[bi].n == 0) { bk[bi].mn = bk[bi].mx = v; }
            else { if (v < bk[bi].mn) bk[bi].mn = v; if (v > bk[bi].mx) bk[bi].mx = v; }
            bk[bi].n++;
        }
        n++;
    }
    if (n > 0) os_ds_push_batch(batch, n);
    (*row_idx)++;
}

/* 回放全量加载：快速解析整个 CSV，为每个映射到的叶变量建立 min/max 桶缓存，
 * 同时把样本推入常规通道。返回 0 成功。 */
int os_replay_load_all(const wchar_t* path)
{
    FILE* f;
    char line[8192];
    char splitbuf[8192];
    int ncols = 0, nf, i, rows = 0, mapped = 0;
    int* col_leaf = NULL;
    int ts_col = -1;
    int64_t row_idx = 0;
    int64_t first_ts = 0, last_ts = 0;
    os_replay_stop(); /* 先停掉旧的实时回放 */
    os_buckets_clear();
    f = _wfopen(path, L"rb");
    if (!f) return -1;
    /* 表头 → 列映射 */
    if (fgets(line, sizeof(line), f)) {
        char* fields[512];
        trim_crlf(line);
        nf = split_csv(line, fields, 512);
        ncols = nf;
        col_leaf = (int*)malloc(sizeof(int) * (nf > 0 ? nf : 1));
        for (i = 0; i < nf; i++) col_leaf[i] = -1;
        for (i = 0; i < nf; i++) {
            char* name = fields[i];
            while (*name == ' ') name++;
            if (!strcmp(name, "timestamp_us")) ts_col = i;
            else if (strncmp(name, "time_iso", 8) != 0) {
                int id = os_vartree_find_by_name(name);
                if (id >= 0) col_leaf[i] = id;
            }
        }
    }
    /* 第一遍：数行（桶数 = min(8192, 行数)） */
    while (fgets(line, sizeof(line), f)) rows++;
    fseek(f, 0, SEEK_SET);
    fgets(line, sizeof(line), f); /* 跳过表头 */
    /* 为映射到的叶分配桶 */
    for (i = 0; i < ncols; i++) {
        int id = col_leaf[i];
        int nb = rows > OS_BUCKET_NB ? OS_BUCKET_NB : rows;
        if (id < 0 || id >= OS_MAX_LEAVES) continue;
        if (g_app.buckets[id].b) continue;
        if (nb <= 0) nb = 1;
        g_app.buckets[id].b = (OS_Bucket*)calloc(nb, sizeof(OS_Bucket));
        if (!g_app.buckets[id].b) continue;
        g_app.buckets[id].nb = nb;
        _snprintf(g_app.buckets[id].name, 256, "%s", g_app.leaves[id].name);
        mapped++;
    }
    g_app.bucket_count = mapped;
    /* 第二遍：填充桶 + 推样本（无实时节奏，全速加载） */
    while (fgets(line, sizeof(line), f)) {
        trim_crlf(line);
        parse_row_all(line, splitbuf, col_leaf, ts_col, ncols, rows,
                      &row_idx, &first_ts, &last_ts);
    }
    fclose(f);
    /* 桶时间跨度 = 首尾行 ts（按行序均分的桶在时间上近似均匀） */
    for (i = 0; i < ncols; i++) {
        int id = col_leaf[i];
        if (id >= 0 && id < OS_MAX_LEAVES && g_app.buckets[id].b) {
            g_app.buckets[id].t0 = first_ts;
            g_app.buckets[id].t1 = last_ts;
        }
    }
    free(col_leaf);
    g_app.acq_state = OS_ACQ_STOPPED;
    os_log(OS_LOG_INFO, "回放加载完成: %ls（%d 行，%d 个变量桶缓存）", path, rows, mapped);
    return 0;
}
