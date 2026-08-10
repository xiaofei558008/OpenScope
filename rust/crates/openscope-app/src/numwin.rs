//! 数值窗口（对应 C 版 numwin.c 最小集）。
//!
//! 子窗口类 "OpenScopeNum"：ListView 逐行显示本窗口系列变量的实时值；
//! 勾选列控制该行是否实时刷新；选中某行后在顶部编辑框输入数值回车/点"写入"
//! 通过 jlink.write_mem 直接写入 MCU 内存（bug6 数值窗口）。

#![allow(non_snake_case)]

use std::ffi::c_void;
use windows::core::*;
use windows::Win32::Foundation::*;
use windows::Win32::Graphics::Gdi::*;
use windows::Win32::UI::Controls::*;
use windows::Win32::UI::WindowsAndMessaging::*;

use crate::app::get_app_mut;
use crate::winmgr;

pub const NUM_CLASS: PCWSTR = w!("OpenScopeNum");

// 子控件 ID
const IDC_NUM_LIST: i32 = 2351;
const IDC_NUM_EDIT: i32 = 2352;
const IDC_NUM_WRITE: i32 = 2353;
const IDC_NUM_TIP: i32 = 2354;

/// 每窗口数值状态（存于 GWLP_USERDATA）。
struct NumWinState {
    /// 每行（= 本窗口 series 顺序）是否实时刷新。
    live: Vec<bool>,
    /// 当前选中的 acq.leaves 下标（写入目标），无则 usize::MAX。
    target: usize,
}

/// 注册数值窗口类。
pub unsafe fn register_class(hinst: HINSTANCE) -> Result<u16> {
    let wc = WNDCLASSW {
        style: CS_HREDRAW | CS_VREDRAW,
        lpfnWndProc: Some(num_wndproc),
        hInstance: hinst,
        lpszClassName: NUM_CLASS,
        hbrBackground: CreateSolidBrush(COLORREF(0x00F0F0F0)),
        hCursor: LoadCursorW(None, IDC_ARROW).unwrap_or_default(),
        ..Default::default()
    };
    let atom = RegisterClassW(&wc);
    if atom == 0 {
        return Err(Error::from_win32());
    }
    Ok(atom)
}

