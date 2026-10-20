/* .map 解析 + XCP 符号注册表冒烟测试（不依赖 ELF/硬件）。 */
#include "app.h"
#include "mapfile.h"
#include "xcpsym.h"

#include <stdio.h>
#include <string.h>
#include <windows.h>

/* xcpsym.c 依赖宿主全局表；本测试自建最小 g_app。 */
OS_App g_app;

static int g_fail;

static void expect(int ok, const char* what)
{
    printf("%s %s\n", ok ? "PASS" : "FAIL", what);
    if (!ok) g_fail++;
}

static int write_file(const char* path, const char* text)
{
    FILE* f = fopen(path, "wb");
    if (!f) return -1;
    fputs(text, f);
    fclose(f);
    return 0;
}

int main(int argc, char** argv)
{
    char dir[MAX_PATH], gnu[MAX_PATH], keil[MAX_PATH], err[256];
    OS_MapSym* syms = NULL;
    int n;

    GetTempPathA(MAX_PATH, dir);
    _snprintf(gnu, MAX_PATH, "%sopenscope_map_smoke_gnu.map", dir);
    _snprintf(keil, MAX_PATH, "%sopenscope_map_smoke_keil.map", dir);

    write_file(gnu,
        "Memory Configuration\n"
        "Name             Origin             Length             Attributes\n"
        "FLASH            0x0000000008000000 0x0000000000080000 xr\n"
        "\n"
        "Linker script and memory map\n"
        " .bss           0x0000000020000000      0x400\n"
        "                0x0000000020000000                . = ALIGN (4)\n"
        "                0x0000000020000000                g_counter\n"
        "                0x0000000020000004                g_gain\n"
        "                0x0000000020000008                g_buffer\n"
        "                0x0000000020000010                PROVIDE (__stack, .)\n"
        "                0x0000000020000014                0x4\n"
        " *(.bss)\n"
        " .bss.g_x       0x0000000020000020        0x4 build/main.o\n"
        "                0x0000000020000020                g_x\n"
        "                0x0000000020000020                g_counter\n");
    n = os_map_load(gnu, &syms, err, sizeof(err));
    expect(n >= 0, "GNU map 解析");
    expect(n == 4, "GNU map 符号数=4（去重/过滤）");
    {
        int i, found = 0;
        for (i = 0; i < n; i++) {
            if (!strcmp(syms[i].name, "g_counter")) {
                expect(syms[i].address == 0x20000000ULL, "GNU g_counter 地址");
                found++;
            }
            if (!strcmp(syms[i].name, "g_x"))
                expect(syms[i].address == 0x20000020ULL, "GNU g_x 地址");
            expect(strcmp(syms[i].name, "PROVIDE") != 0, "GNU 过滤 PROVIDE");
            expect(strcmp(syms[i].name, ".") != 0, "GNU 过滤 '.'");
            expect(strstr(syms[i].name, ".o") == NULL, "GNU 过滤对象文件");
        }
        expect(found == 1, "GNU 同名符号去重");
    }
    os_map_free(syms);

    write_file(keil,
        "Global Symbols\n"
        "Symbol Name                              Value     Ov Type        Size  Object(Section)\n"
        "g_counter                                0x20000000   Data           4  main.o(.data)\n"
        "g_gain                                   0x20000004   Data           4  main.o(.data)\n"
        "g_big                                    0x20000010   Data         128  main.o(.bss)\n"
        "SystemInit                               0x08000100   Thumb Code   132  system.o(.text)\n");
    syms = NULL;
    n = os_map_load(keil, &syms, err, sizeof(err));
    expect(n == 4, "Keil map 符号数=4");
    if (n > 0) {
        int i;
        for (i = 0; i < n; i++) {
            if (!strcmp(syms[i].name, "g_big"))
                expect(syms[i].size == 128, "Keil g_big 大小=128");
            if (!strcmp(syms[i].name, "g_counter"))
                expect(syms[i].address == 0x20000000ULL, "Keil g_counter 地址");
        }
    }
    os_map_free(syms);

    DeleteFileA(gnu);
    DeleteFileA(keil);

    /* ---- XCP 符号注册表：ELF 叶优先于同名 MAP 符号 ---- */
    memset(&g_app, 0, sizeof(g_app));
    _snprintf(g_app.leaves[0].name, sizeof(g_app.leaves[0].name), "g_counter");
    g_app.leaves[0].address = 0x20000100;
    g_app.leaves[0].size = 4;
    g_app.leaves[0].kind = OS_TYPE_INT;
    g_app.leaves[0].is_signed = 1;
    g_app.leaves[1].id = 1;
    _snprintf(g_app.leaves[1].name, sizeof(g_app.leaves[1].name), "g_flag");
    g_app.leaves[1].address = 0x20000104;
    g_app.leaves[1].size = 1;
    g_app.leaves[1].kind = OS_TYPE_UINT;
    g_app.leaf_count = 2;

    {
        wchar_t wmap[MAX_PATH];
        char map2[MAX_PATH];
        _snprintf(map2, MAX_PATH, "%sopenscope_map_smoke_reg.map", dir);
        write_file(map2,
            "                0x20000200                g_counter\n"
            "                0x20000204                g_only_map\n");
        MultiByteToWideChar(CP_UTF8, 0, map2, -1, wmap, MAX_PATH);
        n = os_xcpsym_load_map(wmap, err, sizeof(err));
        expect(n == 2, "注册表 MAP 载入 2 个符号");
        DeleteFileA(map2);
    }
    os_xcpsym_invalidate();
    expect(os_xcpsym_count() == 3, "注册表合并后 3 个符号（ELF 2 + MAP 1）");
    {
        OS_XcpSym s;
        expect(os_xcpsym_find("g_counter", &s) == 0 &&
               s.address == 0x20000100 && s.leaf_id == 0,
               "同名符号优先使用 ELF 地址/类型");
        expect(os_xcpsym_find("g_only_map", &s) == 0 &&
               s.from_map == 1 && s.size == 4,
               "MAP 独有符号可查询");
    }

    /* ---- 值编码 / 格式化 ---- */
    {
        OS_XcpSym s;
        uint8_t b[8];
        char out[64];
        memset(&s, 0, sizeof(s));
        s.size = 2;
        s.is_signed = 1;
        expect(os_xcpsym_encode_value(&s, "-2", b, 8, NULL, 0) == 2 &&
               b[0] == 0xFE && b[1] == 0xFF, "有符号 16 位编码 -2");
        expect(os_xcpsym_format_value(&s, b, 2, out, sizeof(out)) == 0 &&
               !strcmp(out, "-2"), "有符号 16 位格式化");

        memset(&s, 0, sizeof(s));
        s.size = 4;
        s.is_float = 1;
        expect(os_xcpsym_encode_value(&s, "1.5", b, 8, NULL, 0) == 4, "float32 编码");
        expect(os_xcpsym_format_value(&s, b, 4, out, sizeof(out)) == 0 &&
               atof(out) > 1.49 && atof(out) < 1.51, "float32 格式化");

        memset(&s, 0, sizeof(s));
        s.size = 1;
        s.is_bitfield = 1;
        s.bit_offset = 2;
        s.bit_size = 3;
        {
            uint8_t raw[1] = { 0x9C }; /* 100 111 00 -> bits 2..4 = 111b = 7 */
            uint8_t enc[8];
            expect(os_xcpsym_encode_value(&s, "5", enc, 8, NULL, 0) == 1, "位域值编码");
            expect(os_xcpsym_format_value(&s, raw, 1, out, sizeof(out)) == 0 &&
                   !strcmp(out, "7"), "位域提取");
        }
    }

    /* 可选：解析真实 Keil/GNU map（例如 G431 工程），验证 0x2000'0248 分隔格式 */
    if (argc > 1) {
        syms = NULL;
        n = os_map_load(argv[1], &syms, err, sizeof(err));
        printf("real map: %s -> %d symbols\n", argv[1], n);
        if (n > 0) {
            int i, found = 0;
            for (i = 0; i < n; i++) {
                if (!strcmp(syms[i].name, "xcp_cal_mem")) {
                    printf("  xcp_cal_mem addr=0x%llX size=%u\n",
                           (unsigned long long)syms[i].address, (unsigned)syms[i].size);
                    expect(syms[i].address == 0x20000248ULL, "真实 map xcp_cal_mem 地址");
                    expect(syms[i].size == 0x100, "真实 map xcp_cal_mem 大小");
                    found = 1;
                }
            }
            expect(found, "真实 map 找到 xcp_cal_mem");
        }
        os_map_free(syms);
    }

    printf("mapfile_smoke: %s\n", g_fail ? "FAILED" : "ALL PASS");
    return g_fail ? 1 : 0;
}
