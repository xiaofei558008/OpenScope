//! 主窗口框架（对应 C 版 main.c / mainwin.c）。

use std::ffi::c_void;
use std::sync::{Arc, Mutex};

use windows::core::*;
use windows::Win32::Foundation::*;
use windows::Win32::Graphics::Gdi::*;
use windows::Win32::System::LibraryLoader::*;
use windows::Win32::UI::Controls::*;
use windows::Win32::UI::Input::KeyboardAndMouse::*;
use windows::Win32::UI::WindowsAndMessaging::*;

use crate::acq::{AcqState, LeafBuf, LeafSpec};
use crate::winmgr::{self, OsWin, WinKind};

/// RGB 组合成 COLORREF（0x00BBGGRR）。
const fn rgb(r: u8, g: u8, b: u8) -> COLORREF {
    COLORREF((r as u32) | ((g as u32) << 8) | ((b as u32) << 16))
}

pub const WND_MAIN: &str = "OpenScopeMain";
pub const VERSION: &str = "2.0.2";
pub const CHART_CLASS: PCWSTR = w!("OpenScopeChart");

/// UI 刷新定时器 ID。
pub const TIMER_UI_REFRESH: usize = 1;

// 控件 ID
const IDC_BTN_CONNECT: i32 = 2002;
const IDC_BTN_DISCONNECT: i32 = 2003;
const IDC_BTN_START: i32 = 2004;
const IDC_BTN_STOP: i32 = 2005;
const IDC_BTN_OPEN: i32 = 2006;
const IDC_BTN_LOAD_ELF: i32 = 2007;
const IDC_TREE: i32 = 2100;
const IDC_LOG: i32 = 2101;
const IDC_TAB: i32 = 2102;
const IDC_STATUS: i32 = 2103;

// 菜单 ID（与按钮 ID 同为 i32，WM_COMMAND 的 id 是 (wparam & 0xffff) as i32）
const IDM_ABOUT: i32 = 9001;
const IDM_EXIT: i32 = 9002;
const IDM_OPEN_ELF: i32 = 9003;
// 布局/主题（与 C 版一致；功能尚未移植，菜单置灰）
const IDM_LAYOUT_SAVE: i32 = 2021;
const IDM_LAYOUT_LOAD: i32 = 2022;
const IDM_THEME_DARK: i32 = 2701;
// 记录/回放（尚未移植，菜单置灰）
const IDM_REC_LOGSTART: i32 = 2023;
const IDM_REC_LOGSTOP: i32 = 2024;
const IDM_REC_REPLAY: i32 = 2025;
const IDM_REC_REPLAYSTOP: i32 = 2026;
// 窗口
const IDM_WIN_CHART: i32 = 2012;
const IDM_WIN_NUM: i32 = 2013;

// 变量树右键菜单命令
const IDM_CTX_ADD_CHART: i32 = 9501;
const IDM_CTX_ADD_NUM: i32 = 9502;
const IDM_CTX_CHECK_ALL: i32 = 9503;
const IDM_CTX_UNCHECK_ALL: i32 = 9504;

const WC_BUTTON: PCWSTR = w!("BUTTON");
const WC_TREEVIEW: PCWSTR = w!("SysTreeView32");
const WC_LISTVIEW: PCWSTR = w!("SysListView32");
const WC_TABCONTROL: PCWSTR = w!("SysTabControl32");
const STATUS_CLASS: PCWSTR = w!("msctls_statusbar32");

pub struct App {
    hinst: HINSTANCE,
    pub hmain: HWND,
    pub htree: HWND,
    pub hlog: HWND,
    pub htab: HWND,
    pub hstatus: HWND,
    pub btn_connect: HWND,
    pub btn_disconnect: HWND,
    pub btn_start: HWND,
    pub btn_stop: HWND,
    pub btn_load: HWND,
    /// 右侧窗口列表（每窗口一个 tab）。
    pub wins: Vec<OsWin>,
    /// 已加载的 ELF 解析结果（None=未加载）。
    pub elf: Option<openscope_elf::ElfFile>,
    /// ELF 文件路径（供热加载/重载）。
    pub elf_path: String,
    /// J-Link 驱动句柄（加载后持有；Arc 供采集线程共享）。
    pub jlink: Option<Arc<openscope_jlink::Jlink>>,
    /// 已连接标记。
    pub connected: bool,
    /// 采集共享状态（环形缓冲等）。
    pub acq: Arc<Mutex<AcqState>>,
    /// 采集线程句柄。
    pub acq_thread: Option<std::thread::JoinHandle<()>>,
}

impl App {
    pub fn new(hinst: HINSTANCE) -> App {
        App {
            hinst,
            hmain: HWND::default(),
            htree: HWND::default(),
            hlog: HWND::default(),
            htab: HWND::default(),
            hstatus: HWND::default(),
            btn_connect: HWND::default(),
            btn_disconnect: HWND::default(),
            btn_start: HWND::default(),
            btn_stop: HWND::default(),
            btn_load: HWND::default(),
            wins: Vec::new(),
            elf: None,
            elf_path: String::new(),
            jlink: None,
            connected: false,
            acq: Arc::new(Mutex::new(AcqState::default())),
            acq_thread: None,
        }
    }

