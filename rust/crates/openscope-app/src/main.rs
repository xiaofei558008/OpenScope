//! OpenScope 2.x 主程序（Rust 重写入口）
//!
//! 原生 Win32 窗口 + 消息循环。对应 C 版 main.c。

#![windows_subsystem = "windows"]

mod acq;
mod app;
mod chart;
mod ffi;

use windows::core::*;
use windows::Win32::Foundation::*;
use windows::Win32::Graphics::Gdi::*;
use windows::Win32::System::LibraryLoader::*;
use windows::Win32::UI::Controls::*;
use windows::Win32::UI::WindowsAndMessaging::*;

use app::{App, set_app};

fn main() -> Result<()> {
    unsafe {
        let hinst = GetModuleHandleA(None)?;

        // 初始化公共控件（TreeView/ListView/状态栏等）
        let icc = INITCOMMONCONTROLSEX {
            dwSize: std::mem::size_of::<INITCOMMONCONTROLSEX>() as u32,
            dwICC: ICC_TREEVIEW_CLASSES | ICC_LISTVIEW_CLASSES | ICC_BAR_CLASSES,
        };
        let _ = InitCommonControlsEx(&icc);

        // App 生命周期 = 整个进程：Box::into_raw 后指针常驻
        let app = Box::new(App::new(HINSTANCE(hinst.0)));
        let ptr: *mut App = Box::into_raw(app);
        set_app(ptr);

        let app = &mut *ptr;
        // 尽早加载 J-Link DLL（与 C 版一致：模块初始化时加载，避免 wndproc 内首 LoadLibrary）
        if let Err(e) = app.load_jlink() {
            eprintln!("[OpenScope] JLink DLL 加载失败: {}", e);
        }
        let hwnd = app.create_main_window()?;
        if hwnd.0.is_null() {
            return Err(Error::from_win32());
        }
        // CLI 参数：可预加载 ELF 便于自动化测试
        // --autoconnect: 消息循环前直接连接（诊断“wndproc 内阻塞 DLL”死锁）
        let mut selftest = false;
        let mut args = std::env::args();
        args.next();
        while let Some(a) = args.next() {
            if a == "--autoconnect" {
                match app.connect() {
                    Ok(v) => app.log(&format!("[autoconnect] 成功 {}", v)),
                    Err(e) => app.log(&format!("[autoconnect] 失败: {}", e)),
                }
            } else if a == "--selftest" {
                selftest = true;
            } else if a == "--autoselect" || a == "--autoconnect-timer" {
                // 标志位，稍后处理
            } else if a.starts_with('-') {
                // 其它未知开关：忽略，不当作 ELF 路径
            } else {
                app.load_elf_path(&a);
            }
        }

        // 自动演示模式：连接→登记前 N 个变量→启动采集→进入正常消息循环（图表窗口实时渲染）
        // 与 --selftest 的区别：不 exit，走正常 GUI 流程，供截图验证 R1.6 真实曲线。
        if std::env::args().any(|a| a == "--autoselect") {
            // 可选：OPENSC_AUTOSELECT_IDX="615,620,285,612" 指定 ELF 符号索引；
            // 否则默认前 N 个（OPENSC_AUTOSELECT_N）。
            let nauto = std::env::var("OPENSC_AUTOSELECT_N")
                .ok()
                .and_then(|s| s.parse::<usize>().ok())
                .unwrap_or(4);
            let idxs: Vec<usize> = std::env::var("OPENSC_AUTOSELECT_IDX")
                .ok()
                .filter(|s| !s.is_empty())
                .map(|s| {
                    s.split(',')
                        .filter_map(|p| p.trim().parse::<usize>().ok())
                        .collect()
                })
                .unwrap_or_else(|| (0..nauto).collect());
            match app.connect() {
                Ok(v) => app.log(&format!("[autoselect] 连接成功 {}", v)),
                Err(e) => {
                    app.log(&format!("[autoselect] 连接失败: {}", e));
                }
            }
            for i in &idxs {
                app.toggle_leaf(*i, true);
            }
            app.start_acq();
            app.log(&format!("[autoselect] 已登记 {} 个变量并启动采集，进入消息循环", idxs.len()));
        }

        // 自测模式：连接→登记前 N 个变量→采集→报告环形缓冲样本数→安全退出
        // （绕过 Win32 消息封装，直接验证 R1.5/R1.6 真实管线：DLL→read_mem→环形缓冲）
        if selftest {
            let ntest = std::env::var("OPENSC_SELFTEST_N")
                .ok()
                .and_then(|s| s.parse::<usize>().ok())
                .unwrap_or(3);
            match app.connect() {
                Ok(v) => app.log(&format!("[selftest] 连接成功 {}", v)),
                Err(e) => {
                    app.log(&format!("[selftest] 连接失败: {}", e));
                    std::process::exit(1);
                }
            }
            for i in 0..ntest {
                app.toggle_leaf(i, true);
            }
            app.start_acq();
            // 采集 3 秒（读内存），随后报告每个叶子的样本数
            std::thread::sleep(std::time::Duration::from_millis(3000));
            let (names, counts, values) = {
                let acq = app.acq.lock().unwrap_or_else(|e| e.into_inner());
                (
                    acq.leaves.iter().map(|l| l.name.clone()).collect::<Vec<_>>(),
                    acq.leaves.iter().map(|l| l.count).collect::<Vec<_>>(),
                    acq.leaves
                        .iter()
                        .map(|l| {
                            if l.count > 0 {
                                Some(l.samples[(l.head + 7) % crate::acq::RING_CAP])
                            } else {
                                None
                            }
                        })
                        .collect::<Vec<_>>(),
                )
            };
            for i in 0..ntest {
                let v = values
                    .get(i)
                    .and_then(|v| *v)
                    .map(|v| format!("{:.4}", v))
                    .unwrap_or_else(|| "N/A".into());
                app.log(&format!(
                    "[selftest] var[{}] {} 样本={} 最新值={}",
                    i,
                    names.get(i).cloned().unwrap_or_default(),
                    counts.get(i).copied().unwrap_or(0),
                    v
                ));
            }
            app.stop_acq();
            if let Some(j) = &app.jlink {
                j.disconnect();
            }
            app.log("[selftest] 完成，安全退出");
            std::process::exit(0);
        }

        let _ = ShowWindow(hwnd, SW_SHOW);
        let _ = UpdateWindow(hwnd);

        // 诊断：消息循环运行后，从定时器消息里发起连接（验证“wndproc 内调用”是否导致 DLL 死锁）
        if std::env::args().any(|a| a == "--autoconnect-timer") {
            let _ = SetTimer(Some(hwnd), 777, 500, None);
        }

        let mut msg = MSG::default();
        while GetMessageW(&mut msg, None, 0, 0).as_bool() {
            let _ = TranslateMessage(&msg);
            let _ = DispatchMessageW(&msg);
        }
        Ok(())
    }
}
