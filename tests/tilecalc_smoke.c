/*
 * OpenScope Bug6 tab 平铺列宽比例单元测试（tilecalc.c 纯函数）。
 * 覆盖：均分初始化 / 拖拽此消彼长 / 最小宽度夹紧 / 和保持为 1 /
 *      drag2 任意下标 / 退化输入（NULL、越界、total<=0、n<2）不崩溃不越界。
 * 无 Win32 依赖，独立可执行；接入 build_tests.bat 每次构建自动回归。
 */
#include "tilecalc.h"
#include <stdio.h>

static int g_fails = 0;

static void check(int ok, const char* what)
{
    printf("%s %s\n", ok ? "PASS" : "FAIL", what);
    if (!ok) g_fails++;
}

static int near(double a, double b)
{
    double d = a - b;
    if (d < 0) d = -d;
    return d < 1e-9;
}

int main(void)
{
    double r[8];

    /* ---- 初始化均分 ---- */
    os_tile_ratios_init(r, 3);
    check(near(r[0], 1.0 / 3) && near(r[1], 1.0 / 3) && near(r[2], 1.0 / 3),
          "init 3 columns equal thirds");
    check(near(r[0] + r[1] + r[2], 1.0), "init sum is 1.0");
    os_tile_ratios_init(r, 1);
    check(near(r[0], 1.0), "init single column gets 1.0");
    os_tile_ratios_init(NULL, 3); /* 不崩溃 */
    os_tile_ratios_init(r, 0);
    check(1, "init NULL/n=0 safe");

    /* ---- 拖拽：两列此消彼长，和保持 1 ---- */
    os_tile_ratios_init(r, 2);
    check(os_tile_ratios_drag(r, 2, 0, 1000, 100, 60) == 1, "drag right 100px changes");
    check(near(r[0], 0.6) && near(r[1], 0.4), "drag +100px of 1000 -> 0.6/0.4");
    check(near(r[0] + r[1], 1.0), "drag keeps sum 1.0");
    check(os_tile_ratios_drag(r, 2, 0, 1000, -200, 60) == 1
          && near(r[0], 0.4) && near(r[1], 0.6), "drag -200px reverses to 0.4/0.6");

    /* ---- 最小宽度夹紧：60px/1000 = 0.06 ---- */
    os_tile_ratios_init(r, 2);
    check(os_tile_ratios_drag(r, 2, 0, 1000, -900, 60) == 1
          && near(r[0], 0.06) && near(r[1], 0.94), "drag huge left clamps to min 0.06");
    check(os_tile_ratios_drag(r, 2, 0, 1000, -900, 60) == 0,
          "already at min, further left is no-op");
    check(near(r[0] + r[1], 1.0), "sum still 1.0 after clamp");

    /* ---- 三列拖中间分隔：只动相邻两列 ---- */
    os_tile_ratios_init(r, 3);
    check(os_tile_ratios_drag(r, 3, 1, 900, 90, 60) == 1, "3-col drag gap1 changes");
    check(near(r[0], 1.0 / 3), "3-col drag gap1 leaves col0 unchanged");
    check(near(r[1] + r[2], 2.0 / 3), "3-col drag gap1 pair sum preserved");
    check(near(r[0] + r[1] + r[2], 1.0), "3-col total sum still 1.0");

    /* ---- drag2：任意下标（跳过中间最小化列的场景） ---- */
    os_tile_ratios_init(r, 3);
    check(os_tile_ratios_drag2(r, 0, 2, 1000, 200, 60) == 1
          && near(r[0], 1.0 / 3 + 0.2) && near(r[2], 1.0 / 3 - 0.2),
          "drag2 adjusts columns 0 and 2 directly");
    check(near(r[0] + r[1] + r[2], 1.0), "drag2 keeps total sum 1.0");
    check(os_tile_ratios_drag2(r, 1, 1, 1000, 100, 60) == 0, "drag2 same index is no-op");

    /* ---- 退化输入 ---- */
    check(os_tile_ratios_drag(NULL, 2, 0, 1000, 100, 60) == 0, "drag NULL ratios safe");
    check(os_tile_ratios_drag(r, 1, 0, 1000, 100, 60) == 0, "drag n<2 returns 0");
    check(os_tile_ratios_drag(r, 3, 5, 1000, 100, 60) == 0, "drag gap index out of range");
    check(os_tile_ratios_drag(r, 3, 0, 0, 100, 60) == 0, "drag total_px=0 returns 0");
    check(os_tile_ratios_drag(r, 3, 0, -100, 100, 60) == 0, "drag negative total returns 0");
    check(os_tile_ratios_drag2(NULL, 0, 1, 1000, 100, 60) == 0, "drag2 NULL safe");
    check(os_tile_ratios_drag2(r, -1, 1, 1000, 100, 60) == 0, "drag2 negative index safe");

    /* ---- 双向夹紧仍越界：两列总宽不足 2*min ---- */
    r[0] = 0.1; r[1] = 0.02; r[2] = 0.88;
    check(os_tile_ratios_drag(r, 3, 0, 1000, 500, 120) == 0,
          "pair smaller than 2*min is no-op");

    printf("%s\n", g_fails ? "FAILURES" : "ALL PASS");
    return g_fails;
}
