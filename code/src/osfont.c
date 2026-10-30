/* osfont.c - see osfont.h */
#include "osfont.h"
#include "dpi.h"
#include <wchar.h>

static wchar_t g_mono[LF_FACESIZE] = L"Consolas";
static wchar_t g_ui[LF_FACESIZE] = L"Segoe UI";
static HFONT g_hmono, g_hui;

static int CALLBACK enum_cb(const LOGFONTW* lf, const TEXTMETRICW* tm, DWORD type, LPARAM lp)
{
    (void)lf; (void)tm; (void)type;
    *(int*)lp = 1;
    return 0;
}

static int face_exists(const wchar_t* face)
{
    LOGFONTW lf;
    int found = 0;
    HDC dc = GetDC(NULL);
    ZeroMemory(&lf, sizeof(lf));
    lf.lfCharSet = DEFAULT_CHARSET;
    wcsncpy(lf.lfFaceName, face, LF_FACESIZE - 1);
    EnumFontFamiliesExW(dc, &lf, enum_cb, (LPARAM)&found, 0);
    ReleaseDC(NULL, dc);
    return found;
}

static int load_dir(const wchar_t* dir)
{
    wchar_t pat[MAX_PATH], path[MAX_PATH];
    WIN32_FIND_DATAW fd;
    HANDLE h;
    int n = 0;
    _snwprintf(pat, MAX_PATH, L"%s\\JetBrainsMono*.ttf", dir);
    pat[MAX_PATH - 1] = 0;
    h = FindFirstFileW(pat, &fd);
    if (h == INVALID_HANDLE_VALUE) return 0;
    do {
        _snwprintf(path, MAX_PATH, L"%s\\%s", dir, fd.cFileName);
        path[MAX_PATH - 1] = 0;
        if (AddFontResourceExW(path, FR_PRIVATE, 0) > 0) n++;
    } while (FindNextFileW(h, &fd));
    FindClose(h);
    return n;
}

void os_font_init(void)
{
    wchar_t dir[MAX_PATH], *p;
    if (!GetModuleFileNameW(NULL, dir, MAX_PATH)) dir[0] = 0;
    p = wcsrchr(dir, L'\\');
    if (p) {
        int n;
        wcscpy(p, L"\\fonts");
        n = load_dir(dir);
        if (!n) { *p = 0; p = wcsrchr(dir, L'\\'); if (p) { wcscpy(p, L"\\fonts"); n = load_dir(dir); } } /* bin\Release\.. */
        if (!n) load_dir(L"D:\\OpenScope\\fonts");
    }
    if (face_exists(L"JetBrains Mono")) wcscpy(g_mono, L"JetBrains Mono");
    else if (face_exists(L"Consolas")) wcscpy(g_mono, L"Consolas");
    else wcscpy(g_mono, L"Microsoft YaHei");
    if (face_exists(L"Microsoft YaHei UI")) wcscpy(g_ui, L"Microsoft YaHei UI");
    else if (face_exists(L"Microsoft YaHei")) wcscpy(g_ui, L"Microsoft YaHei");
}

const wchar_t* os_font_mono_face(void) { return g_mono; }
const wchar_t* os_font_ui_face(void) { return g_ui; }

HFONT os_font_create(const wchar_t* face, int px96, int weight)
{
    return CreateFontW(-os_px(px96), 0, 0, 0, weight, 0, 0, 0, DEFAULT_CHARSET,
                       OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                       DEFAULT_PITCH | FF_DONTCARE, face);
}

HFONT os_font_mono(void)
{
    if (!g_hmono) g_hmono = os_font_create(g_mono, 13, FW_NORMAL);
    return g_hmono;
}

HFONT os_font_ui(void)
{
    if (!g_hui) g_hui = os_font_create(g_ui, 12, FW_NORMAL);
    return g_hui;
}

void os_font_dpi_changed(void)
{
    /* old handles may still be selected in child windows; leak-free swap happens on next WM_SETFONT */
    HFONT a = g_hmono, b = g_hui;
    g_hmono = g_hui = NULL;
    if (a) DeleteObject(a);
    if (b) DeleteObject(b);
}