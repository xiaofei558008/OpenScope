/*
 * OpenScope scope window module (scope.dll)
 *
 * Capability: OS_CAP_WINDOW
 *   - window type "scope.bar" (示波器窗口): realtime curve window
 *   - add variable via fuzzy search dialog (fw->leaf_find)
 *   - series management (add / remove / write value)
 *   - realtime polyline drawing with rolling history
 *   - write-back via fw->write_leaf, written samples marked
 */
#include "module_api.h"

#include <windows.h>
#include <commctrl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SCOPE_MAX_SERIES  16
#define SCOPE_HIST        1024
#define SCOPE_MAX_WINS    16

typedef struct ScopeSeries {
    int       leaf_id;
    char      name[256];
    COLORREF  color;
    int64_t   ts[SCOPE_HIST];
    double    vals[SCOPE_HIST];
    int       written[SCOPE_HIST];
    int       head;      /* next write position */
    int       count;     /* valid samples */
} ScopeSeries;

typedef struct ScopeWin {
    HWND        hwnd;
    HWND        hBtnAdd, hBtnDel, hBtnWrite, hBtnClose;
    ScopeSeries series[SCOPE_MAX_SERIES];
    int         nseries;
    int         sel;      /* selected legend row, -1 = none */
} ScopeWin;

typedef struct ScopeMod {
    ScopeWin* wins[SCOPE_MAX_WINS];
    int       nwins;
} ScopeMod;

/* ---------- pick-variable dialog ---------- */
#define IDC_P_EDIT    101
#define IDC_P_LIST    102
#define IDC_P_OK      103
#define IDC_P_CANCEL  104

typedef struct PickState {
    int ids[512];
    int count;
    int chosen;
} PickState;

/* ---------- write-value dialog ---------- */
#define IDC_V_EDIT    201
#define IDC_V_OK      202
#define IDC_V_CANCEL  203
#define IDC_V_INFO    204

typedef struct ValState {
    int   leaf_id;
    char  name[256];
    double current;
} ValState;

static const OS_Framework* g_fw;
static const char* g_win_class = "OSScopeWin";
static const char* g_pick_class = "OSScopePick";
static const char* g_val_class = "OSScopeVal";
static wchar_t g_wcls_win[64], g_wcls_pick[64], g_wcls_val[64];

static const COLORREF g_palette[8] = {
    RGB(255, 96, 96),  RGB(96, 200, 255), RGB(120, 230, 120),
    RGB(255, 200, 80), RGB(220, 140, 255), RGB(80, 255, 220),
    RGB(255, 160, 120), RGB(180, 180, 200)
};

/* ---------------- UTF-8 helpers ---------------- */

static void wide_to_utf8_buf(const wchar_t* s, char* out, int outlen)
{
    if (outlen <= 0) return;
    WideCharToMultiByte(CP_UTF8, 0, s, -1, out, outlen, NULL, NULL);
    out[outlen - 1] = 0;
}

static void utf8_to_wide_buf(const char* s, wchar_t* out, int outlen)
{
    if (outlen <= 0) return;
    MultiByteToWideChar(CP_UTF8, 0, s ? s : "", -1, out, outlen);
    out[outlen - 1] = 0;
}

/* ---------------- series helpers ---------------- */

static void series_append(ScopeSeries* s, const OS_Sample* smp)
{
    s->ts[s->head] = smp->ts_us;
    s->vals[s->head] = smp->value;
    s->written[s->head] = smp->written ? 1 : 0;
    s->head = (s->head + 1) % SCOPE_HIST;
    if (s->count < SCOPE_HIST) s->count++;
}

static int series_find_name(ScopeSeries* s, const char* name)
{
    int i;
    for (i = 0; i < g_fw->leaf_count(); i++) {
        const char* n = g_fw->leaf_name(i);
        if (n && strcmp(n, name) == 0) return i;
    }
    return -1;
}

static void resolve_ids(ScopeWin* w)
{
    int i;
    for (i = 0; i < w->nseries; i++) {
        ScopeSeries* s = &w->series[i];
        s->leaf_id = series_find_name(s, s->name);
    }
    InvalidateRect(w->hwnd, NULL, TRUE);
}

/* ---------------- paint ---------------- */