    /// 注册主窗口类并创建主窗口。
    pub unsafe fn create_main_window(&mut self) -> Result<HWND> {
        let class = w!("OpenScopeMain");
        let wc = WNDCLASSW {
            style: CS_HREDRAW | CS_VREDRAW,
            lpfnWndProc: Some(wndproc),
            hInstance: self.hinst,
            lpszClassName: class,
            hbrBackground: CreateSolidBrush(rgb(240, 240, 240)),
            hIcon: LoadIconW(Some(self.hinst), PCWSTR(1 as *const u16)).unwrap_or_default(),
            hCursor: LoadCursorW(None, IDC_ARROW).unwrap_or_default(),
            ..Default::default()
        };
        let atom = RegisterClassW(&wc);
        if atom == 0 {
            return Err(Error::from_win32());
        }

        // 注册图表窗口类与数值窗口类
        let _ = crate::chart::register_class(self.hinst)?;
        let _ = crate::numwin::register_class(self.hinst)?;

        let title = format!("OpenScope v{} - MCU 变量采集与标定", VERSION);
        let title_wide: Vec<u16> = title.encode_utf16().chain(std::iter::once(0)).collect();
        let hwnd = CreateWindowExW(
            WINDOW_EX_STYLE::default(),
            class,
            PCWSTR(title_wide.as_ptr()),
            WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            1280,
            820,
            None,
            None,
            Some(self.hinst),
            None,
        )?;
        self.hmain = hwnd;
        self.create_children()?;
        self.create_menu_bar()?;
        self.layout(1280, 820);
        Ok(hwnd)
    }

    /// 复刻 C 版菜单栏（文件/采集/记录回放/窗口/帮助，见 code/src/mainwin.c:2363）。
    /// 已移植的功能菜单项可用；尚未移植（布局、深色模式、CSV记录、回放、窗口管理）
    /// 置灰，待对应功能落地后点亮。
    unsafe fn create_menu_bar(&mut self) -> Result<()> {
        let mfile = CreateMenu()?;
        let macq = CreateMenu()?;
        let mlog = CreateMenu()?;
        let mwin = CreateMenu()?;
        let mhelp = CreateMenu()?;

        // 文件(&F)
        AppendMenuW(mfile, MF_STRING, IDM_OPEN_ELF as usize, w!("打开 ELF 文件...\tCtrl+O"))?;
        AppendMenuW(mfile, MF_STRING | MF_GRAYED, IDM_LAYOUT_SAVE as usize, w!("保存布局为..."))?;
        AppendMenuW(mfile, MF_STRING | MF_GRAYED, IDM_LAYOUT_LOAD as usize, w!("加载布局..."))?;
        AppendMenuW(mfile, MF_SEPARATOR, 0, PCWSTR::null())?;
        AppendMenuW(mfile, MF_STRING | MF_GRAYED, IDM_THEME_DARK as usize, w!("深色模式"))?;
        AppendMenuW(mfile, MF_SEPARATOR, 0, PCWSTR::null())?;
        AppendMenuW(mfile, MF_STRING, IDM_EXIT as usize, w!("退出\tAlt+F4"))?;

        // 采集(&A)：菜单 ID 复用按钮 ID，WM_COMMAND 同一套处理
        AppendMenuW(macq, MF_STRING, IDC_BTN_CONNECT as usize, w!("连接...\tF5"))?;
        AppendMenuW(macq, MF_STRING, IDC_BTN_DISCONNECT as usize, w!("断开\tF6"))?;
        AppendMenuW(macq, MF_SEPARATOR, 0, PCWSTR::null())?;
        AppendMenuW(macq, MF_STRING, IDC_BTN_START as usize, w!("开始采集\tF7"))?;
        AppendMenuW(macq, MF_STRING, IDC_BTN_STOP as usize, w!("停止采集\tF8"))?;

        // 记录/回放(&L)：CSV 记录/离线回放尚未移植，全部置灰
        AppendMenuW(mlog, MF_STRING | MF_GRAYED, IDM_REC_LOGSTART as usize, w!("开始 CSV 记录..."))?;
        AppendMenuW(mlog, MF_STRING | MF_GRAYED, IDM_REC_LOGSTOP as usize, w!("停止记录"))?;
        AppendMenuW(mlog, MF_SEPARATOR, 0, PCWSTR::null())?;
        AppendMenuW(mlog, MF_STRING | MF_GRAYED, IDM_REC_REPLAY as usize, w!("离线回放..."))?;
        AppendMenuW(mlog, MF_STRING | MF_GRAYED, IDM_REC_REPLAYSTOP as usize, w!("停止回放"))?;

        // 窗口(&W)：波形/数值窗口
        AppendMenuW(mwin, MF_STRING, IDM_WIN_CHART as usize, w!("波形窗口"))?;
        AppendMenuW(mwin, MF_STRING, IDM_WIN_NUM as usize, w!("数值窗口"))?;

        // 帮助(&H)
        AppendMenuW(mhelp, MF_STRING, IDM_ABOUT as usize, w!("关于 OpenScope"))?;

        let gmenu = CreateMenu()?;
        AppendMenuW(gmenu, MF_POPUP | MF_STRING, mfile.0 as usize, w!("文件(&F)"))?;
        AppendMenuW(gmenu, MF_POPUP | MF_STRING, macq.0 as usize, w!("采集(&A)"))?;
        AppendMenuW(gmenu, MF_POPUP | MF_STRING, mlog.0 as usize, w!("记录/回放(&L)"))?;
        AppendMenuW(gmenu, MF_POPUP | MF_STRING, mwin.0 as usize, w!("窗口(&W)"))?;
        AppendMenuW(gmenu, MF_POPUP | MF_STRING, mhelp.0 as usize, w!("帮助(&H)"))?;

        SetMenu(self.hmain, Some(gmenu))?;
        Ok(())
    }

