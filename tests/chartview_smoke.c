/*
 * OpenScope F23 波形视图数学单元测试（chartview.c 纯函数）。
 * 覆盖：滚轮缩放锚点保持 / span 收窄 / 1ms 最小窗 / full 夹紧 / 无变化判定，
 *      框选 X/Y 映射，退化输入（空窗/零高/框退化成点）不崩溃不越界。
 * 无 Win32 依赖，独立可执行；接入 build_tests.bat 每次构建自动回归。
 */
#include "chartview.h"
#include <stdio.h>
#include <stdint.h>

static int g_fails = 0;

static void check(int ok, const char* what)
{
    printf("%s %s\n", ok ? "PASS" : "FAIL", what);
    if (!ok) g_fails++;
}

int main(void)
{
    int64_t nx0, nx1;
    double ny0, ny1, nyhi, nylo;

    /* ---- os_cv_zoom_x：锚点比例保持 ---- */
    nx0 = nx1 = -1;
    check(os_cv_zoom_x(0, 100000, 0.5, 0.8, INT64_MIN, INT64_MAX, &nx0, &nx1) == 1,
          "zoom_x f=0.5 factor=0.8 returns changed");
    check(nx0 == 10000 && nx1 == 90000, "zoom_x anchor 0.5 preserved (10000..90000)");
    check(nx1 - nx0 == 80000, "zoom_x span shrinks by 0.8");

    check(os_cv_zoom_x(0, 100000, 0.0, 0.8, INT64_MIN, INT64_MAX, &nx0, &nx1) == 1
          && nx0 == 0 && nx1 == 80000, "zoom_x anchor 0.0 keeps left edge");
    check(os_cv_zoom_x(0, 100000, 1.0, 0.8, INT64_MIN, INT64_MAX, &nx0, &nx1) == 1
          && nx0 == 20000 && nx1 == 100000, "zoom_x anchor 1.0 keeps right edge");

    /* ---- os_cv_zoom_x：最小窗 1ms 夹紧 ---- */
    check(os_cv_zoom_x(0, 500, 0.5, 0.1, INT64_MIN, INT64_MAX, &nx0, &nx1) == 1
          && (nx1 - nx0) == 1000, "zoom_x clamps span to 1ms minimum");

    /* ---- os_cv_zoom_x：放大出数据范围后夹紧到 full ---- */
    check(os_cv_zoom_x(10000, 90000, 0.5, 2.0, 0, 100000, &nx0, &nx1) == 1
          && nx0 == 0 && nx1 == 100000, "zoom_x clamps zoom-out to full range");

    /* ---- os_cv_zoom_x：已在最小窗继续放大 => 无变化 ---- */
    check(os_cv_zoom_x(0, 1000, 0.5, 0.8, INT64_MIN, INT64_MAX, &nx0, &nx1) == 0,
          "zoom_x no-op when already at min span");

    /* ---- os_cv_zoom_x：退化输入 ---- */
    check(os_cv_zoom_x(1000, 1000, 0.5, 0.8, INT64_MIN, INT64_MAX, &nx0, &nx1) == 0,
          "zoom_x degenerate x1<=x0 returns 0");
    check(os_cv_zoom_x(0, 1000, 0.5, 0.8, INT64_MIN, INT64_MAX, NULL, &nx1) == 0,
          "zoom_x NULL out args safe");

    /* ---- os_cv_zoom_y：锚点比例保持 ---- */
    check(os_cv_zoom_y(0.0, 10.0, 0.5, 0.8, &ny0, &ny1) == 1
          && ny0 > 0.99 && ny0 < 1.01 && ny1 > 8.99 && ny1 < 9.01,
          "zoom_y anchor 0.5 preserved");
    check(os_cv_zoom_y(0.0, 10.0, 0.0, 0.8, &ny0, &ny1) == 1
          && ny0 > -1e-9 && ny1 > 7.99 && ny1 < 8.01, "zoom_y anchor 0.0 keeps bottom");
    check(os_cv_zoom_y(0.0, 10.0, 1.0, 0.8, &ny0, &ny1) == 1
          && ny0 > 1.99 && ny0 < 2.01 && ny1 > 9.99 && ny1 < 10.01, "zoom_y anchor 1.0 keeps top");
    check(os_cv_zoom_y(5.0, 5.0, 0.5, 0.8, &ny0, &ny1) == 0, "zoom_y degenerate yhi==ylo returns 0");

    /* ---- os_cv_box_x：框选区间映射 ---- */
    os_cv_box_x(0, 100000, 56, 500, 200, 400, &nx0, &nx1);
    check(nx0 == 32432 && nx1 == 77477, "box_x maps box [200,400] to [32432,77477]");
    os_cv_box_x(0, 100000, 56, 500, 400, 200, &nx0, &nx1);  /* 反向框选 br<=bl 由函数兜底 */
    check(nx0 == 0 && nx1 == 100000, "box_x reverse box left unchanged");
    os_cv_box_x(0, 100000, 56, 56, 100, 200, &nx0, &nx1);   /* 零宽绘图区 */
    check(nx0 == 0 && nx1 == 100000, "box_x zero-width plot leaves range unchanged");

    /* ---- os_cv_box_y：框选 Y 映射（顶框边=高值，底框边=低值） ---- */
    os_cv_box_y(10.0, 0.0, 26, 200, 50, 100, &nyhi, &nylo);
    check(nyhi > 8.6 && nyhi < 8.65 && nylo > 5.7 && nylo < 5.8 && nyhi > nylo,
          "box_y maps box [50,100] to yhi>nylo subrange");
    os_cv_box_y(10.0, 0.0, 26, 200, 100, 50, &nyhi, &nylo);
    check(nyhi == 10.0 && nylo == 0.0, "box_y reverse box leaves range unchanged");
    os_cv_box_y(10.0, 0.0, 200, 200, 50, 100, &nyhi, &nylo); /* 零高绘图区 */
    check(nyhi == 10.0 && nylo == 0.0, "box_y zero-height plot leaves range unchanged");

    printf(g_fails ? "FAILURES: %d\n" : "ALL PASS\n", g_fails);
    return g_fails;
}