static void scope_paint(ScopeWin* w)
{
    PAINTSTRUCT ps;
    HDC hdc, mem;
    HBITMAP bmp, oldbmp;
    RECT rc;
    HBRUSH bg = CreateSolidBrush(RGB(18, 20, 26));
    HPEN gridpen = CreatePen(PS_SOLID, 1, RGB(48, 52, 62));
    int top = 30, i, k;
    double ymin = 0, ymax = 1;
    RECT plot;

    hdc = BeginPaint(w->hwnd, &ps);
    GetClientRect(w->hwnd, &rc);
    mem = CreateCompatibleDC(hdc);
    bmp = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
    oldbmp = (HBITMAP)SelectObject(mem, bmp);

    FillRect(mem, &rc, bg);
    plot = rc;
    plot.top += top;
    plot.left += 56;
    plot.bottom -= 18;
    plot.right -= 6;

    /* grid */
    {
        int gx, gy;
        HPEN oldp = (HPEN)SelectObject(mem, gridpen);
        for (gx = plot.left; gx < plot.right; gx += 48) {
            MoveToEx(mem, gx, plot.top, NULL);
            LineTo(mem, gx, plot.bottom);
        }
        for (gy = plot.top; gy < plot.bottom; gy += 40) {
            MoveToEx(mem, plot.left, gy, NULL);
            LineTo(mem, plot.right, gy);
        }
        SelectObject(mem, oldp);
    }

    /* autoscale Y over visible series */
    {
        int have = 0;
        for (i = 0; i < w->nseries; i++) {
            ScopeSeries* s = &w->series[i];
            int n = s->count > SCOPE_HIST ? SCOPE_HIST : s->count;
            for (k = 0; k < n; k++) {
                int idx = (s->head - n + k + SCOPE_HIST) % SCOPE_HIST;
                double v = s->vals[idx];
                if (!have) { ymin = ymax = v; have = 1; }
                if (v < ymin) ymin = v;
                if (v > ymax) ymax = v;
            }
        }
        if (have) {
            double pad = (ymax - ymin) * 0.1;
            if (pad < 1e-12) pad = 1;
            ymin -= pad; ymax += pad;
        }

        /* series polylines */
        for (i = 0; i < w->nseries; i++) {
            ScopeSeries* s = &w->series[i];
            HPEN pen = CreatePen(PS_SOLID, 2, s->color);
            HPEN oldpen = (HPEN)SelectObject(mem, pen);
            int n = s->count > SCOPE_HIST ? SCOPE_HIST : s->count;
            for (k = 0; k < n; k++) {
                int idx = (s->head - n + k + SCOPE_HIST) % SCOPE_HIST;
                double v = s->vals[idx];
                int x, y;
                if (n <= 1) { x = plot.left; y = plot.bottom; }
                else x = plot.left + (plot.right - plot.left) * k / (n - 1);
                y = plot.top + (int)((ymax - v) / (ymax - ymin) * (plot.bottom - plot.top));
                if (y < plot.top) y = plot.top;
                if (y > plot.bottom) y = plot.bottom;
                if (k == 0) MoveToEx(mem, x, y, NULL);
                else LineTo(mem, x, y);
                if (s->written[idx]) {
                    HBRUSH wh = CreateSolidBrush(RGB(255, 255, 255));
                    RECT sq;
                    sq.left = x - 3; sq.top = y - 3; sq.right = x + 3; sq.bottom = y + 3;
                    FillRect(mem, &sq, wh);
                    DeleteObject(wh);
                }
            }
            SelectObject(mem, oldpen);
            DeleteObject(pen);
        }
    }

    /* Y 轴刻度（左侧槽位） */
    {
        HFONT font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
        HFONT oldfont = (HFONT)SelectObject(mem, font);
        int kk;
        SetBkMode(mem, TRANSPARENT);
        SetTextColor(mem, RGB(150, 150, 160));
        for (kk = 0; kk <= 4; kk++) {
            double v = ymax - (ymax - ymin) * kk / 4.0;
            int y = plot.top + (plot.bottom - plot.top) * kk / 4;
            char txt[64];
            wchar_t wt[64];
            RECT r;
            _snprintf(txt, sizeof(txt), "%.3g", v);
            utf8_to_wide_buf(txt, wt, 64);
            r.left = 2; r.top = y - 8; r.right = plot.left - 4; r.bottom = y + 8;
            DrawTextW(mem, wt, -1, &r, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
        }
        SelectObject(mem, oldfont);
    }

    /* 时间轴（底部） */
    {
        HFONT font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
        HFONT oldfont = (HFONT)SelectObject(mem, font);
        int64_t t0 = 0, t1 = 0;
        int have_t = 0, kk, nvis = 0;
        SetBkMode(mem, TRANSPARENT);
        SetTextColor(mem, RGB(150, 150, 160));
        for (i = 0; i < w->nseries; i++) {
            ScopeSeries* s = &w->series[i];
            int n = s->count > SCOPE_HIST ? SCOPE_HIST : s->count;
            for (k = 0; k < n; k++) {
                int idx = (s->head - n + k + SCOPE_HIST) % SCOPE_HIST;
                nvis++;
                if (s->ts[idx] && s->ts[idx] != -1) {
                    if (!have_t) { t0 = t1 = s->ts[idx]; have_t = 1; }
                    if (s->ts[idx] < t0) t0 = s->ts[idx];
                    if (s->ts[idx] > t1) t1 = s->ts[idx];
                }
            }
        }
        for (kk = 0; kk <= 4; kk++) {
            int x = plot.left + (plot.right - plot.left) * kk / 4;
            double sec = have_t ? (double)(t1 - t0) / 1e6 * kk / 4.0
                                : (double)nvis * kk / 4.0;
            wchar_t wt[64];
            RECT r;
            if (sec >= 60.0) _snwprintf(wt, 64, L"%dm%02ds", (int)(sec / 60.0), (int)sec % 60);
            else if (sec >= 1.0) _snwprintf(wt, 64, L"%.1fs", sec);
            else if (sec >= 0.001) _snwprintf(wt, 64, L"%.0fms", sec * 1000.0);
            else _snwprintf(wt, 64, L"0");
            r.left = x - 40; r.top = plot.bottom + 1;
            r.right = x + 40; r.bottom = rc.bottom - 1;
            DrawTextW(mem, wt, -1, &r, DT_CENTER | DT_TOP | DT_SINGLELINE);
        }
        SelectObject(mem, oldfont);
    }

    /* 无数据提示 */
    if (w->nseries > 0) {
        int total = 0;
        for (i = 0; i < w->nseries; i++) total += w->series[i].count;
        if (total == 0) {
            HFONT font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
            HFONT oldfont = (HFONT)SelectObject(mem, font);
            wchar_t wt[160];
            RECT r = plot;
            utf8_to_wide_buf("等待采集数据…（请连接 MCU 并开始采集）", wt, 160);
            SetBkMode(mem, TRANSPARENT);
            SetTextColor(mem, RGB(130, 140, 155));
            DrawTextW(mem, wt, -1, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            SelectObject(mem, oldfont);
        }
    }

    /* legend */
    {
        HFONT font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
        HFONT oldfont = (HFONT)SelectObject(mem, font);
        SetBkMode(mem, TRANSPARENT);
        for (i = 0; i < w->nseries; i++) {
            ScopeSeries* s = &w->series[i];
            char text[512];
            RECT lr;
            int y = 3 + i * 16;
            double lastv = 0;
            if (s->count > 0) {
                int idx = (s->head - 1 + SCOPE_HIST) % SCOPE_HIST;
                lastv = s->vals[idx];
            }
            _snprintf(text, sizeof(text), "%s = %g%s", s->name, lastv,
                      s->leaf_id < 0 ? " [missing]" : "");
            lr.left = 6; lr.top = y; lr.right = 16; lr.bottom = y + 12;
            {
                HBRUSH sw = CreateSolidBrush(s->color);
                FillRect(mem, &lr, sw);
                DeleteObject(sw);
            }
            lr.left = 20; lr.top = y; lr.right = plot.right; lr.bottom = y + 14;
            if (i == w->sel) SetTextColor(mem, RGB(255, 255, 255));
            else SetTextColor(mem, RGB(210, 214, 224));
            {
                wchar_t wt[512];
                utf8_to_wide_buf(text, wt, 512);
                DrawTextW(mem, wt, -1, &lr, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
            }
        }
        SelectObject(mem, oldfont);
    }

    /* border */
    {
        HPEN bpen = CreatePen(PS_SOLID, 1, RGB(70, 76, 90));
        HPEN oldp = (HPEN)SelectObject(mem, bpen);
        SelectObject(mem, GetStockObject(NULL_BRUSH));
        Rectangle(mem, 0, 0, rc.right - 1, rc.bottom - 1);
        SelectObject(mem, oldp);
        DeleteObject(bpen);
    }

    BitBlt(hdc, 0, 0, rc.right, rc.bottom, mem, 0, 0, SRCCOPY);
    SelectObject(mem, oldbmp);
    DeleteObject(bmp);
    DeleteDC(mem);
    DeleteObject(bg);
    DeleteObject(gridpen);
    EndPaint(w->hwnd, &ps);
}

static void layout_buttons(ScopeWin* w)
{
    RECT rc;
    GetClientRect(w->hwnd, &rc);
    if (w->hBtnAdd) MoveWindow(w->hBtnAdd, 2, 2, 76, 24, TRUE);
    if (w->hBtnDel) MoveWindow(w->hBtnDel, 80, 2, 76, 24, TRUE);
    if (w->hBtnWrite) MoveWindow(w->hBtnWrite, 158, 2, 76, 24, TRUE);
    if (w->hBtnClose) MoveWindow(w->hBtnClose, 236, 2, 56, 24, TRUE);
}

static void ensure_buttons(ScopeWin* w)
{
    if (w->hBtnAdd) return;
    w->hBtnAdd = CreateWindowW(L"BUTTON", L"添加变量",
                               WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                               2, 2, 76, 24, w->hwnd, (HMENU)1, GetModuleHandleW(NULL), NULL);
    w->hBtnDel = CreateWindowW(L"BUTTON", L"删除系列",
                               WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                               80, 2, 76, 24, w->hwnd, (HMENU)2, GetModuleHandleW(NULL), NULL);
    w->hBtnWrite = CreateWindowW(L"BUTTON", L"写值",
                                 WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                 158, 2, 76, 24, w->hwnd, (HMENU)3, GetModuleHandleW(NULL), NULL);
    w->hBtnClose = CreateWindowW(L"BUTTON", L"关闭",
                                 WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                 236, 2, 56, 24, w->hwnd, (HMENU)4, GetModuleHandleW(NULL), NULL);
    SendMessageW(w->hBtnAdd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
    SendMessageW(w->hBtnDel, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
    SendMessageW(w->hBtnWrite, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
    SendMessageW(w->hBtnClose, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
}

static int legend_hit(ScopeWin* w, int y)
{
    int i;
    for (i = 0; i < w->nseries; i++) {
        if (y >= 3 + i * 16 && y < 3 + i * 16 + 14) return i;
    }
    return -1;
}

/* ---------------- modal helper ---------------- */

static void modal_run(HWND dlg, HWND owner)
{
    MSG msg;
    ShowWindow(dlg, SW_SHOW);
    UpdateWindow(dlg);
    if (owner) EnableWindow(owner, FALSE);
    while (GetMessageW(&msg, NULL, 0, 0)) {
        if (!IsWindow(dlg)) break;
        if (IsDialogMessageW(dlg, &msg)) continue;
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    if (owner) {
        EnableWindow(owner, TRUE);
        SetForegroundWindow(owner);
    }
    if (IsWindow(dlg)) DestroyWindow(dlg);
}

/* ---------------- pick dialog ---------------- */

static void pick_refresh(HWND dlg)
{
    PickState* st = (PickState*)GetWindowLongPtrW(dlg, GWLP_USERDATA);
    wchar_t wtext[512];
    char text[512];
    HWND hList = GetDlgItem(dlg, IDC_P_LIST);
    HWND hEdit = GetDlgItem(dlg, IDC_P_EDIT);
    int i;
    GetWindowTextW(hEdit, wtext, 512);
    wide_to_utf8_buf(wtext, text, sizeof(text));
    st->count = g_fw->leaf_find(text, st->ids, 512);
    SendMessageW(hList, LB_RESETCONTENT, 0, 0);
    for (i = 0; i < st->count; i++) {
        const char* nm = g_fw->leaf_name(st->ids[i]);
        wchar_t wnm[320];
        int idx;
        utf8_to_wide_buf(nm ? nm : "?", wnm, 320);
        idx = (int)SendMessageW(hList, LB_ADDSTRING, 0, (LPARAM)wnm);
        SendMessageW(hList, LB_SETITEMDATA, idx, st->ids[i]);
    }
}

static LRESULT CALLBACK pick_proc(HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_CREATE: {
        PickState* st = (PickState*)calloc(1, sizeof(PickState));
        HINSTANCE hi = GetModuleHandleW(NULL);
        st->chosen = -1;
        SetWindowLongPtrW(dlg, GWLP_USERDATA, (LONG_PTR)st);
        CreateWindowW(L"STATIC", L"关键字:",
                      WS_CHILD | WS_VISIBLE, 10, 10, 60, 20, dlg, NULL, hi, NULL);
        CreateWindowW(L"EDIT", L"",
                      WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                      75, 8, 250, 22, dlg, (HMENU)IDC_P_EDIT, hi, NULL);
        CreateWindowW(L"LISTBOX", L"",
                      WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOINTEGRALHEIGHT,
                      10, 38, 315, 220, dlg, (HMENU)IDC_P_LIST, hi, NULL);
        CreateWindowW(L"BUTTON", L"确定",
                      WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                      170, 268, 72, 26, dlg, (HMENU)IDC_P_OK, hi, NULL);
        CreateWindowW(L"BUTTON", L"取消",
                      WS_CHILD | WS_VISIBLE,
                      250, 268, 72, 26, dlg, (HMENU)IDC_P_CANCEL, hi, NULL);
        return 0;
    }
    case WM_SETFOCUS:
        SetFocus(GetDlgItem(dlg, IDC_P_EDIT));
        return 0;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_P_EDIT:
            if (HIWORD(wParam) == EN_CHANGE) pick_refresh(dlg);
            return 0;
        case IDC_P_OK: {
            PickState* st = (PickState*)GetWindowLongPtrW(dlg, GWLP_USERDATA);
            int cur = (int)SendMessageW(GetDlgItem(dlg, IDC_P_LIST), LB_GETCURSEL, 0, 0);
            if (cur >= 0)
                st->chosen = (int)SendMessageW(GetDlgItem(dlg, IDC_P_LIST),
                                               LB_GETITEMDATA, cur, 0);
            DestroyWindow(dlg);
            return 0;
        }
        case IDC_P_CANCEL:
            DestroyWindow(dlg);
            return 0;
        }
        return 0;
    case WM_DESTROY: {
        PickState* st = (PickState*)GetWindowLongPtrW(dlg, GWLP_USERDATA);
        free(st);
        return 0;
    }
    }
    return DefWindowProcW(dlg, msg, wParam, lParam);
}

/* ---------------- value dialog ---------------- */

static LRESULT CALLBACK val_proc(HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_CREATE: {
        ValState* st = (ValState*)calloc(1, sizeof(ValState));
        HINSTANCE hi = GetModuleHandleW(NULL);
        wchar_t info[512];
        SetWindowLongPtrW(dlg, GWLP_USERDATA, (LONG_PTR)st);
        CreateWindowW(L"STATIC", L"",
                      WS_CHILD | WS_VISIBLE, 10, 12, 300, 20, dlg, (HMENU)IDC_V_INFO, hi, NULL);
        CreateWindowW(L"EDIT", L"",
                      WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                      10, 38, 300, 24, dlg, (HMENU)IDC_V_EDIT, hi, NULL);
        CreateWindowW(L"BUTTON", L"确定",
                      WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                      170, 72, 68, 26, dlg, (HMENU)IDC_V_OK, hi, NULL);
        CreateWindowW(L"BUTTON", L"取消",
                      WS_CHILD | WS_VISIBLE,
                      246, 72, 68, 26, dlg, (HMENU)IDC_V_CANCEL, hi, NULL);
        _snwprintf(info, 512, L"%hs = %g", st->name, st->current);
        SetWindowTextW(GetDlgItem(dlg, IDC_V_INFO), info);
        return 0;
    }
    case WM_SETFOCUS:
        SetFocus(GetDlgItem(dlg, IDC_V_EDIT));
        return 0;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_V_OK: {
            ValState* st = (ValState*)GetWindowLongPtrW(dlg, GWLP_USERDATA);
            wchar_t wtext[256];
            char text[256];
            char err[256] = "";
            GetWindowTextW(GetDlgItem(dlg, IDC_V_EDIT), wtext, 256);
            wide_to_utf8_buf(wtext, text, sizeof(text));
            if (g_fw->write_leaf(st->leaf_id, atof(text), err, sizeof(err)) != OS_ERR_OK) {
                wchar_t werr[512];
                utf8_to_wide_buf(err[0] ? err : "写入失败", werr, 512);
                MessageBoxW(dlg, werr, L"写值", MB_OK | MB_ICONERROR);
                return 0;
            }
            DestroyWindow(dlg);
            return 0;
        }
        case IDC_V_CANCEL:
            DestroyWindow(dlg);
            return 0;
        }
        return 0;
    case WM_DESTROY: {
        ValState* st = (ValState*)GetWindowLongPtrW(dlg, GWLP_USERDATA);
        free(st);
        return 0;
    }
    }
    return DefWindowProcW(dlg, msg, wParam, lParam);
}

/* ---------------- window proc ---------------- */

static LRESULT CALLBACK scope_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    ScopeWin* w = (ScopeWin*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    switch (msg) {
    case WM_CREATE: {
        w = (ScopeWin*)((CREATESTRUCTW*)lParam)->lpCreateParams;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)w);
        w->hwnd = hwnd;
        w->sel = -1;
        ensure_buttons(w);
        layout_buttons(w);
        return 0;
    }
    case WM_SIZE:
        if (w) layout_buttons(w);
        return 0;
    case WM_PAINT: {
        HDC hdc = GetDC(hwnd);
        RECT rc;
        GetClientRect(hwnd, &rc);
        ReleaseDC(hwnd, hdc);
        if (w) scope_paint(w);
        return 0;
    }
    case WM_ERASEBKGND:
        return 1;
    case WM_LBUTTONDOWN:
        if (w) {
            int hit = legend_hit(w, (int)(short)HIWORD(lParam));
            w->sel = hit;
            InvalidateRect(hwnd, NULL, TRUE);
        }
        return 0;
    case WM_LBUTTONDBLCLK:
        if (w) {
            int hit = legend_hit(w, (int)(short)HIWORD(lParam));
            if (hit >= 0) w->sel = hit;
            if (w->sel >= 0 && w->sel < w->nseries && w->series[w->sel].leaf_id >= 0) {
                ScopeSeries* s = &w->series[w->sel];
                ValState* st = (ValState*)calloc(1, sizeof(ValState));
                double cur = 0;
                if (s->count > 0) {
                    int idx = (s->head - 1 + SCOPE_HIST) % SCOPE_HIST;
                    cur = s->vals[idx];
                }
                st->leaf_id = s->leaf_id;
                _snprintf(st->name, sizeof(st->name), "%s", s->name);
                st->current = cur;
                {
                    HWND dlg = CreateWindowExW(WS_EX_DLGMODALFRAME, g_wcls_val, L"写值",
                                               WS_CAPTION | WS_SYSMENU | WS_OVERLAPPED,
                                               CW_USEDEFAULT, CW_USEDEFAULT, 340, 140,
                                               hwnd, NULL, GetModuleHandleW(NULL), st);
                    if (dlg) {
                        SendMessageW(dlg, WM_SETFONT,
                                     (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
                        modal_run(dlg, hwnd);
                    } else {
                        free(st);
                    }
                }
            }
        }
        return 0;
    case WM_COMMAND:
        if (!w) break;
        switch (LOWORD(wParam)) {
        case 1: /* add variable */
        {
            PickState* st = (PickState*)calloc(1, sizeof(PickState));
            HWND dlg;
            st->chosen = -1;
            dlg = CreateWindowExW(WS_EX_DLGMODALFRAME, g_wcls_pick, L"添加变量",
                                  WS_CAPTION | WS_SYSMENU | WS_OVERLAPPED,
                                  CW_USEDEFAULT, CW_USEDEFAULT, 360, 330,
                                  hwnd, NULL, GetModuleHandleW(NULL), st);
            if (dlg) {
                SendMessageW(dlg, WM_SETFONT,
                             (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
                modal_run(dlg, hwnd);
                if (st->chosen >= 0 && w->nseries < SCOPE_MAX_SERIES) {
                    ScopeSeries* s = &w->series[w->nseries];
                    const char* nm = g_fw->leaf_name(st->chosen);
                    memset(s, 0, sizeof(*s));
                    s->leaf_id = st->chosen;
                    _snprintf(s->name, sizeof(s->name), "%s", nm ? nm : "?");
                    s->color = g_palette[w->nseries % 8];
                    w->nseries++;
                    w->sel = w->nseries - 1;
                    InvalidateRect(hwnd, NULL, TRUE);
                }
            } else {
                free(st);
            }
            return 0;
        }
        case 2: /* remove selected series */
            if (w->sel >= 0 && w->sel < w->nseries) {
                int i;
                for (i = w->sel; i < w->nseries - 1; i++)
                    w->series[i] = w->series[i + 1];
                w->nseries--;
                w->sel = -1;
                InvalidateRect(hwnd, NULL, TRUE);
            }
            return 0;
        case 3: /* write value for selected series */
            if (w->sel >= 0 && w->sel < w->nseries &&
                w->series[w->sel].leaf_id >= 0) {
                ScopeSeries* s = &w->series[w->sel];
                ValState* st = (ValState*)calloc(1, sizeof(ValState));
                double cur = 0;
                if (s->count > 0) {
                    int idx = (s->head - 1 + SCOPE_HIST) % SCOPE_HIST;
                    cur = s->vals[idx];
                }
                st->leaf_id = s->leaf_id;
                _snprintf(st->name, sizeof(st->name), "%s", s->name);
                st->current = cur;
                {
                    HWND dlg = CreateWindowExW(WS_EX_DLGMODALFRAME, g_wcls_val, L"写值",
                                               WS_CAPTION | WS_SYSMENU | WS_OVERLAPPED,
                                               CW_USEDEFAULT, CW_USEDEFAULT, 340, 140,
                                               hwnd, NULL, GetModuleHandleW(NULL), st);
                    if (dlg) {
                        SendMessageW(dlg, WM_SETFONT,
                                     (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
                        modal_run(dlg, hwnd);
                    } else {
                        free(st);
                    }
                }
            }
            return 0;
        case 4: /* close window */
            if (w && w->hwnd) {
                if (g_fw && g_fw->post_msg)
                    g_fw->post_msg(OS_WM_WIN_CLOSED, (WPARAM)w->hwnd, 0);
                else
                    PostMessageW(GetParent(w->hwnd), OS_WM_WIN_CLOSED, (WPARAM)w->hwnd, 0);
            }
            return 0;
        }
        break;
    case WM_DESTROY:
        if (w) {
            if (w->hBtnAdd) DestroyWindow(w->hBtnAdd);
            if (w->hBtnDel) DestroyWindow(w->hBtnDel);
            if (w->hBtnWrite) DestroyWindow(w->hBtnWrite);
            if (w->hBtnClose) DestroyWindow(w->hBtnClose);
            free(w);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
        }
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

/* ---------------- module entry points ---------------- */

static void register_class(const char* cls, WNDPROC proc)
{
    WNDCLASSW wc;
    wchar_t* wcls;
    if (cls == g_win_class) wcls = g_wcls_win;
    else if (cls == g_pick_class) wcls = g_wcls_pick;
    else wcls = g_wcls_val;
    utf8_to_wide_buf(cls, wcls, 64);
    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = proc;
    wc.hInstance = GetModuleHandleW(NULL);
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszClassName = wcls;
    RegisterClassW(&wc);
}

static int mod_init(const OS_Framework* fw, void** out_ctx)
{
    ScopeMod* m = (ScopeMod*)calloc(1, sizeof(ScopeMod));
    if (!m) return OS_ERR_FAIL;
    g_fw = fw;
    register_class(g_win_class, scope_proc);
    register_class(g_pick_class, pick_proc);
    register_class(g_val_class, val_proc);
    *out_ctx = m;
    if (fw) fw->log(OS_LOG_INFO, "scope 模块: 已初始化（示波器窗口）");
    return OS_ERR_OK;
}

static void mod_deinit(void* ctx)
{
    ScopeMod* m = (ScopeMod*)ctx;
    int i;
    if (!m) return;
    for (i = 0; i < m->nwins; i++) {
        if (m->wins[i] && m->wins[i]->hwnd) DestroyWindow(m->wins[i]->hwnd);
    }
    free(m);
}

static HWND mod_create_window(void* ctx, const char* type, HWND parent,
                              int x, int y, int w, int h, const char* title)
{
    ScopeMod* m = (ScopeMod*)ctx;
    ScopeWin* sw;
    wchar_t wcls[64], wtitle[256];
    HWND hw;
    if (!m || !type || strcmp(type, "scope.bar") != 0) return NULL;
    if (m->nwins >= SCOPE_MAX_WINS) return NULL;
    sw = (ScopeWin*)calloc(1, sizeof(ScopeWin));
    if (!sw) return NULL;
    utf8_to_wide_buf(g_win_class, wcls, 64);
    utf8_to_wide_buf(title ? title : "示波器窗口", wtitle, 256);
    hw = CreateWindowExW(WS_EX_CLIENTEDGE, wcls, wtitle,
                         WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
                         x, y, w, h, parent, NULL, GetModuleHandleW(NULL), sw);
    if (!hw) {
        free(sw);
        return NULL;
    }
    sw->hwnd = hw;
    m->wins[m->nwins++] = sw;
    return hw;
}

static void mod_destroy_window(void* ctx, HWND hwnd)
{
    ScopeMod* m = (ScopeMod*)ctx;
    int i;
    if (!m || !hwnd) return;
    for (i = 0; i < m->nwins; i++) {
        if (m->wins[i] && m->wins[i]->hwnd == hwnd) {
            if (IsWindow(hwnd)) DestroyWindow(hwnd);
            memmove(&m->wins[i], &m->wins[i + 1],
                    sizeof(ScopeWin*) * (m->nwins - i - 1));
            m->nwins--;
            return;
        }
    }
}

static void mod_on_samples(void* ctx, const OS_Sample* samples, int count)
{
    ScopeMod* m = (ScopeMod*)ctx;
    int i, j, k;
    if (!m || !samples || count <= 0) return;
    for (i = 0; i < m->nwins; i++) {
        ScopeWin* w = m->wins[i];
        int changed = 0;
        if (!w || !w->hwnd) continue;
        for (j = 0; j < count; j++) {
            const OS_Sample* s = &samples[j];
            for (k = 0; k < w->nseries; k++) {
                if (w->series[k].leaf_id == s->var_id) {
                    series_append(&w->series[k], s);
                    changed = 1;
                    break;
                }
            }
        }
        if (changed) InvalidateRect(w->hwnd, NULL, TRUE);
    }
}

static void mod_on_reload(void* ctx)
{
    ScopeMod* m = (ScopeMod*)ctx;
    int i;
    if (!m) return;
    for (i = 0; i < m->nwins; i++) {
        if (m->wins[i]) resolve_ids(m->wins[i]);
    }
}

static int mod_win_add_var(void* ctx, HWND hwnd, int leaf_id)
{
    ScopeMod* m = (ScopeMod*)ctx;
    int i;
    if (!m || !hwnd || leaf_id < 0) return OS_ERR_INVALID_ARG;
    for (i = 0; i < m->nwins; i++) {
        ScopeWin* w = m->wins[i];
        if (w && w->hwnd == hwnd) {
            const char* nm;
            ScopeSeries* s;
            if (w->nseries >= SCOPE_MAX_SERIES) return OS_ERR_BUSY;
            nm = g_fw->leaf_name(leaf_id);
            s = &w->series[w->nseries];
            memset(s, 0, sizeof(*s));
            s->leaf_id = leaf_id;
            _snprintf(s->name, sizeof(s->name), "%s", nm ? nm : "?");
            s->color = g_palette[w->nseries % 8];
            w->nseries++;
            w->sel = w->nseries - 1;
            if (g_fw && g_fw->log)
                g_fw->log(OS_LOG_DEBUG, "示波器窗口添加变量: id=%d name=%s",
                          leaf_id, s->name);
            InvalidateRect(hwnd, NULL, TRUE);
            return OS_ERR_OK;
        }
    }
    return OS_ERR_FAIL;
}

static const OS_WindowType g_window_types[] = {
    { "scope.bar", "示波器窗口" },
    { NULL, NULL }
};

static const OS_Module g_module = {
    OS_API_VERSION,
    OS_CAP_WINDOW,
    "scope",
    "1.1.0",
    "示波器窗口模块：实时曲线、变量模糊搜索、数值写回",
    g_window_types,
    mod_init,
    mod_deinit,
    NULL,
    mod_create_window,
    mod_destroy_window,
    mod_on_samples,
    mod_on_reload,
    mod_win_add_var
};

const OS_Module* os_module_get(void)
{
    return &g_module;
}