    /// 新增一个窗口（波形/数值），作为新 tab 插入右侧并选中显示。
    unsafe fn add_window(&mut self, kind: WinKind) -> Result<usize> {
        let idx = self.wins.len();
        let hwnd = match kind {
            WinKind::Chart => winmgr::create_chart_hwnd(self.htab, self.hinst, idx)?,
            WinKind::Number => winmgr::create_number_hwnd(self.htab, self.hinst, idx)?,
        };
        let title = match kind {
            WinKind::Chart => format!("波形{}", idx + 1),
            WinKind::Number => format!("数值{}", idx + 1),
        };
        winmgr::insert_tab_item(self.htab, idx, &title);
        self.wins.push(OsWin {
            kind,
            hwnd,
            title: title.clone(),
            series: Vec::new(),
        });
        let _ = SendMessageW(self.htab, TCM_SETCURSEL, Some(WPARAM(idx)), None);
        // 显示新窗口、隐藏其余（直接内联，避免经 get_app_mut 二次可变借用）
        for (i, w) in self.wins.iter().enumerate() {
            let _ = ShowWindow(w.hwnd, if i == idx { SW_SHOWNA } else { SW_HIDE });
        }
        // 立即铺满 tab 显示区
        let _ = MoveWindow(hwnd, 0, 22, 1, 1, true);
        let _ = self.reposition_windows();
        self.log(&format!(
            "已添加{}窗口（{}）",
            if matches!(kind, WinKind::Chart) {
                "波形"
            } else {
                "数值"
            },
            title
        ));
        Ok(idx)
    }

    /// 把全部窗口子控件重新铺满 tab 显示区。
    unsafe fn reposition_windows(&self) -> i32 {
        let mut rect = RECT::default();
        let _ = GetClientRect(self.htab, &mut rect);
        let tab_w = rect.right - rect.left;
        let tab_h = rect.bottom - rect.top;
        for w_ in &self.wins {
            let _ = MoveWindow(w_.hwnd, 0, 22, tab_w, tab_h - 22, true);
        }
        tab_h - 22
    }

    /// 确保 ELF 变量 leaf_idx 已登记为采集叶（不重复），返回其在 acq.leaves 中的下标。
    pub fn register_leaf(&mut self, leaf_idx: usize) -> Option<usize> {
        let info = self.elf.as_ref().and_then(|elf| elf.var_at(leaf_idx))?;
        let name = info.name.clone();
        let address = info.address;
        let size = info.symbol_size as u32;
        let mut is_new = false;
        let idx = {
            let mut acq = self.acq.lock().unwrap_or_else(|e| e.into_inner());
            if let Some(p) = acq.leaves.iter().position(|l| l.name == name) {
                p
            } else {
                acq.leaves.push(LeafBuf::from_elf(name.clone(), address, size));
                is_new = true;
                acq.leaves.len() - 1
            }
        };
        if is_new {
            self.log(&format!("已登记变量: {}", name));
        }
        Some(idx)
    }

    /// 快捷键表：Ctrl+O 打开 ELF；F5-F8 对应采集菜单（ID 复用按钮 ID）。
    pub unsafe fn create_accelerators() -> Result<HACCEL> {
        let accel = [
            ACCEL {
                fVirt: FVIRTKEY | FCONTROL,
                key: b'O' as u16,
                cmd: IDM_OPEN_ELF as u16,
            },
            ACCEL { fVirt: FVIRTKEY, key: VK_F5.0, cmd: IDC_BTN_CONNECT as u16 },
            ACCEL { fVirt: FVIRTKEY, key: VK_F6.0, cmd: IDC_BTN_DISCONNECT as u16 },
            ACCEL { fVirt: FVIRTKEY, key: VK_F7.0, cmd: IDC_BTN_START as u16 },
            ACCEL { fVirt: FVIRTKEY, key: VK_F8.0, cmd: IDC_BTN_STOP as u16 },
        ];
        CreateAcceleratorTableW(&accel)
    }

