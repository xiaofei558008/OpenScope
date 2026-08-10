//! 波形窗口（对应 C 版 chartwin.c 最小集）。
//!
//! 子窗口类 "OpenScopeChart"，GDI 双缓冲绘制实时曲线。数据源为 acq::AcqState。

#![allow(non_snake_case)]

use std::ffi::c_void;
use windows::core::*;
use windows::Win32::Foundation::*;
use windows::Win32::Graphics::Gdi::*;
use windows::Win32::UI::WindowsAndMessaging::*;

use crate::acq::RING_CAP;
use crate::app::{get_app_mut, CHART_CLASS};
use crate::winmgr;

/// 系列调色板（与 C 版一致的常见颜色）。
const PALETTE: [COLORREF; 8] = [
    COLORREF(0x000000FF), // 红
    COLORREF(0x0000FF00), // 绿
    COLORREF(0x00FF0000), // 蓝
    COLORREF(0x00FF00FF), // 紫
    COLORREF(0x0000FFFF), // 黄
    COLORREF(0x00FFFFFF), // 白
    COLORREF(0x000080FF), // 橙
    COLORREF(0x0000FFFF), // 青
];

/// 每窗口视图状态（存于 GWLP_USERDATA）。
#[derive(Clone, Copy)]
pub struct ChartView {
    /// 可视点个数（<= RING_CAP）。
    pub view_pts: usize,
}

impl Default for ChartView {
    fn default() -> Self {
        ChartView { view_pts: RING_CAP }
    }
}

/// 注册图表窗口类。
pub unsafe fn register_class(hinst: HINSTANCE) -> Result<u16> {
    let wc = WNDCLASSW {
        style: CS_HREDRAW | CS_VREDRAW,
        lpfnWndProc: Some(chart_wndproc),
        hInstance: hinst,
        lpszClassName: CHART_CLASS,
        hbrBackground: CreateSolidBrush(COLORREF(0x00202020)),
        hCursor: LoadCursorW(None, IDC_ARROW).unwrap_or_default(),
        ..Default::default()
    };
    let atom = RegisterClassW(&wc);
    if atom == 0 {
        return Err(Error::from_win32());
    }
    Ok(atom)
}

/// 创建图表子窗口（父窗口=tab 控件）。
pub unsafe fn create(parent: HWND, hinst: HINSTANCE, id: i32) -> Result<HWND> {
    let hwnd = CreateWindowExW(
        WINDOW_EX_STYLE::default(),
        CHART_CLASS,
        None,
        WINDOW_STYLE(WS_CHILD.0 | WS_VISIBLE.0 | WS_BORDER.0 | WS_CLIPSIBLINGS.0),
        0,
        0,
        0,
        0,
        Some(parent),
        Some(HMENU(id as *mut c_void)),
        Some(hinst),
        None,
    )?;
    let view = Box::new(ChartView::default());
    let _ = SetWindowLongPtrW(
        hwnd,
        GWLP_USERDATA,
        Box::into_raw(view) as isize,
    );
    Ok(hwnd)
}

unsafe fn chart_view(hwnd: HWND) -> &'static mut ChartView {
    let p = GetWindowLongPtrW(hwnd, GWLP_USERDATA) as *mut ChartView;
    if p.is_null() {
        // 首次访问（理论上 create 已设置）
        static mut FALLBACK: ChartView = ChartView { view_pts: RING_CAP };
        &mut FALLBACK
    } else {
        &mut *p
    }
}

