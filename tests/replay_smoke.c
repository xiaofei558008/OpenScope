/*
 * OpenScope offline-replay regression test (no hardware, no UI).
 *
 * Compiles together with code/src/datalog.c; stubs the framework
 * surface (g_app, vartree lookup, sample delivery, util helpers)
 * so the replay state machine can be driven with a synthetic CSV
 * and a controllable clock.
 *
 * Build:  cl /nologo /W2 /I code\src tests\replay_smoke.c code\src\datalog.c ^
 *             /Fe:tests\bin\replay_smoke.exe /link user32.lib
 * Run:    tests\bin\replay_smoke.exe
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "app.h"
#include "datalog.h"
#include "vartree.h"

OS_App g_app;

/* ---- controllable clock ---- */
static int64_t g_now_us = 0;
int64_t os_time_us(void) { return g_now_us; }
void os_time_iso(int64_t us, char* out, int outlen)
{
    _snprintf(out, outlen, "T%lld", (long long)us);
}

/* ---- logging: keep test output quiet ---- */
void os_log(int level, const char* fmt, ...)
{
    (void)level;
    (void)fmt;
}

/* ---- vartree lookup stub: match leaf name ---- */
int os_vartree_find_by_name(const char* name)
{
    int i;
    for (i = 0; i < g_app.leaf_count; i++)
        if (strcmp(g_app.leaves[i].name, name) == 0) return i;
    return -1;
}

/* ---- sample delivery stub ---- */
static OS_Sample g_pushed[1024];
static int g_npushed = 0;
void os_ds_push_batch(OS_Sample* samples, int n)
{
    int i;
    for (i = 0; i < n && g_npushed < 1024; i++) g_pushed[g_npushed++] = samples[i];
}

/* ---- minimal util stubs (datalog.c replay only) ---- */
static uint64_t read_le(const uint8_t* raw, int size)
{
    uint64_t v = 0;
    int i;
    for (i = size - 1; i >= 0; i--) v = (v << 8) | raw[i];
    return v;
}

static void write_le(uint8_t* out, uint64_t v, int size)
{
    int i;
    for (i = 0; i < size; i++) out[i] = (uint8_t)(v >> (8 * i));
}

int os_parse_text(const char* text, uint8_t* out, int max_size, int* out_size,
                  OS_TypeKind kind, int is_signed, int is_bitfield, int bit_offset,
                  int bit_size, const OS_EnumVal* enums, int enum_count)
{
    int sz;
    double d;
    uint64_t v;
    (void)is_bitfield; (void)bit_offset; (void)bit_size; (void)enums; (void)enum_count;
    sz = max_size < 8 ? max_size : 8;
    if (sz < 1) sz = 1;
    if (kind == OS_TYPE_FLOAT) {
        if (sz == 4) {
            float f = (float)atof(text);
            memcpy(out, &f, 4);
        } else if (sz == 8) {
            double dd = atof(text);
            memcpy(out, &dd, 8);
        } else {
            memset(out, 0, sz);
        }
    } else {
        d = atof(text);
        v = is_signed ? (uint64_t)(int64_t)d : (uint64_t)d;
        write_le(out, v, sz);
    }
    *out_size = sz;
    return 1;
}

void os_format_raw(char* out, int outlen, const uint8_t* raw, int size,
                   OS_TypeKind kind, int is_signed, int is_ptr, int is_bitfield,
                   int bit_offset, int bit_size, double* out_val,
                   const OS_EnumVal* enums, int enum_count)
{
    double v = 0;
    (void)is_ptr; (void)is_bitfield; (void)bit_offset; (void)bit_size;
    (void)enums; (void)enum_count;
    if (kind == OS_TYPE_FLOAT && size == 4) {
        float f;
        memcpy(&f, raw, 4);
        v = f;
    } else if (kind == OS_TYPE_FLOAT && size == 8) {
        memcpy(&v, raw, 8);
    } else {
        uint64_t u = read_le(raw, size);
        v = is_signed && size <= 4 ? (double)(int64_t)(int32_t)u : (double)u;
    }
    if (out_val) *out_val = v;
    _snprintf(out, outlen, "%.10g", v);
}