    unsafe fn create_children(&mut self) -> Result<()> {
        let par = self.hmain;
        // 顶栏按钮
        self.btn_load = make_button(par, IDC_BTN_LOAD_ELF, w!("加载 ELF"), 8, 6, 80, 26)?;
        self.btn_connect = make_button(par, IDC_BTN_CONNECT, w!("连接"), 94, 6, 70, 26)?;
        self.btn_disconnect = make_button(par, IDC_BTN_DISCONNECT, w!("断开"), 168, 6, 70, 26)?;
        self.btn_start = make_button(par, IDC_BTN_START, w!("开始"), 242, 6, 70, 26)?;
        self.btn_stop = make_button(par, IDC_BTN_STOP, w!("停止"), 316, 6, 70, 26)?;
        // 左侧变量树
        self.htree = CreateWindowExW(
            WINDOW_EX_STYLE::default(),
            WC_TREEVIEW,
            None,
            WINDOW_STYLE(
                WS_CHILD.0 | WS_VISIBLE.0 | WS_BORDER.0 | TVS_HASLINES | TVS_LINESATROOT
                    | TVS_HASBUTTONS | TVS_SHOWSELALWAYS | TVS_CHECKBOXES | TVS_NOHSCROLL,
            ),
            0,
            0,
            0,
            0,
            Some(par),
            Some(HMENU(IDC_TREE as *mut c_void)),
            Some(self.hinst),
            None,
        )?;
        // 右侧 tab
        self.htab = CreateWindowExW(
            WINDOW_EX_STYLE::default(),
            WC_TABCONTROL,
            None,
            WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
            0,
            0,
            0,
            0,
            Some(par),
            Some(HMENU(IDC_TAB as *mut c_void)),
            Some(self.hinst),
            None,
        )?;
        // 初始窗口：第一个波形 tab（后续菜单/右键可继续添加）
        self.add_window(WinKind::Chart)?;
        // 底部日志
        self.hlog = CreateWindowExW(
            WINDOW_EX_STYLE::default(),
            WC_LISTVIEW,
            None,
            WINDOW_STYLE(WS_CHILD.0 | WS_VISIBLE.0 | WS_BORDER.0 | LVS_REPORT | LVS_NOSORTHEADER),
            0,
            0,
            0,
            0,
            Some(par),
            Some(HMENU(IDC_LOG as *mut c_void)),
            Some(self.hinst),
            None,
        )?;
        // 状态栏
        self.hstatus = CreateWindowExW(
            WINDOW_EX_STYLE::default(),
            STATUS_CLASS,
            None,
            WS_CHILD | WS_VISIBLE,
            0,
            0,
            0,
            0,
            Some(par),
            Some(HMENU(IDC_STATUS as *mut c_void)),
            Some(self.hinst),
            None,
        )?;
        // 日志列
        let mut col_text: Vec<u16> = "消息".encode_utf16().collect();
        col_text.push(0);
        let hdr = &[
            LVCOLUMNW {
                mask: LVCF_TEXT | LVCF_WIDTH | LVCF_FMT,
                fmt: LVCFMT_LEFT,
                cx: 640,
                pszText: PWSTR(col_text.as_mut_ptr()),
                ..Default::default()
            },
        ];
        let _ = SendMessageW(self.hlog, LVM_INSERTCOLUMNW, Some(WPARAM(0)), Some(LPARAM(hdr.as_ptr() as isize)));
        Ok(())
    }

    /// 布局子窗口。传入客户区宽高。
    pub unsafe fn layout(&mut self, w: i32, h: i32) {
        let btn_h = 38;
        let log_h = 140;
        let status_h = 22;
        let tree_w = 320;
        let y0 = btn_h;
        // 按钮已在 create 时绝对定位（顶栏），随宽度自适应可后续做
        let _ = MoveWindow(self.htree, 0, y0, tree_w, h - y0 - log_h - status_h, true);
        let tab_w = w - tree_w - 8;
        let tab_h = h - y0 - log_h - status_h;
        let _ = MoveWindow(self.htab, tree_w + 4, y0, tab_w, tab_h, true);
        // 每个窗口占据 tab 显示区（tab 行 ~22px）
        for w_ in &self.wins {
            let _ = MoveWindow(w_.hwnd, 0, 22, tab_w, tab_h - 22, true);
        }
        let _ = MoveWindow(
            self.hlog,
            0,
            h - log_h - status_h,
            w,
            log_h,
            true,
        );
        let _ = MoveWindow(self.hstatus, 0, h - status_h, w, status_h, true);
    }

    /// 弹出文件对话框加载 ELF 并填充变量树。
    pub unsafe fn load_elf_dialog(&mut self) {
        let Some(path) = crate::ffi::open_elf_dialog(self.hmain) else {
            return;
        };
        self.load_elf_path(&path);
    }

    /// 加载指定路径的 ELF；成功填充变量树，失败写日志。
    pub unsafe fn load_elf_path(&mut self, path: &str) {
        match openscope_elf::open_elf(std::path::Path::new(path)) {
            Ok(elf) => {
                let n = elf.var_count();
                self.elf = Some(elf);
                self.elf_path = path.to_string();
                self.fill_tree();
                self.log(&format!("已加载 ELF: {}（{} 个全局符号）", path, n));
            }
            Err(e) => {
                self.log(&format!("ELF 加载失败: {}", e));
            }
        }
    }

    /// 把 ELF 顶层变量（以及结构体/数组的子节点）填入左侧树。
    pub unsafe fn fill_tree(&self) {
        // TVI_ROOT = (HTREEITEM)(ULONG_PTR)-0x10000；传 -1 不是合法句柄，删除会失败导致重复加载时树叠加
        let _ = SendMessageW(
            self.htree,
            TVM_DELETEITEM,
            Some(WPARAM(0)),
            Some(LPARAM(-0x10000)),
        );
        let Some(elf) = &self.elf else { return };
        for i in 0..elf.var_count() {
            let v = elf.var_at(i).unwrap();
            let mut text = format!("{}  @0x{:X}", v.name, v.address);
            if v.symbol_size > 0 {
                text.push_str(&format!("  [{} B]", v.symbol_size));
            }
            tree_insert(self.htree, None, &text, i as isize + 1);
        }
    }

    /// 写日志（暂存到 UI 列表，后续接入 ListView）。
    pub fn log(&mut self, msg: &str) {
        os_log(self.hmain, msg);
    }

    /// 查找 JLink_x64.dll：exe 目录 → 当前目录 → ./dll。
    fn find_jlink_dll(&self) -> std::path::PathBuf {
        if let Ok(exe) = std::env::current_exe() {
            if let Some(dir) = exe.parent() {
                let p = dir.join("JLink_x64.dll");
                if p.exists() {
                    return p;
                }
            }
        }
        let p = std::path::PathBuf::from("dll/JLink_x64.dll");
        if p.exists() {
            return p;
        }
        std::path::PathBuf::from("JLink_x64.dll")
    }

