/* 块读合并预算：锁住“kHz 不是 MHz”的修正，避免再把空隙放大到 4096 字节。 */
#include "batchplan.h"
#include <stdio.h>

static int g_fail;

static void expect_int(const char* name, int got, int want)
{
    if (got == want) {
        printf("  PASS %s = %d\n", name, got);
        return;
    }
    printf("  FAIL %s = %d (want %d)\n", name, got, want);
    g_fail++;
}

int main(void)
{
    int gap4, run4, gap12, run12, gap50, run50, gap_lo;
    printf("batchplan smoke\n");

    gap4 = os_bp_gap_limit(4000);
    run4 = os_bp_run_limit(4000);
    gap12 = os_bp_gap_limit(12000);
    run12 = os_bp_run_limit(12000);
    gap50 = os_bp_gap_limit(50000);
    run50 = os_bp_run_limit(50000);
    gap_lo = os_bp_gap_limit(50);

    expect_int("4MHz gap", gap4, 100);
    expect_int("4MHz run", run4, 256);
    expect_int("12MHz gap", gap12, 300);
    expect_int("12MHz run", run12, 300);
    expect_int("50MHz gap", gap50, 1250);
    expect_int("50MHz run", run50, 1250);
    expect_int("50kHz gap floor", gap_lo, OS_BATCH_GAP_MIN);
    expect_int("speed 0 uses 4MHz gap", os_bp_gap_limit(0), 100);

    /* 旧公式 s*25 在 4MHz 得到 100000，再被夹到 4096。修正后不得回到这个值。 */
    expect_int("4MHz gap is not the 4096 clamp", gap4 == OS_BATCH_RUN_MAX, 0);

    expect_int("contiguous no split",
               os_bp_should_split(0x20000000, 0x20000004, 0x20000004, 4, gap4, run4), 0);
    expect_int("gap 100 stays in run",
               os_bp_should_split(0x20000000, 0x20000004, 0x20000004 + 100, 4, gap4, run4), 0);
    expect_int("gap 101 splits",
               os_bp_should_split(0x20000000, 0x20000004, 0x20000004 + 101, 4, gap4, run4), 1);
    expect_int("run longer than 256 splits",
               os_bp_should_split(0x20000000, 0x20000004, 0x20000000 + 300, 4, 4096, run4), 1);
    expect_int("same address alias shares the run",
               os_bp_should_split(0x20000010, 0x20000014, 0x20000010, 4, gap4, run4), 0);
    expect_int("address going backwards splits",
               os_bp_should_split(0x20000100, 0x20000104, 0x20000000, 4, gap4, run4), 1);

    if (g_fail) {
        printf("FAILED %d\n", g_fail);
        return 1;
    }
    printf("ALL PASS\n");
    return 0;
}