/* ---- helpers ---- */
static int add_leaf(const char* name, OS_TypeKind kind, int size, int is_signed)
{
    OS_Leaf* L;
    if (g_app.leaf_count >= OS_MAX_LEAVES) return -1;
    L = &g_app.leaves[g_app.leaf_count];
    memset(L, 0, sizeof(*L));
    L->id = g_app.leaf_count;
    _snprintf(L->name, sizeof(L->name), "%s", name);
    L->kind = kind;
    L->size = (uint32_t)size;
    L->is_signed = is_signed;
    L->watched = 1;
    return g_app.leaf_count++;
}

static int check(int cond, const char* what)
{
    printf("%s %s\n", cond ? "PASS" : "FAIL", what);
    return cond ? 0 : 1;
}

static void write_csv(const char* path)
{
    FILE* f = fopen(path, "wb");
    if (!f) return;
    fputs("timestamp_us,time_iso,temp,adc,bogus\n", f);
    fputs("1000000,T1000000,25.5,100,0\n", f);
    fputs("1001000,T1001000,26,200,0\n", f);
    fputs("1002000,T1002000,26.5,300,0\n", f);
    fputs("1003000,T1003000,\"27\",400,0\n", f); /* quoted field */
    fclose(f);
}

int main(void)
{
    int fails = 0;
    int temp, adc;
    const char* csv = "tests\\replay_sample.csv";

    setvbuf(stdout, NULL, _IONBF, 0);
    memset(&g_app, 0, sizeof(g_app));
    temp = add_leaf("temp", OS_TYPE_FLOAT, 4, 0);
    adc = add_leaf("adc", OS_TYPE_UINT, 4, 0);
    if (temp != 0 || adc != 1) return 2;

    write_csv(csv);

    /* nonexistent file must fail cleanly */
    fails += check(os_replay_start(L"tests\\no_such_file.csv") != 0,
                   "replay_start rejects missing file");
    fails += check(g_app.acq_state == OS_ACQ_STOPPED,
                   "acq_state stays stopped after failed start");

    /* normal replay */
    fails += check(os_replay_start(L"tests\\replay_sample.csv") == 0,
                   "replay_start opens CSV");
    fails += check(g_app.acq_state == OS_ACQ_REPLAY,
                   "acq_state becomes REPLAY");

    g_now_us = 1000000;
    os_replay_tick(); /* row 1: temp=25.5 adc=100 */
    fails += check(g_npushed == 2, "tick1 pushes 2 samples");
    if (g_npushed >= 2) {
        fails += check(g_pushed[0].value == 25.5 && g_pushed[1].value == 100,
                       "tick1 values correct");
        fails += check(g_pushed[0].ts_us == 1000000 && g_pushed[0].var_id == temp,
                       "tick1 timestamps/ids correct");
    }

    g_now_us = 1001000;
    os_replay_tick(); /* row 2 */
    fails += check(g_npushed == 4, "tick2 pushes 2 samples");
    if (g_npushed >= 4)
        fails += check(g_pushed[2].value == 26 && g_pushed[3].value == 200,
                       "tick2 values correct");

    g_now_us = 1002000;
    os_replay_tick(); /* row 3 */
    fails += check(g_npushed == 6, "tick3 pushes 2 samples");

    g_now_us = 1003000;
    os_replay_tick(); /* row 4 (quoted value) -> EOF, replay stops */
    fails += check(g_npushed == 8, "tick4 pushes 2 samples (quoted)");
    if (g_npushed >= 8)
        fails += check(g_pushed[6].value == 27 && g_pushed[7].value == 400,
                       "tick4 quoted values correct");
    fails += check(g_app.replay == NULL, "replay auto-stops at EOF");
    fails += check(g_app.acq_state == OS_ACQ_STOPPED,
                   "acq_state returns to STOPPED after EOF");

    /* tick after EOF must be a no-op */
    g_npushed = 0;
    os_replay_tick();
    fails += check(g_npushed == 0, "tick after EOF is no-op");

    printf(fails == 0 ? "ALL PASS\n" : "FAILURES: %d\n", fails);
    return fails == 0 ? 0 : 1;
}