    /// 加载 J-Link DLL（与 C 版一致：模块初始化时尽早加载，避免在 wndproc 内首次 LoadLibrary）。
    pub unsafe fn load_jlink(&mut self) -> std::result::Result<(), String> {
        if self.jlink.is_none() {
            let dll_path = self.find_jlink_dll();
            let dir = dll_path
                .parent()
                .map(|d| d.to_path_buf())
                .unwrap_or_default();
            self.jlink = Some(Arc::new(openscope_jlink::Jlink::load(&dir)?));
        }
        Ok(())
    }

    /// 连接 J-Link（AD-JLINK 序列）。成功返回驱动信息字符串。
    pub unsafe fn connect(&mut self) -> std::result::Result<String, String> {
        // 懒加载 DLL（兜底；正常路径在 create_main_window 前已加载）
        self.load_jlink()?;
        // 设备名：默认 "Cortex-M4"（实测仅此名被 DLL 识别；"CORTEX-M4T" 等均失败）。
        // 可用环境变量 OPENSC_DEVICE 覆盖，供后续设备选择 UI 接入。
        let device = std::env::var("OPENSC_DEVICE").unwrap_or_else(|_| "Cortex-M4".into());
        let mut cfg = openscope_jlink::ConnectCfg {
            device,
            iface_jtag: false,
            speed_khz: 4000,
            probe_index: -1,
            serial: String::new(),
        };
        let mut scan_log = String::new();
        if std::env::var("OPENSC_NO_SCAN").as_deref() != Ok("1") {
            let jlink = self.jlink.as_ref().unwrap();
            // 自动扫描仿真器，优先用序列号选择（J-Link PRO 上按 index 选择会失败 rc=-1）
            let probes = jlink.scan();
            if let Some(first) = probes.first() {
                cfg.serial = first.serial_number.to_string();
                scan_log = format!(
                    "扫描到仿真器: {} (SN:{})",
                    first.product(),
                    first.serial_number
                );
            }
        } else {
            // 诊断：跳过 scan，直接用序列号（与 C 版按钮路径一致：用缓存的仿真器列表）
            cfg.serial = std::env::var("OPENSC_SERIAL").unwrap_or_else(|_| "174504925".into());
            scan_log = format!("[no-scan] 直接按序列号 {} 连接", cfg.serial);
        }
        if !scan_log.is_empty() {
            self.log(&scan_log);
        }
        let mut logs: Vec<String> = Vec::new();
        let rc = {
            let jlink = self.jlink.as_ref().unwrap();
            jlink.connect(&cfg, &mut |m| {
                // 逐条实时打印，便于定位 connect 中挂起的具体 DLL 调用
                os_log(self.hmain, m);
                logs.push(m.to_string())
            })
        };
        for m in &logs {
            self.log(m);
        }
        if rc.is_ok() {
            self.connected = true;
            let info = {
                let jlink = self.jlink.as_ref().unwrap();
                jlink.info()
            };
            self.update_status(&format!(
                "已连接 {} (DLL {}  HW {})",
                info.emulator, info.dll_version, info.hw_version
            ));
            Ok(info.dll_version)
        } else {
            self.connected = false;
            self.update_status("连接失败");
            Err(rc.unwrap_err())
        }
    }

    /// 断开 J-Link。
    pub unsafe fn disconnect(&mut self) {
        // 先停止采集
        self.stop_acq();
        if let Some(j) = &self.jlink {
            j.disconnect();
        }
        self.connected = false;
        self.update_status("未连接");
        self.log("已断开连接");
    }

    /// 开始采集：登记当前勾选叶变量，派生采集线程，启动 UI 刷新定时器。
    pub unsafe fn start_acq(&mut self) {
        if !self.connected {
            self.log("请先连接 J-Link");
            return;
        }
        let Some(jlink) = self.jlink.clone() else {
            self.log("J-Link 未加载");
            return;
        };
        let specs: Vec<LeafSpec> = {
            let acq = self.acq.lock().unwrap_or_else(|e| e.into_inner());
            acq.leaves
                .iter()
                .map(|l| LeafSpec::new(&l.name, l.address, l.size))
                .collect()
        };
        if specs.is_empty() {
            self.log("请先在左侧变量树勾选要采集的变量");
            return;
        }
        let n = specs.len();
        let handle = crate::acq::start(jlink, self.acq.clone(), specs);
        if handle.is_none() {
            self.log("采集已在运行");
            return;
        }
        self.acq_thread = handle;
        let _ = SetTimer(Some(self.hmain), TIMER_UI_REFRESH, 33, None);
        self.log(&format!("采集开始：{} 个变量", n));
    }

    /// 停止采集并停掉 UI 定时器。
    pub unsafe fn stop_acq(&mut self) {
        if self.acq_thread.is_some() {
            crate::acq::stop(&self.acq, self.acq_thread.take());
            let _ = KillTimer(Some(self.hmain), TIMER_UI_REFRESH);
            self.log("采集停止");
        }
    }

    /// 变量树勾选变化：把对应 ELF 顶层符号加入/移出采集列表。
    pub fn toggle_leaf(&mut self, leaf_idx: usize, checked: bool) {
        let info = self.elf.as_ref().and_then(|elf| elf.var_at(leaf_idx));
        let Some(v) = info else {
            return;
        };
        let name = v.name.clone();
        let address = v.address;
        let size = v.symbol_size as u32;
        let mut msg = String::new();
        {
            let mut acq = self.acq.lock().unwrap_or_else(|e| e.into_inner());
            let pos = acq.leaves.iter().position(|l| l.name == name);
            if checked {
                if pos.is_none() {
                    acq.leaves
                        .push(LeafBuf::from_elf(name.clone(), address, size));
                    msg = format!("已添加变量: {}", name);
                }
            } else if let Some(p) = pos {
                acq.leaves.remove(p);
                msg = format!("已移除变量: {}", name);
            }
        }
        if !msg.is_empty() {
            self.log(&msg);
        }
    }

