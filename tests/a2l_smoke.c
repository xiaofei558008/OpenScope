/* A2L 导出冒烟：验证 CANape 关键语法、类型、地址、标定量/测量量分类。 */
#include "app.h"
#include "a2l_export.h"

#include <stdio.h>
#include <string.h>
#include <windows.h>

OS_App g_app;

static int g_fail;

static void expect(int ok, const char* what)
{
    printf("%s %s\n", ok ? "PASS" : "FAIL", what);
    if (!ok) g_fail++;
}

static char* read_all(const wchar_t* path, int* len)
{
    FILE* f = _wfopen(path, L"rb");
    long n;
    char* buf;
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    n = ftell(f);
    fseek(f, 0, SEEK_SET);
    buf = (char*)malloc((size_t)n + 1);
    if (!buf) { fclose(f); return NULL; }
    if (fread(buf, 1, (size_t)n, f) != (size_t)n) { free(buf); fclose(f); return NULL; }
    buf[n] = 0;
    fclose(f);
    if (len) *len = (int)n;
    return buf;
}

static OS_Leaf* leaf_at(int i, const char* name, uint64_t addr, uint32_t size,
                        OS_TypeKind kind, int is_signed)
{
    OS_Leaf* L = &g_app.leaves[i];
    memset(L, 0, sizeof(*L));
    L->id = i;
    _snprintf(L->name, sizeof(L->name), "%s", name);
    L->address = addr;
    L->size = size;
    L->kind = kind;
    L->is_signed = is_signed;
    return L;
}

int main(void)
{
    wchar_t path[MAX_PATH], path2[MAX_PATH];
    char* text;
    int len = 0;

    memset(&g_app, 0, sizeof(g_app));
    leaf_at(0, "g_counter", 0x20000100, 4, OS_TYPE_INT, 1);
    leaf_at(1, "g_gain", 0x20000104, 4, OS_TYPE_FLOAT, 1);
    leaf_at(2, "g_flag", 0x20000108, 1, OS_TYPE_BOOL, 0);
    leaf_at(3, "const_table", 0x08001000, 2, OS_TYPE_UINT, 0);
    leaf_at(4, "big_buf", 0x20000200, 32, OS_TYPE_STRING, 0);
    leaf_at(5, "state", 0x2000010C, 4, OS_TYPE_ENUM, 1);
    g_app.leaves[5].enum_count = 2;
    _snprintf(g_app.leaves[5].enums[0].name, 64, "IDLE");
    g_app.leaves[5].enums[0].value = 0;
    _snprintf(g_app.leaves[5].enums[1].name, 64, "RUN");
    g_app.leaves[5].enums[1].value = 1;
    g_app.leaf_count = 6;

    GetTempPathW(MAX_PATH, path);
    wcscat(path, L"openscope_a2l_smoke.a2l");
    wcscpy(path2, path);

    expect(os_a2l_export(path, "ECU", 1) == 0, "A2L 导出（标定模式）");
    text = read_all(path, &len);
    expect(text != NULL && len > 0, "A2L 文件非空");
    if (text) {
        expect(strstr(text, "ASAP2_VERSION 1 71") != NULL, "ASAP2 版本");
        expect(strstr(text, "CHARACTERISTIC g_counter") != NULL, "RAM 标量 -> CHARACTERISTIC");
        expect(strstr(text, "VALUE 0x20000100 RL_SLONG") != NULL, "CHARACTERISTIC 类型/地址");
        expect(strstr(text, "MEASUREMENT const_table") != NULL, "Flash 常量 -> MEASUREMENT");
        expect(strstr(text, "ECU_ADDRESS 0x08001000") != NULL, "MEASUREMENT 地址");
        expect(strstr(text, "FLOAT32_IEEE") != NULL, "float32 类型");
        expect(strstr(text, "MATRIX_DIM 32") != NULL, "字节数组 MATRIX_DIM");
        expect(strstr(text, "COMPU_VTAB") != NULL, "枚举 COMPU_VTAB");
        expect(strstr(text, "BIT_MASK") == NULL, "普通变量无 BIT_MASK");
        expect(strstr(text, "BYTE_ORDER MSB_LAST") != NULL, "小端字节序");
        expect(strstr(text, "PROJECT_NO 1") != NULL, "HEADER PROJECT_NO");
        free(text);
    }

    expect(os_a2l_export(path, "ECU", 0) == 0, "A2L 导出（全测量模式）");
    text = read_all(path, &len);
    if (text) {
        expect(strstr(text, "CHARACTERISTIC g_counter") == NULL, "全测量模式无 CHARACTERISTIC");
        expect(strstr(text, "MEASUREMENT g_counter") != NULL, "全测量模式 g_counter");
        free(text);
    }

    DeleteFileW(path);
    DeleteFileW(path2);
    printf("a2l_smoke: %s\n", g_fail ? "FAILED" : "ALL PASS");
    return g_fail ? 1 : 0;
}
