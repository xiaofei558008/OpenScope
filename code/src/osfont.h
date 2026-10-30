#ifndef OS_FONT_H
#define OS_FONT_H
#include <windows.h>
/* UI P0: private JetBrains Mono (FR_PRIVATE from <exe>\fonts or D:\OpenScope\fonts),
 * fallback Consolas, CJK fallback Microsoft YaHei. Never installs anything system-wide. */
void           os_font_init(void);              /* once at startup, after os_dpi_init */
const wchar_t* os_font_mono_face(void);         /* "JetBrains Mono" or "Consolas" */
const wchar_t* os_font_ui_face(void);           /* "Microsoft YaHei UI" or "Segoe UI" */
HFONT          os_font_mono(void);              /* cached, 13px@96 dpi, do not delete */
HFONT          os_font_ui(void);                /* cached, 12px@96 dpi, do not delete */
HFONT          os_font_create(const wchar_t* face, int px96, int weight); /* caller deletes */
void           os_font_dpi_changed(void);       /* rebuild cached fonts */
#endif