unsafe extern "system" fn chart_wndproc(
    hwnd: HWND,
    msg: u32,
    wparam: WPARAM,
    lparam: LPARAM,
) -> LRESULT {
    match msg {
        WM_ERASEBKGND => LRESULT(1),
        WM_PAINT => {
            let view = chart_view(hwnd);
            paint(hwnd, view.view_pts);
            let _ = ValidateRect(Some(hwnd), None);
            LRESULT(0)
        }
        WM_MOUSEWHEEL => {
            // 滚轮缩放 X：上=放大（更少点数），下=缩小（更多点数）
            let delta = ((wparam.0 >> 16) & 0xffff) as i16;
            let view = chart_view(hwnd);
            if delta > 0 {
                view.view_pts = (view.view_pts / 2).clamp(64, RING_CAP);
            } else {
                view.view_pts = (view.view_pts * 2).clamp(64, RING_CAP);
            }
            let _ = InvalidateRect(Some(hwnd), None, false);
            LRESULT(0)
        }
        WM_CONTEXTMENU => {
            // 右键：列出本窗口系列，选择移除
            let id = GetDlgCtrlID(hwnd);
            let Some(win_idx) = winmgr::win_index_from_id(id) else {
                return LRESULT(0);
            };
            let mut names: Vec<String> = Vec::new();
            if let Some(app) = get_app_mut() {
                if let Some(w) = app.wins.get(win_idx) {
                    let acq = app.acq.lock().unwrap_or_else(|e| e.into_inner());
                    for &si in &w.series {
                        if let Some(l) = acq.leaves.get(si) {
                            names.push(l.name.clone());
                        }
                    }
                }
            }
            if names.is_empty() {
                return LRESULT(0);
            }
            let menu = CreatePopupMenu();
            let Ok(menu) = menu else { return LRESULT(0) };
            for (i, nm) in names.iter().enumerate() {
                let label = format!("移除系列: {}", nm);
                let wl: Vec<u16> = label.encode_utf16().chain(std::iter::once(0)).collect();
                let _ = AppendMenuW(menu, MF_STRING, (3000 + i) as usize, PCWSTR(wl.as_ptr()));
            }
            let mut pt = POINT::default();
            let _ = GetCursorPos(&mut pt);
            let sel = TrackPopupMenu(
                menu,
                TPM_RETURNCMD | TPM_RIGHTBUTTON,
                pt.x,
                pt.y,
                None,
                hwnd,
                None,
            );
            let _ = DestroyMenu(menu);
            let c = sel.0 as i32;
            let idx = c - 3000;
            if idx >= 0 {
                winmgr::remove_series_from_win(win_idx, idx as usize);
            }
            LRESULT(0)
        }
        WM_DESTROY => {
            let p = GetWindowLongPtrW(hwnd, GWLP_USERDATA) as *mut ChartView;
            if !p.is_null() {
                drop(Box::from_raw(p));
            }
            LRESULT(0)
        }
        _ => DefWindowProcW(hwnd, msg, wparam, lparam),
    }
}

