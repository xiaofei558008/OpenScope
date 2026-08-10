//! 窗口/tab 管理器（对应 C 版多窗口与 tab 体系的最小集）。
//!
//! 模型：右侧 TabControl 的每一个 tab = 一个窗口（波形/数值）。App 维护
//! `Vec<OsWin>`，每项含 kind / hwnd / 标题 / 系列（acq.leaves 下标）。切换 tab
//! 时（TCN_SELCHANGE）仅显示当前窗口的子 HWND，隐藏其余。

#![allow(non_snake_case)]

use std::ffi::c_void;
use windows::core::*;
use windows::Win32::Foundation::*;
use windows::Win32::Graphics::Gdi::InvalidateRect;
use windows::Win32::UI::Controls::*;
use windows::Win32::UI::WindowsAndMessaging::*;

use crate::app::{get_app_mut, CHART_CLASS};
use crate::acq::LeafSpec;

/// 窗口类型。
#[derive(Clone, Copy, PartialEq, Eq)]
pub enum WinKind {
    Chart,
    Number,
}

/// 一个窗口实例（一个 tab）。
pub struct OsWin {
    pub kind: WinKind,
    pub hwnd: HWND,
    pub title: String,
    /// 本窗口观测的叶变量（acq.leaves 下标）。
    pub series: Vec<usize>,
}

/// 根据控制 ID 判断并定位窗口：波形 ID 从 2300 起，数值 ID 从 2350 起。
pub fn win_index_from_id(id: i32) -> Option<usize> {
    if (2300..2400).contains(&id) {
        let base = if id < 2350 { 2300 } else { 2350 };
        return Some((id - base) as usize);
    }
    None
}

pub fn win_kind_from_id(id: i32) -> Option<WinKind> {
    if (2300..2350).contains(&id) {
        Some(WinKind::Chart)
    } else if (2350..2400).contains(&id) {
        Some(WinKind::Number)
    } else {
        None
    }
}

/// 在右侧 TabControl 插入一个 tab 项。
pub unsafe fn insert_tab_item(htab: HWND, index: usize, title: &str) {
    let mut wide: Vec<u16> = title.encode_utf16().collect();
    wide.push(0);
    let mut item = TCITEMW::default();
    item.mask = TCIF_TEXT;
    item.pszText = PWSTR(wide.as_mut_ptr());
    let _ = SendMessageW(
        htab,
        TCM_INSERTITEMW,
        Some(WPARAM(index)),
        Some(LPARAM(&raw mut item as isize)),
    );
}

/// 重命名指定 tab 项。
pub unsafe fn rename_tab_item(htab: HWND, index: usize, title: &str) {
    let mut wide: Vec<u16> = title.encode_utf16().collect();
    wide.push(0);
    let mut item = TCITEMW::default();
    item.mask = TCIF_TEXT;
    item.pszText = PWSTR(wide.as_mut_ptr());
    let _ = SendMessageW(
        htab,
        TCM_SETITEMW,
        Some(WPARAM(index)),
        Some(LPARAM(&raw mut item as isize)),
    );
}

/// 当前选中的 tab 下标。
pub unsafe fn current_tab(htab: HWND) -> usize {
    let r = SendMessageW(htab, TCM_GETCURSEL, None, None);
    r.0 as usize
}

pub unsafe fn tab_count(htab: HWND) -> usize {
    let r = SendMessageW(htab, TCM_GETITEMCOUNT, None, None);
    r.0 as usize
}

/// 创建波形窗口子窗口（父=tab 控件）。
pub unsafe fn create_chart_hwnd(
    parent: HWND,
    hinst: HINSTANCE,
    win_idx: usize,
) -> Result<HWND> {
    crate::chart::create(parent, hinst, (2300 + win_idx) as i32)
}

/// 创建数值窗口子窗口（父=tab 控件）。
pub unsafe fn create_number_hwnd(
    parent: HWND,
    hinst: HINSTANCE,
    win_idx: usize,
) -> Result<HWND> {
    crate::numwin::create(parent, hinst, (2350 + win_idx) as i32)
}

/// 切换激活窗口：显示 wins[idx].hwnd，隐藏其余。
pub unsafe fn switch_win(win_idx: usize) {
    if let Some(app) = get_app_mut() {
        let wins = &app.wins;
        if win_idx >= wins.len() {
            return;
        }
        for (i, w) in wins.iter().enumerate() {
            let vis = i == win_idx;
            let _ = ShowWindow(
                w.hwnd,
                if vis { SW_SHOWNA } else { SW_HIDE },
            );
        }
    }
}

/// 把一批 ELF 变量（acq.leaves 下标）登记为采集叶并加入指定窗口系列。
/// 自动去除重复。leaf 尚未登记时按 ELF 符号补登。
pub unsafe fn add_vars_to_win(win_idx: usize, leaf_idxs: &[usize]) {
    if let Some(app) = get_app_mut() {
        if win_idx >= app.wins.len() {
            return;
        }
        // 先确保每个变量都在 acq.leaves 中（通过现有登记逻辑）
        for &idx in leaf_idxs {
            app.register_leaf(idx);
        }
        // 再加入窗口系列（去重）
        {
            let series = &mut app.wins[win_idx].series;
            for &idx in leaf_idxs {
                if !series.contains(&idx) {
                    series.push(idx);
                }
            }
        }
        let _ = InvalidateRect(Some(app.wins[win_idx].hwnd), None, false);
    }
}

/// 从指定窗口移除系列。
pub unsafe fn remove_series_from_win(win_idx: usize, series_idx: usize) {
    if let Some(app) = get_app_mut() {
        if win_idx >= app.wins.len() {
            return;
        }
        let w = &mut app.wins[win_idx];
        if series_idx < w.series.len() {
            w.series.remove(series_idx);
        }
        let _ = InvalidateRect(Some(w.hwnd), None, false);
    }
}

/// 把 acq.leaves 中现有叶的规格取出来（供数值窗口显示）。
pub unsafe fn leaf_specs() -> Vec<(usize, LeafSpec)> {
    if let Some(app) = get_app_mut() {
        let acq = app.acq.lock().unwrap_or_else(|e| e.into_inner());
        acq.leaves
            .iter()
            .enumerate()
            .map(|(i, l)| {
                (
                    i,
                    LeafSpec {
                        name: l.name.clone(),
                        address: l.address,
                        size: l.size,
                        is_signed: l.is_signed,
                        is_float: l.is_float,
                    },
                )
            })
            .collect()
    } else {
        Vec::new()
    }
}