    /// 设置状态栏文本（第 0 段）。
    pub fn update_status(&self, text: &str) {
        let mut text_wide: Vec<u16> = text.encode_utf16().collect();
        text_wide.push(0);
        unsafe {
            let _ = SendMessageW(
                self.hstatus,
                SB_SETTEXT,
                Some(WPARAM(0)),
                Some(LPARAM(text_wide.as_ptr() as isize)),
            );
        }
    }

    /// 收集变量树中已勾选节点的 ELF 下标（lParam 编码：idx+1）。
    fn checked_leaf_idxs(&self) -> Vec<usize> {
        let mut out = Vec::new();
        unsafe {
            let root = SendMessageW(
                self.htree,
                TVM_GETNEXTITEM,
                Some(WPARAM(TVGN_ROOT as usize)),
                None,
            )
            .0;
            if root == 0 {
                return out;
            }
            let mut cur = root;
            while cur != 0 {
                let mut item = TVITEMW::default();
                item.hItem = HTREEITEM(cur);
                item.mask = TVIF_PARAM | TVIF_STATE;
                item.stateMask = TVIS_STATEIMAGEMASK;
                let r = SendMessageW(self.htree, TVM_GETITEMW, None, Some(LPARAM(&raw mut item as isize)));
                if r.0 != 0 {
                    // 勾选 = 状态图索引 2（1=未勾，2=已勾）
                    let state_img = (item.state.0 & 0xf000) >> 12;
                    let idx = item.lParam.0 - 1;
                    if state_img == 2 && idx >= 0 {
                        out.push(idx as usize);
                    }
                }
                cur = SendMessageW(
                    self.htree,
                    TVM_GETNEXTITEM,
                    Some(WPARAM(TVGN_NEXT as usize)),
                    Some(LPARAM(cur)),
                )
                .0;
            }
        }
        out
    }

    /// 右键菜单：把勾选变量加到目标窗口（无对应类型窗口时自动新建）。
    unsafe fn ctx_add_checked_to(&mut self, kind: WinKind) {
        let checked = self.checked_leaf_idxs();
        if checked.is_empty() {
            self.log("请先在变量树勾选要添加的变量");
            return;
        }
        // 目标窗口：优先当前选中 tab（类型匹配时），否则该类型最近一个，否则新建
        let cur = winmgr::current_tab(self.htab);
        let mut win_idx = self.wins.get(cur).filter(|w| w.kind == kind).map(|_| cur);
        if win_idx.is_none() {
            win_idx = self.wins.iter().rposition(|w| w.kind == kind);
        }
        let win_idx = match win_idx {
            Some(i) => i,
            None => match self.add_window(kind) {
                Ok(i) => i,
                Err(e) => {
                    self.log(&format!("创建窗口失败: {}", e));
                    return;
                }
            },
        };
        // 登记变量（acq.leaves 去重）并加入窗口系列（去重）
        let mut added = 0usize;
        for &idx in &checked {
            if let Some(leaf) = self.register_leaf(idx) {
                let series = &mut self.wins[win_idx].series;
                if !series.contains(&leaf) {
                    series.push(leaf);
                    added += 1;
                }
            }
        }
        let title = self.wins[win_idx].title.clone();
        let _ = InvalidateRect(Some(self.wins[win_idx].hwnd), None, false);
        self.log(&format!("已添加 {} 个变量到 {}", added, title));
    }

    /// 右键菜单：全部勾选/全部取消勾选，并同步采集列表。
    unsafe fn ctx_check_all(&mut self, check: bool) {
        let root = SendMessageW(
            self.htree,
            TVM_GETNEXTITEM,
            Some(WPARAM(TVGN_ROOT as usize)),
            None,
        )
        .0;
        if root == 0 {
            return;
        }
        let mut cur = root;
        let mut set = TVITEMW::default();
        set.mask = TVIF_STATE | TVIF_HANDLE;
        set.stateMask = TVIS_STATEIMAGEMASK;
        // 勾选 = 状态图索引 2，取消 = 1（TVIS_STATEIMAGEMASK 高 12 位）
        set.state = TREE_VIEW_ITEM_STATE_FLAGS(if check { 2 << 12 } else { 1 << 12 });
        while cur != 0 {
            // 读取 lParam（更新 state 后 set 会覆盖 hItem，需单独取）
            let mut get = TVITEMW::default();
            get.hItem = HTREEITEM(cur);
            get.mask = TVIF_PARAM;
            let _ = SendMessageW(self.htree, TVM_GETITEMW, None, Some(LPARAM(&raw mut get as isize)));
            set.hItem = HTREEITEM(cur);
            let _ = SendMessageW(self.htree, TVM_SETITEMW, None, Some(LPARAM(&raw mut set as isize)));
            let idx = get.lParam.0 - 1;
            if idx >= 0 {
                self.toggle_leaf(idx as usize, check);
            }
            cur = SendMessageW(
                self.htree,
                TVM_GETNEXTITEM,
                Some(WPARAM(TVGN_NEXT as usize)),
                Some(LPARAM(cur)),
            )
            .0;
        }
        self.log(if check {
            "已全部勾选变量"
        } else {
            "已全部取消勾选变量"
        });
    }

