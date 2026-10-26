/* 第五轮：行表映射回归（行 → 断点地址、PC → 行）。
 * 1) line_sample.out（gen_line_elf.py 生成，最小 ELF，只含 .debug_line）：逐条断言；
 * 2) 可选：环境变量 OS_TC375_ELF 指向实板 tc375_scope.elf（fw_main.c 构建）时，断言实板上发现的几处。
 * 构建：./build.sh 或 build_msvc.bat */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "elf.h"

static int g_fail;
#define CHECK(c, ...) do { if (!(c)) { g_fail++; printf("  FAIL %s:%d: ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); } } while (0)

static void expect_line(OS_ElfFile* f, int line, unsigned want_addr, int want_back)
{
    uint32_t a = 0; char fl[512]; int back = -1;
    int ok = os_elf_addr_of_line(f, "main.c", line, &a);
    if (ok && !os_elf_line_of_addr(f, a, fl, sizeof(fl), &back)) back = 0;
    printf("  main.c:%-3d -> 0x%08X -> 行 %d\n", line, (unsigned)a, back);
    CHECK(ok && a == want_addr, "line %d: addr 0x%08X, want 0x%08X", line, (unsigned)a, want_addr);
    CHECK(back == want_back, "line %d: back line %d, want %d", line, back, want_back);
}

static void expect_pc(OS_ElfFile* f, unsigned pc, int want_line)
{
    char fl[512]; int ln = 0;
    int ok = os_elf_line_of_addr(f, pc, fl, sizeof(fl), &ln);
    if (!ok) ln = 0;
    printf("  PC 0x%08X -> 行 %d\n", pc, ln);
    CHECK(ln == want_line, "pc 0x%08X: line %d, want %d", pc, ln, want_line);
}

int main(int argc, char** argv)
{
    char err[256];
    const char* sample = argc > 1 ? argv[1] : "line_sample.out";
    const char* tc = getenv("OS_TC375_ELF");
    OS_ElfFile* f = os_elf_open(sample, err, sizeof(err));
    printf("[1] %s\n", sample);
    CHECK(f != NULL, "open %s: %s", sample, err);
    if (f) {
        expect_line(f, 10, 0x80001000u, 10);
        expect_line(f, 12, 0x80001008u, 13);   /* 空行 → 同函数下一条有代码的行（旧：上一行 11） */
        expect_line(f, 14, 0x8000100Cu, 16);   /* 只有零长度行：代码并入 16，命中时显示 16（如实） */
        expect_line(f, 15, 0x8000100Cu, 16);
        expect_line(f, 17, 0x80001010u, 17);   /* 跳过 non-stmt 行 0x8000100E（旧：取第一行） */
        expect_line(f, 30, 0x80001014u, 30);   /* 旧：PC→行 被序列 A 的结束行抢成 17 */
        expect_pc(f, 0x8000100Eu, 17);
        expect_pc(f, 0x80001014u, 30);
        expect_pc(f, 0x8000101Cu, 31);
        expect_pc(f, 0x80001020u, 0);          /* 序列外：无行（旧：31） */
        expect_pc(f, 0x80000FFEu, 0);
        os_elf_close(f);
    }
    if (tc && tc[0]) {
        printf("[2] 实板 ELF %s\n", tc);
        f = os_elf_open(tc, err, sizeof(err));
        CHECK(f != NULL, "open %s: %s", tc, err);
        if (f) {
            expect_line(f, 71, 0x80002860u, 71);   /* main 入口（旧：PC→行 报 stm_hz 的 57 行） */
            expect_line(f, 85, 0x800028CAu, 85);   /* g_loop_cnt++ */
            expect_line(f, 86, 0x800028D4u, 87);   /* 空行 → if (g_cfg.enable)（旧：0x800028CE，第 85 行中间的 add） */
            expect_line(f, 88, 0x800028DAu, 88);
            expect_line(f, 95, 0x80002906u, 95);   /* 旧：0x80002900（94/95 同地址的零长度行）→ 显示 94 */
            expect_pc(f, 0x80002A9Eu, 65);         /* stm_hz 首地址 = main 序列结束地址 */
            os_elf_close(f);
        }
    } else {
        printf("[2] 跳过（设置 OS_TC375_ELF=实板 tc375_scope.elf 可加跑）\n");
    }
    printf(g_fail ? "RESULT: FAIL (%d)\n" : "RESULT: PASS\n", g_fail);
    return g_fail ? 1 : 0;
}
