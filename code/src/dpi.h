#ifndef OS_DPI_H
#define OS_DPI_H
#include <windows.h>

/* UI P0: per-monitor DPI v2 + shared UI metrics (Terminator-style thin gap). */

#define OS_GAP_VISUAL 2   /* visible gap between panes at 96 dpi (px) */
#define OS_GAP_HIT    6   /* splitter drag hit area at 96 dpi (px) */

/* Call once at startup before any window is created. Returns 2 = PMv2, 1 = system aware, 0 = none. */
int  os_dpi_init(void);
/* DPI of hwnd's monitor (96 if unknown / hwnd NULL -> system DPI). */
UINT os_dpi_of(HWND hwnd);
/* Current main-window DPI cached by os_dpi_set_current (WM_CREATE / WM_DPICHANGED). */
UINT os_dpi_current(void);
void os_dpi_set_current(UINT dpi);
/* Scale a 96-dpi pixel value to the current DPI (or a given dpi). */
int  os_px(int v96);
int  os_px_dpi(int v96, UINT dpi);
/* Scaled visual gap (>=1) and extra hit padding per side. */
int  os_gap(void);
int  os_gap_hit_pad(void);
/* GetSystemMetricsForDpi with fallback. */
int  os_metric(int index);

#endif