    /// WM_CONTEXTMENU：在变量树显示右键菜单（添加变量到窗口/全选/取消全选）。
    unsafe fn show_tree_context_menu(&mut self, hwnd: HWND, wparam: WPARAM, lparam: LPARAM) {
        // wparam 是被点控件（键盘触发 Shift+F10 时也是焦点控件句柄）
        let ctl = HWND(wparam.0 as *mut c_void);
        if ctl != self.htree {
            return;
        }
        let mut pt = POINT::default();
        if (lparam.0 as usize) == 0xffff_ffff {
            // 键盘触发：用鼠标位置
            let _ = GetCursorPos(&mut pt);
        } else {
            pt.x = (lparam.0 & 0xffff) as i32;
            pt.y = ((lparam.0 >> 16) & 0xffff) as i32;
        }
        let Ok(menu) = CreatePopupMenu() else { return };
        let _ = AppendMenuW(menu, MF_STRING, IDM_CTX_ADD_CHART as usize, w!("添加变量到波形窗口"));
        let _ = AppendMenuW(menu, MF_STRING, IDM_CTX_ADD_NUM as usize, w!("添加变量到数值窗口"));
        let _ = AppendMenuW(menu, MF_SEPARATOR, 0, PCWSTR::null());
        let _ = AppendMenuW(menu, MF_STRING, IDM_CTX_CHECK_ALL as usize, w!("全部勾选"));
        let _ = AppendMenuW(menu, MF_STRING, IDM_CTX_UNCHECK_ALL as usize, w!("全部取消勾选"));
        let sel = TrackPopupMenu(
            menu,
            TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_LEFTALIGN,
            pt.x,
            pt.y,
            None,
            hwnd,
            None,
        );
        let _ = DestroyMenu(menu);
        // TPM_RETURNCMD：返回值即被选菜单项 ID（0=未选）
        match sel.0 as i32 {
            IDM_CTX_ADD_CHART => self.ctx_add_checked_to(WinKind::Chart),
            IDM_CTX_ADD_NUM => self.ctx_add_checked_to(WinKind::Number),
            IDM_CTX_CHECK_ALL => self.ctx_check_all(true),
            IDM_CTX_UNCHECK_ALL => self.ctx_check_all(false),
            _ => {}
        }
    }
}

unsafe fn make_button(
    parent: HWND,
    id: i32,
    text: PCWSTR,
    x: i32,
    y: i32,
    w: i32,
    h: i32,
) -> Result<HWND> {
    CreateWindowExW(
        WINDOW_EX_STYLE::default(),
        WC_BUTTON,
        text,
        WINDOW_STYLE(WS_CHILD.0 | WS_VISIBLE.0 | WS_TABSTOP.0 | BS_PUSHBUTTON as u32),
        x,
        y,
        w,
        h,
        Some(parent),
        Some(HMENU(id as *mut c_void)),
        Some(HINSTANCE(GetModuleHandleA(None)?.0)),
        None,
    )
}

/// 向 TreeView 插入一个节点（lParam 编码：id+1，0 表示无关联）。
fn tree_insert(hwnd: HWND, parent: Option<HTREEITEM>, text: &str, lparam: isize) -> Option<HTREEITEM> {
    unsafe {
        let mut text_wide: Vec<u16> = text.encode_utf16().collect();
        text_wide.push(0);
        let mut ins = TVINSERTSTRUCTW::default();
        ins.hParent = parent.unwrap_or_default();
        ins.hInsertAfter = TVI_LAST;
        ins.Anonymous.item.mask = TVIF_TEXT | TVIF_PARAM;
        ins.Anonymous.item.pszText = PWSTR(text_wide.as_mut_ptr());
        ins.Anonymous.item.lParam = LPARAM(lparam);
        let hr = SendMessageW(hwnd, TVM_INSERTITEMW, None, Some(LPARAM(&raw mut ins as isize)));
        if hr.0 == 0 {
            None
        } else {
            Some(HTREEITEM(hr.0))
        }
    }
}