/// 双缓冲绘制。只画本窗口所属系列（app.wins[win_idx].series）。
unsafe fn paint(hwnd: HWND, view_pts: usize) {
    let mut rc = RECT::default();
    let _ = GetClientRect(hwnd, &mut rc);
    let w = (rc.right - rc.left).max(1);
    let h = (rc.bottom - rc.top).max(1);

    let hdc = GetDC(Some(hwnd));
    let mem = CreateCompatibleDC(Some(hdc));
    let bmp = CreateCompatibleBitmap(hdc, w, h);
    let old_bmp = SelectObject(mem, HGDIOBJ(bmp.0));

    // 背景（深色，与 C 版暗色波形窗口一致）
    let bg = CreateSolidBrush(COLORREF(0x00101010));
    let _ = FillRect(mem, &rc, bg);
    let _ = DeleteObject(HGDIOBJ(bg.0));

    let mut nseries = 0usize;
    // 本窗口系列（从控制 ID 反查窗口下标）
    let id = GetDlgCtrlID(hwnd);
    let mut series: Vec<usize> = Vec::new();
    let mut name: Vec<String> = Vec::new();
    if let Some(win_idx) = winmgr::win_index_from_id(id) {
        if let Some(app) = get_app_mut() {
            if let Some(w) = app.wins.get(win_idx) {
                series = w.series.clone();
            }
            let acq = app.acq.lock().unwrap_or_else(|e| e.into_inner());
            for &si in &series {
                if let Some(l) = acq.leaves.get(si) {
                    name.push(l.name.clone());
                }
            }
            nseries = series.len().min(PALETTE.len());

            // 本窗口全系列共享一个 Y 值域（与 C 版 chart_compute_view 一致）
            let (mut vmin, mut vmax) = (f32::INFINITY, f32::NEG_INFINITY);
            let mut have_data = false;
            for &si in &series[..nseries] {
                let Some(leaf) = acq.leaves.get(si) else { continue };
                let start = if leaf.count >= view_pts {
                    (leaf.head + RING_CAP - view_pts) % RING_CAP
                } else {
                    0
                };
                let n = leaf.count.min(view_pts);
                for k in 0..n {
                    let v = leaf.samples[(start + k) % RING_CAP];
                    if v.is_finite() {
                        vmin = vmin.min(v);
                        vmax = vmax.max(v);
                        have_data = true;
                    }
                }
            }
            if !have_data {
                vmin = -1.0;
                vmax = 1.0;
            }
            if vmax - vmin < 1e-12 {
                vmax += 1.0;
                vmin -= 1.0;
            }
            let pad = (vmax - vmin) * 0.08;
            vmin -= pad;
            vmax += pad;
            if vmax - vmin < 1e-12 {
                vmax = vmin + 1.0;
            }

            for (pi, &si) in series[..nseries].iter().enumerate() {
                let Some(leaf) = acq.leaves.get(si) else { continue };
                let n = leaf.count.min(view_pts);
                if n < 2 {
                    continue;
                }
                let start = if leaf.count >= view_pts {
                    (leaf.head + RING_CAP - view_pts) % RING_CAP
                } else {
                    0
                };

                let pen = CreatePen(PS_SOLID, 1, PALETTE[pi]);
                let old_pen = SelectObject(mem, HGDIOBJ(pen.0));
                let brush = CreateSolidBrush(PALETTE[pi]);
                let old_brush = SelectObject(mem, HGDIOBJ(brush.0));
                // 点数少/放大时画采样点圆点（>3px 间距即可见）
                let dots = n <= 300;
                let mut first = true;
                for k in 0..n {
                    let v = leaf.samples[(start + k) % RING_CAP];
                    let x = (k as f32 / (n - 1) as f32 * (w - 1) as f32) as i32;
                    let y = (h as f32
                        - (v - vmin) / (vmax - vmin) * (h - 1) as f32) as i32;
                    let y = y.clamp(0, h - 1);
                    if first {
                        let _ = MoveToEx(mem, x, y, None);
                        first = false;
                    } else {
                        let _ = LineTo(mem, x, y);
                    }
                    if dots {
                        let _ = Ellipse(mem, x - 2, y - 2, x + 2, y + 2);
                    }
                }
                let _ = SelectObject(mem, old_brush);
                let _ = DeleteObject(HGDIOBJ(brush.0));
                let _ = SelectObject(mem, old_pen);
                let _ = DeleteObject(HGDIOBJ(pen.0));
            }
            drop(acq);
        }
    }

    // 空窗口提示：在变量树右键添加变量
    if nseries == 0 {
        let mut label: Vec<u16> = "（右键变量树 → 添加变量到波形窗口）"
            .encode_utf16()
            .chain(std::iter::once(0))
            .collect();
        let _ = SetTextColor(mem, COLORREF(0x00686868));
        let _ = TextOutW(mem, 12, 12, &label);
    } else if !name.is_empty() {
        // 左上角画系列名图例
        for (pi, nm) in name.iter().enumerate().take(nseries) {
            let brush = CreateSolidBrush(PALETTE[pi]);
            let old_brush = SelectObject(mem, HGDIOBJ(brush.0));
            let _ = Rectangle(mem, 10, 8 + pi as i32 * 16, 22, 20 + pi as i32 * 16);
            let _ = SelectObject(mem, old_brush);
            let _ = DeleteObject(HGDIOBJ(brush.0));
            let mut wl: Vec<u16> = nm.encode_utf16().chain(std::iter::once(0)).collect();
            let _ = SetTextColor(mem, COLORREF(0x00C8C8C8));
            let _ = TextOutW(mem, 26, 6 + pi as i32 * 16, &wl);
        }
    }

    let _ = BitBlt(hdc, 0, 0, w, h, Some(mem), 0, 0, SRCCOPY);
    let _ = SelectObject(mem, old_bmp);
    let _ = DeleteObject(HGDIOBJ(bmp.0));
    let _ = DeleteDC(mem);
    let _ = ReleaseDC(None, hdc);
}