/// 创建数值子窗口（父=tab 控件），id=2350+win_idx。
pub unsafe fn create(parent: HWND, hinst: HINSTANCE, id: i32) -> Result<HWND> {
    let hwnd = CreateWindowExW(
        WINDOW_EX_STYLE::default(),
        NUM_CLASS,
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
    let state = Box::new(NumWinState {
        live: Vec::new(),
        target: usize::MAX,
    });
    let _ = SetWindowLongPtrW(hwnd, GWLP_USERDATA, Box::into_raw(state) as isize);

    // 顶部：编辑框 + 写入按钮 + 提示
    let edit = CreateWindowExW(
        WINDOW_EX_STYLE::default(),
        w!("EDIT"),
        None,
        WINDOW_STYLE(WS_CHILD.0 | WS_VISIBLE.0 | WS_BORDER.0 | WS_TABSTOP.0 | ES_AUTOHSCROLL as u32),
        8,
        8,
        140,
        24,
        Some(hwnd),
        Some(HMENU(IDC_NUM_EDIT as *mut c_void)),
        Some(hinst),
        None,
    )?;
    let btn = CreateWindowExW(
        WINDOW_EX_STYLE::default(),
        w!("BUTTON"),
        w!("写入"),
        WINDOW_STYLE(WS_CHILD.0 | WS_VISIBLE.0 | WS_TABSTOP.0 | BS_PUSHBUTTON as u32),
        152,
        8,
        64,
        24,
        Some(hwnd),
        Some(HMENU(IDC_NUM_WRITE as *mut c_void)),
        Some(hinst),
        None,
    )?;
    let tip = CreateWindowExW(
        WINDOW_EX_STYLE::default(),
        w!("STATIC"),
        w!("在列表选中变量，输入数值后点\"写入\"或回车，直接写 MCU 内存"),
        WINDOW_STYLE(WS_CHILD.0 | WS_VISIBLE.0 | 0x0Cu32), // SS_LEFTNOWORDWRAP
        224,
        10,
        400,
        20,
        Some(hwnd),
        Some(HMENU(IDC_NUM_TIP as *mut c_void)),
        Some(hinst),
        None,
    )?;
    let _ = edit;
    let _ = btn;
    let _ = tip;

    // ListView（4 列：实时☑ / 变量名 / 值 / 地址）
    let list = CreateWindowExW(
        WINDOW_EX_STYLE::default(),
        w!("SysListView32"),
        None,
        WINDOW_STYLE(
            WS_CHILD.0
                | WS_VISIBLE.0
                | WS_BORDER.0
                | LVS_REPORT
                | LVS_SINGLESEL
                | LVS_SHOWSELALWAYS,
        ),
        0,
        40,
        200,
        200,
        Some(hwnd),
        Some(HMENU(IDC_NUM_LIST as *mut c_void)),
        Some(hinst),
        None,
    )?;
    // 支持原生复选框（列 0）
    let ex = SendMessageW(list, LVM_SETEXTENDEDLISTVIEWSTYLE, Some(WPARAM(LVS_EX_CHECKBOXES as usize)), Some(LPARAM(LVS_EX_CHECKBOXES as isize)));
    let _ = ex;
    let cols: [(&str, i32); 4] = [
        ("实时", 60),
        ("变量名", 200),
        ("值", 140),
        ("地址", 120),
    ];
    for (ci, (name, cx)) in cols.iter().enumerate() {
        let mut wide: Vec<u16> = name.encode_utf16().collect();
        wide.push(0);
        let mut lv = LVCOLUMNW::default();
        lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
        lv.fmt = LVCFMT_LEFT;
        lv.cx = *cx;
        lv.pszText = PWSTR(wide.as_mut_ptr());
        let _ = SendMessageW(list, LVM_INSERTCOLUMNW, Some(WPARAM(ci)), Some(LPARAM(&raw mut lv as isize)));
    }
    Ok(hwnd)
}

/// 数值窗口子控件（在 WM_SIZE 时重排）。
unsafe fn layout(hwnd: HWND) {
    let mut rc = RECT::default();
    let _ = GetClientRect(hwnd, &mut rc);
    let w = rc.right - rc.left;
    let h = rc.bottom - rc.top;
    let list = GetDlgItem(Some(hwnd), IDC_NUM_LIST).unwrap_or_default();
    if !list.0.is_null() {
        let _ = MoveWindow(list, 8, 40, (w - 16).max(80), (h - 48).max(80), true);
    }
    let tip = GetDlgItem(Some(hwnd), IDC_NUM_TIP).unwrap_or_default();
    if !tip.0.is_null() {
        let _ = MoveWindow(tip, 224, 10, (w - 240).max(120), 20, true);
    }
}

/// 从 acq.leaves 刷新 ListView：重建行（跟随窗口系列），更新值列。
unsafe fn refresh_list(list: HWND) {
    let _ = SendMessageW(list, LVM_DELETEALLITEMS, None, None);
    let parent = GetParent(list).unwrap_or_default();
    let win_idx = winmgr::win_index_from_id(GetDlgCtrlID(parent));
    if win_idx.is_none() {
        return;
    }
    let mut rows: Vec<(usize, String, f32, u64)> = Vec::new(); // (leaf_idx, name, latest, addr)
    let mut live: Vec<bool> = Vec::new();
    let mut target = usize::MAX;
    if let Some(app) = get_app_mut() {
        let win_idx = win_idx.unwrap();
        if let Some(w) = app.wins.get(win_idx) {
            let acq = app.acq.lock().unwrap_or_else(|e| e.into_inner());
            for &si in &w.series {
                if let Some(l) = acq.leaves.get(si) {
                    rows.push((si, l.name.clone(), l.latest, l.address));
                }
            }
        }
        // 读取窗口状态里的 live/target
        let p = GetWindowLongPtrW(parent, GWLP_USERDATA) as *mut NumWinState;
        if !p.is_null() {
            let st = &*p;
            live = st.live.clone();
            target = st.target;
        }
        drop(app);
    }
    // 重建状态尺寸
    {
        let p = GetWindowLongPtrW(parent, GWLP_USERDATA) as *mut NumWinState;
        if !p.is_null() {
            let st = &mut *p;
            st.live.resize(rows.len(), true);
            // target 越界则失效
            if st.target != usize::MAX
                && !rows.iter().any(|(i, _, _, _)| *i == st.target)
            {
                st.target = usize::MAX;
            }
            live = st.live.clone();
            target = st.target;
        }
    }
    if rows.is_empty() {
        // 空提示行
        let mut item = LVITEMW::default();
        item.mask = LVIF_TEXT | LVIF_PARAM;
        let mut hint: Vec<u16> = "（右键变量树 → 添加变量到数值窗口）"
            .encode_utf16()
            .collect();
        hint.push(0);
        item.pszText = PWSTR(hint.as_mut_ptr());
        let _ = SendMessageW(list, LVM_INSERTITEMW, None, Some(LPARAM(&raw mut item as isize)));
        return;
    }
    for (ri, (leaf_idx, name, latest, addr)) in rows.iter().enumerate() {
        let ri32 = ri as i32;
        let mut item = LVITEMW::default();
        item.mask = LVIF_TEXT | LVIF_PARAM;
        item.iItem = ri32;
        item.lParam = LPARAM(*leaf_idx as isize + 1);
        let mut nm: Vec<u16> = name.encode_utf16().collect();
        nm.push(0);
        item.pszText = PWSTR(nm.as_mut_ptr());
        item.cchTextMax = 128;
        let _ = SendMessageW(list, LVM_INSERTITEMW, None, Some(LPARAM(&raw mut item as isize)));
        // 值列
        if live.get(ri).copied().unwrap_or(true) {
            let val = format!("{:.4}", latest);
            set_cell(list, ri32, 2, &val);
        } else {
            set_cell(list, ri32, 2, "—");
        }
        // 地址列
        set_cell(list, ri32, 3, &format!("0x{:X}", addr));
        // 勾选列状态
        let mut st = LVITEMW::default();
        st.mask = LVIF_STATE;
        st.stateMask = LVIS_STATEIMAGEMASK;
        st.state = LIST_VIEW_ITEM_STATE_FLAGS(if live.get(ri).copied().unwrap_or(true) {
            2 << 12
        } else {
            1 << 12
        });
        let _ = SendMessageW(list, LVM_SETITEMW, None, Some(LPARAM(&raw mut st as isize)));
    }
}

unsafe fn set_cell(list: HWND, row: i32, col: i32, text: &str) {
    let mut wide: Vec<u16> = text.encode_utf16().collect();
    wide.push(0);
    let mut item = LVITEMW::default();
    item.mask = LVIF_TEXT;
    item.iItem = row;
    item.iSubItem = col;
    item.pszText = PWSTR(wide.as_mut_ptr());
    let _ = SendMessageW(list, LVM_SETITEMTEXTW, Some(WPARAM(row as usize)), Some(LPARAM(&raw mut item as isize)));
}

/// 解析编辑框数值 → 按 size 打包小端字节；解析失败返回 None。
fn parse_number(text: &str, size: u32) -> Option<Vec<u8>> {
    let t = text.trim();
    if t.is_empty() {
        return None;
    }
    if t.contains('.') || t.eq_ignore_ascii_case("nan") || t.eq_ignore_ascii_case("inf")
        || t.to_lowercase().contains('e')
    {
        let v: f32 = t.parse().ok()?;
        let mut b = v.to_le_bytes().to_vec();
        b.truncate(size.min(4) as usize);
        return Some(b);
    }
    // 十六进制
    if let Some(h) = t.strip_prefix("0x").or_else(|| t.strip_prefix("0X")) {
        let v = u64::from_str_radix(h, 16).ok()?;
        return Some(v.to_le_bytes()[..size.min(8) as usize].to_vec());
    }
    // 十进制整数（支持负号）
    if let Ok(v) = i64::from_str_radix(t, 10) {
        return Some(v.to_le_bytes()[..size.min(8) as usize].to_vec());
    }
    let v: u64 = t.parse().ok()?;
    Some(v.to_le_bytes()[..size.min(8) as usize].to_vec())
}

/// 点"写入"：把编辑框数值写到当前选中变量地址。
unsafe fn do_write(hwnd: HWND) {
    let parent = hwnd;
    let edit = GetDlgItem(Some(parent), IDC_NUM_EDIT).unwrap_or_default();
    let list = GetDlgItem(Some(parent), IDC_NUM_LIST).unwrap_or_default();
    if edit.0.is_null() {
        return;
    }
    // 当前选中行
    let sel = SendMessageW(list, LVM_GETNEXTITEM, Some(WPARAM(usize::MAX)), Some(LPARAM(LVNI_SELECTED as isize)));
    let leaf_idx = if sel.0 != -1 {
        let mut item = LVITEMW::default();
        item.mask = LVIF_PARAM;
        item.iItem = sel.0 as i32;
        let _ = SendMessageW(list, LVM_GETITEMW, None, Some(LPARAM(&raw mut item as isize)));
        (item.lParam.0 - 1) as usize
    } else {
        let p = GetWindowLongPtrW(parent, GWLP_USERDATA) as *mut NumWinState;
        if p.is_null() {
            return;
        }
        (*p).target
    };
    // 读取编辑框文本
    let len = GetWindowTextLengthW(edit);
    let mut buf = vec![0u16; (len + 1) as usize];
    let n = GetWindowTextW(edit, &mut buf);
    let text: String = String::from_utf16_lossy(&buf[..n as usize]);
    if let Some(app) = get_app_mut() {
        let info = {
            let acq = app.acq.lock().unwrap_or_else(|e| e.into_inner());
            acq.leaves.get(leaf_idx).map(|l| (l.address, l.size, l.name.clone()))
        };
        let Some((address, size, name)) = info else {
            app.log("数值窗口：目标变量未登记");
            return;
        };
        let Some(data) = parse_number(&text, size) else {
            app.log(&format!("数值窗口：\"{}\" 不是合法数值", text));
            return;
        };
        let Some(jlink) = &app.jlink else {
            app.log("数值窗口：J-Link 未加载/未连接");
            return;
        };
        let req = openscope_jlink::MemReq {
            address,
            size,
            data,
        };
        match jlink.write_mem(&req) {
            Ok(()) => app.log(&format!("已写入 {} <- {}", name, text)),
            Err(rc) => app.log(&format!("写入 {} 失败 rc={}", name, rc)),
        }
    }
}

unsafe extern "system" fn num_wndproc(
    hwnd: HWND,
    msg: u32,
    wparam: WPARAM,
    lparam: LPARAM,
) -> LRESULT {
    match msg {
        WM_CREATE => LRESULT(0),
        WM_SIZE => {
            layout(hwnd);
            LRESULT(0)
        }
        WM_ERASEBKGND => LRESULT(1),
        WM_PAINT => {
            // 刷新值（由主窗口定时器 InvalidRect 触发）
            let list = GetDlgItem(Some(hwnd), IDC_NUM_LIST).unwrap_or_default();
            if !list.0.is_null() {
                refresh_list(list);
                let _ = RedrawWindow(Some(list), None, None, RDW_INVALIDATE | RDW_UPDATENOW);
            }
            let _ = ValidateRect(Some(hwnd), None);
            LRESULT(0)
        }
        WM_COMMAND => {
            let id = (wparam.0 & 0xffff) as i32;
            if id == IDC_NUM_WRITE {
                do_write(hwnd);
            }
            LRESULT(0)
        }
        WM_NOTIFY => {
            let p = lparam.0 as *const NMHDR;
            if !p.is_null() {
                let hdr = &*p;
                if hdr.idFrom == IDC_NUM_LIST as usize && hdr.code == LVN_ITEMCHANGED {
                    // 勾选切换 → 更新 live；选中 → 记录 target
                    let nm = &*(p as *const NMLISTVIEW);
                    if nm.iItem >= 0 {
                        let p_state = GetWindowLongPtrW(hwnd, GWLP_USERDATA) as *mut NumWinState;
                        if !p_state.is_null() {
                            let st = &mut *p_state;
                            if (nm.iItem as usize) < st.live.len() {
                                let checked = (nm.uNewState & 0xf000) >> 12 == 2;
                                st.live[nm.iItem as usize] = checked;
                            }
                            if (nm.uNewState & LVIS_SELECTED.0) != 0 {
                                let mut item = LVITEMW::default();
                                item.mask = LVIF_PARAM;
                                item.iItem = nm.iItem;
                                let list = GetDlgItem(Some(hwnd), IDC_NUM_LIST).unwrap_or_default();
                                let _ = SendMessageW(
                                    list,
                                    LVM_GETITEMW,
                                    None,
                                    Some(LPARAM(&raw mut item as isize)),
                                );
                                let idx = (item.lParam.0 - 1) as usize;
                                st.target = idx;
                            }
                        }
                    }
                }
            }
            LRESULT(0)
        }
        WM_DESTROY => {
            let p = GetWindowLongPtrW(hwnd, GWLP_USERDATA) as *mut NumWinState;
            if !p.is_null() {
                drop(Box::from_raw(p));
            }
            LRESULT(0)
        }
        _ => DefWindowProcW(hwnd, msg, wparam, lparam),
    }
}
