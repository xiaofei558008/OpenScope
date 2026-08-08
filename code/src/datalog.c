#include "app.h"
#include "datalog.h"
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
    if (!g_app.log_csv) return;
    for (i = 0; i < g_app.leaf_count; i++) {
        OS_Leaf* L = &g_app.leaves[i];
        if (!L->watched) continue;
        os_time_iso(L->sample.ts_us, iso, sizeof(iso));
        fprintf(g_app.log_csv, "%lld,%s,", (long long)L->sample.ts_us, iso);
        csv_write_field(g_app.log_csv, L->sample.text);
        fputc('\n', g_app.log_csv);
    }
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
        os_format_raw(batch[n].text, sizeof(batch[n].text), raw, sz, L->kind, L->is_signed,
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
        nf = split_csv(r->pending, fields, 512);
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