unsafe extern "system" fn wndproc(hwnd: HWND, msg: u32, wparam: WPARAM, lparam: LPARAM) -> LRESULT {
    match msg {
        WM_COMMAND => {
            let id = (wparam.0 & 0xffff) as i32;
            match id {
                IDC_BTN_CONNECT => {
                    if let Some(app) = get_app_mut() {
                        match app.connect() {
                            Ok(_) => app.log("连接成功"),
                            Err(e) => app.log(&format!("连接失败: {}", e)),
                        }
                    }
                }
                IDC_BTN_DISCONNECT => {
                    if let Some(app) = get_app_mut() {
                        app.disconnect();
                    }
                }
                IDC_BTN_START => {
                    if let Some(app) = get_app_mut() {
                        app.start_acq();
                    }
                }
                IDC_BTN_STOP => {
                    if let Some(app) = get_app_mut() {
                        app.stop_acq();
                    }
                }
                IDC_BTN_LOAD_ELF => {
                    if let Some(app) = get_app_mut() {
                        app.load_elf_dialog();
                    }
                }
                IDM_OPEN_ELF => {
                    // 菜单/快捷键 Ctrl+O：打开 ELF 文件
                    if let Some(app) = get_app_mut() {
                        app.load_elf_dialog();
                    }
                }
                IDM_EXIT => {
                    let _ = DestroyWindow(hwnd);
                }
                IDM_ABOUT => {
                    show_about(hwnd);
                }
                IDM_WIN_CHART => {
                    if let Some(app) = get_app_mut() {
                        let _ = app.add_window(WinKind::Chart);
                    }
                }
                IDM_WIN_NUM => {
                    if let Some(app) = get_app_mut() {
                        let _ = app.add_window(WinKind::Number);
                    }
                }
                IDM_CTX_ADD_CHART | IDM_CTX_ADD_NUM => {
                    // 右键菜单：把勾选的变量加到目标窗口
                    let kind = if id == IDM_CTX_ADD_CHART {
                        WinKind::Chart
                    } else {
                        WinKind::Number
                    };
                    if let Some(app) = get_app_mut() {
                        app.ctx_add_checked_to(kind);
                    }
                }
                IDM_CTX_CHECK_ALL | IDM_CTX_UNCHECK_ALL => {
                    let check = id == IDM_CTX_CHECK_ALL;
                    if let Some(app) = get_app_mut() {
                        app.ctx_check_all(check);
                    }
                }
                _ => {}
            }
            LRESULT(0)
        }
        WM_CONTEXTMENU => {
            // 变量树右键菜单
            let htree = (lparam.0 as usize == 0xffff_ffff) as bool;
            let _ = htree;
            if let Some(app) = get_app_mut() {
                app.show_tree_context_menu(hwnd, wparam, lparam);
            }
            LRESULT(0)
        }
        WM_SIZE => {
            let w = (lparam.0 & 0xffff) as i32;
            let h = ((lparam.0 >> 16) & 0xffff) as i32;
            if let Some(app) = get_app_mut() {
                app.layout(w, h);
            }
            LRESULT(0)
        }
        WM_NOTIFY => {
            // 变量树复选框切换 → 采集列表增删
            let p = lparam.0 as *const NMHDR;
            if !p.is_null() {
                let hdr = &*p;
                if hdr.idFrom == IDC_TREE as usize && hdr.code == TVN_ITEMCHANGEDW {
                    let nm = &*(p as *const NMTVITEMCHANGE);
                    let idx = nm.lParam.0 - 1;
                    let checked = (nm.uStateNew & 0xf000) > (nm.uStateOld & 0xf000);
                    if idx >= 0 {
                        if let Some(app) = get_app_mut() {
                            app.toggle_leaf(idx as usize, checked);
                        }
                    }
                } else if hdr.idFrom == IDC_TAB as usize && hdr.code == TCN_SELCHANGE {
                    // 切换 tab → 切换激活窗口
                    if let Some(app) = get_app_mut() {
                        let sel = winmgr::current_tab(app.htab);
                        winmgr::switch_win(sel);
                    }
                }
            }
            LRESULT(0)
        }
        WM_TIMER => {
            if wparam.0 == TIMER_UI_REFRESH {
                if let Some(app) = get_app_mut() {
                    // 刷新所有波形/数值窗口
                    for w_ in &app.wins {
                        let _ = InvalidateRect(Some(w_.hwnd), None, false);
                    }
                }
            } else if wparam.0 == 777 {
                // 诊断定时器：首次触发即发起连接（验证 wndproc 内 DLL 调用）
                let _ = KillTimer(Some(hwnd), 777);
                if let Some(app) = get_app_mut() {
                    app.log("[timer] 定时器触发，发起连接...");
                    match app.connect() {
                        Ok(_) => app.log("连接成功"),
                        Err(e) => app.log(&format!("连接失败: {}", e)),
                    }
                }
            }
            LRESULT(0)
        }
        WM_GETMINMAXINFO => LRESULT(0),
        WM_DESTROY => {
            if let Some(app) = get_app_mut() {
                // 关窗即断开：确保 JLINKARM_Close 被调用，避免 DLL 会话残留导致下次连接挂起
                app.stop_acq();
                if let Some(j) = &app.jlink {
                    j.disconnect();
                }
            }
            PostQuitMessage(0);
            LRESULT(0)
        }
        _ => DefWindowProcW(hwnd, msg, wparam, lparam),
    }
}

// App 由 main 中 Box::into_raw 常驻进程生命；WndProc 仅在主线程访问，指针恒定有效。
static mut APP: *mut App = std::ptr::null_mut();
pub fn set_app(app: *mut App) {
    unsafe { APP = app; }
}
pub(crate) fn get_app_mut() -> Option<&'static mut App> {
    let p = unsafe { APP };
    if p.is_null() {
        None
    } else {
        Some(unsafe { &mut *p })
    }
}

/// “关于 OpenScope”对话框（内容复刻 C 版 mainwin.c:2661）。
unsafe fn show_about(owner: HWND) {
    let text = format!(
        "OpenScope v{}\n\nMCU 变量采集与标定工具（类 CANape）\nRust + Win32 重写\n\n晶圆上的生物技术开发和提供支持\n网址: www.opendebugger.com",
        VERSION
    );
    let t: Vec<u16> = text.encode_utf16().chain(std::iter::once(0)).collect();
    let c: Vec<u16> = "关于".encode_utf16().chain(std::iter::once(0)).collect();
    let _ = MessageBoxW(
        Some(owner),
        PCWSTR(t.as_ptr()),
        PCWSTR(c.as_ptr()),
        MB_OK | MB_ICONINFORMATION,
    );
}

/// 日志写入（简版：先打印到控制台，UI 日志栏后续接 ListView）。
pub fn os_log(_hwnd: HWND, msg: &str) {
    let mut handle = std::io::stderr();
    use std::io::Write;
    let _ = writeln!(handle, "[OpenScope] {}", msg);
}
