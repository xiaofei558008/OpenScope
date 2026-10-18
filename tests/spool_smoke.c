/* 录制临时缓存单元测试（无需硬件）：clear / begin / push / end / save_as */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "app.h"
#include "datalog.h"

OS_App g_app;
static int g_fail;

static void expect(int ok, const char* what)
{
    printf("%s %s\n", ok ? "PASS" : "FAIL", what);
    if (!ok) g_fail++;
}

int64_t os_time_us(void) { return 123456789; }
void os_time_iso(int64_t us, char* out, int outlen)
{
    _snprintf(out, outlen, "T%lld", (long long)us);
}
void os_log(int level, const char* fmt, ...) { (void)level; (void)fmt; }
int os_vartree_find_by_name(const char* name) { (void)name; return -1; }
void os_ds_push_batch(OS_Sample* samples, int n) { (void)samples; (void)n; }

int os_parse_text(const char* text, uint8_t* out, int max_size, int* out_size,
                  OS_TypeKind kind, int is_signed, int is_bitfield, int bit_offset,
                  int bit_size, const OS_EnumVal* enums, int enum_count)
{
    (void)text; (void)out; (void)max_size; (void)out_size; (void)kind;
    (void)is_signed; (void)is_bitfield; (void)bit_offset; (void)bit_size;
    (void)enums; (void)enum_count;
    return 0;
}
void os_format_raw(char* text, int text_cap, const uint8_t* raw, int size,
                   OS_TypeKind kind, int is_signed, int is_ptr, int is_bitfield,
                   int bit_offset, int bit_size, double* out_value,
                   const OS_EnumVal* enums, int enum_count)
{
    (void)text; (void)text_cap; (void)raw; (void)size; (void)kind;
    (void)is_signed; (void)is_ptr; (void)is_bitfield; (void)bit_offset;
    (void)bit_size; (void)out_value; (void)enums; (void)enum_count;
}

int main(void)
{
    OS_Sample s;
    wchar_t dest[MAX_PATH];
    FILE* f;
    char line[256];
    setvbuf(stdout, NULL, _IONBF, 0);
    memset(&g_app, 0, sizeof(g_app));
    g_app.leaf_count = 1;
    g_app.leaves[0].id = 0;
    g_app.leaves[0].watched = 1;
    _snprintf(g_app.leaves[0].name, sizeof(g_app.leaves[0].name), "fsin");

    expect(os_spool_begin() == 0, "begin 建立临时缓存");
    expect(g_app.spool_active == 1, "spool_active");
    expect(g_app.spool_path[0] != 0, "缓存路径非空");

    memset(&s, 0, sizeof(s));
    s.ts_us = 1000;
    s.var_id = 0;
    s.size = 4;
    _snprintf(s.text, sizeof(s.text), "1.250");
    os_spool_push(&s, 1);
    s.ts_us = 2000;
    _snprintf(s.text, sizeof(s.text), "2.500");
    os_spool_push(&s, 1);
    expect(g_app.spool_total == 2, "写入 2 样本");

    os_spool_end();
    expect(g_app.spool_active == 0, "end 后 inactive");
    expect(os_spool_has_data() == 1, "有可保存数据");

    GetTempPathW(MAX_PATH, dest);
    wcscat(dest, L"openscope_spool_smoke.csv");
    expect(os_spool_save_as(dest) == 0, "save_as 成功");
    f = _wfopen(dest, L"rb");
    expect(f != NULL, "打开保存文件");
    if (f) {
        fgets(line, sizeof(line), f);
        expect(strstr(line, "timestamp_us") != NULL && strstr(line, "fsin") != NULL, "表头正确");
        fclose(f);
    }
    DeleteFileW(dest);

    os_spool_clear();
    expect(os_spool_has_data() == 0, "clear 后无数据");
    expect(g_app.spool_path[0] == 0, "clear 后路径清空");

    expect(os_spool_begin() == 0, "再次 begin（模拟重新采集）");
    os_spool_push(&s, 1);
    os_spool_end();
    expect(os_spool_has_data() == 1, "第二段有数据");
    os_spool_clear();

    if (g_fail) {
        printf("SPOOL SMOKE FAIL (%d)\n", g_fail);
        return 1;
    }
    printf("SPOOL SMOKE ALL PASS\n");
    return 0;
}
