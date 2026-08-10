//! JLink_x64.dll 动态绑定（对应 C 版 jlink.c 的 os_jlink_bind）。
//!
//! 用 libloading 动态加载 JLink_x64.dll 并 GetProcAddress 绑定符号，与 C 版一致：
//! 不依赖导入库，运行时从模块同目录加载。

use libloading::Library;
use std::path::Path;
use std::sync::Mutex;

use crate::{ConnectCfg, DriverInfo, MemReq};

/// 仿真器枚举信息（对齐 SEGGER JLINKARM_EMU_CONNECT_INFO，264 字节，见 C 版 OS_JLinkEmuInfo）。
#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct ConnectInfo {
    pub serial_number: u32,
    pub connection: u8,
    pub s_usb_addr: [u8; 7],
    pub s_ip_addr: [i8; 16],
    pub time: u32,
    pub time_us: u64,
    pub hw_version: u32,
    pub ab_mac_addr: [u8; 6],
    pub ac_product: [i8; 32],
    pub ac_nickname: [i8; 32],
    pub ac_fw_string: [i8; 112],
    pub is_dhcp_ip: u8,
    pub is_dhcp_ip_valid: u8,
    pub num_ip_conns: u8,
    pub num_ip_conns_valid: u8,
    pub a_padding: [u8; 34],
}

impl Default for ConnectInfo {
    fn default() -> Self {
        unsafe { std::mem::zeroed() }
    }
}

/// 把 DLL 返回的 i8 缓冲区裁剪成字符串（首个 NUL 截断）。
pub fn cstr_trim(buf: &[i8]) -> String {
    let bytes: Vec<u8> = buf
        .iter()
        .take_while(|c| **c != 0)
        .map(|c| *c as u8)
        .collect();
    String::from_utf8_lossy(&bytes).into_owned()
}

impl ConnectInfo {
    pub const SIZE: usize = 264;

    fn cstr(&self, arr: &[i8]) -> String {
        let bytes: Vec<u8> = arr
            .iter()
            .take_while(|c| **c != 0)
            .map(|c| *c as u8)
            .collect();
        String::from_utf8_lossy(&bytes).into_owned()
    }

    pub fn product(&self) -> String {
        self.cstr(&self.ac_product)
    }
    pub fn nickname(&self) -> String {
        self.cstr(&self.ac_nickname)
    }
    pub fn fw_string(&self) -> String {
        self.cstr(&self.ac_fw_string)
    }
}

/// J-Link 驱动句柄。函数指针按需懒加载；全部调用需外部互斥（采集线程/主线程不并发进 DLL）。
pub struct Jlink {
    lib: Library,
    open: unsafe extern "C" fn(*const i8) -> i32,
    close: unsafe extern "C" fn(),
    is_connected: unsafe extern "C" fn() -> i32,
    exec_cmd: unsafe extern "C" fn(*const i8, *mut i8, i32) -> i32,
    connect: unsafe extern "C" fn() -> i32,
    tif_select: unsafe extern "C" fn(i32) -> i32,
    set_speed: unsafe extern "C" fn(i32) -> i32,
    read_mem: unsafe extern "C" fn(u32, u32, *mut u8) -> i32,
    write_mem: unsafe extern "C" fn(u32, u32, *const u8) -> i32,
    get_dll_version: unsafe extern "C" fn() -> u32,
    get_hw_version: unsafe extern "C" fn() -> u32,
    get_fw_string: unsafe extern "C" fn(*mut i8, i32) -> i32,
    emu_get_list: unsafe extern "C" fn(i32, *mut ConnectInfo, i32) -> i32,
    emu_select_by_index: unsafe extern "C" fn(i32) -> i32,
    emu_select_by_usbsn: unsafe extern "C" fn(u32) -> i32,
}

/// 全局连接状态锁：所有 DLL 调用经此互斥，防止采集线程与 UI 并发进 DLL。
pub static JLINK_LOCK: Mutex<()> = Mutex::new(());

impl Jlink {
    /// 加载 JLink_x64.dll（从 dir 目录）。失败返回 Err。
    pub fn load(dir: &Path) -> Result<Jlink, String> {
        let path = dir.join("JLink_x64.dll");
        let lib = unsafe { Library::new(&path) }
            .map_err(|e| format!("加载 JLink_x64.dll 失败: {} ({})", path.display(), e))?;
        macro_rules! b {
            ($name:literal, $t:ty) => {
                unsafe {
                    *lib.get::<$t>($name.as_bytes())
                        .map_err(|e| format!("缺少导出 {}: {}", $name, e))?
                }
            };
        }
        let j = Jlink {
            open: b!("JLINKARM_Open", unsafe extern "C" fn(*const i8) -> i32),
            close: b!("JLINKARM_Close", unsafe extern "C" fn()),
            is_connected: b!("JLINKARM_IsConnected", unsafe extern "C" fn() -> i32),
            exec_cmd: b!("JLINKARM_ExecCommand", unsafe extern "C" fn(*const i8, *mut i8, i32) -> i32),
            connect: b!("JLINKARM_Connect", unsafe extern "C" fn() -> i32),
            tif_select: b!("JLINKARM_TIF_Select", unsafe extern "C" fn(i32) -> i32),
            set_speed: b!("JLINKARM_SetSpeed", unsafe extern "C" fn(i32) -> i32),
            read_mem: b!("JLINKARM_ReadMem", unsafe extern "C" fn(u32, u32, *mut u8) -> i32),
            write_mem: b!("JLINKARM_WriteMem", unsafe extern "C" fn(u32, u32, *const u8) -> i32),
            get_dll_version: b!("JLINKARM_GetDLLVersion", unsafe extern "C" fn() -> u32),
            get_hw_version: b!("JLINKARM_GetHardwareVersion", unsafe extern "C" fn() -> u32),
            get_fw_string: b!("JLINKARM_GetFirmwareString", unsafe extern "C" fn(*mut i8, i32) -> i32),
            emu_get_list: b!("JLINKARM_EMU_GetList", unsafe extern "C" fn(i32, *mut ConnectInfo, i32) -> i32),
            emu_select_by_index: b!("JLINKARM_EMU_SelectByIndex", unsafe extern "C" fn(i32) -> i32),
            emu_select_by_usbsn: b!("JLINKARM_EMU_SelectByUSBSN", unsafe extern "C" fn(u32) -> i32),
            lib,
        };
        Ok(j)
    }

    /// 枚举 USB/IP 上的 J-Link 设备。
    pub fn scan(&self) -> Vec<ConnectInfo> {
        let mut infos = [ConnectInfo::default(); 8];
        let n = unsafe { (self.emu_get_list)(3, infos.as_mut_ptr(), 8) };
        if n <= 0 {
            return Vec::new();
        }
        infos.iter().take(n as usize).cloned().collect()
    }

    /// 连接（AD-JLINK 序列：EMU 选择在 open 前，Device 在 open 后）。
    pub fn connect(&self, cfg: &ConnectCfg, log: &mut dyn FnMut(&str)) -> Result<(), String> {
        let _guard = JLINK_LOCK.lock().unwrap_or_else(|e| e.into_inner());
        unsafe {
            // 1) 仿真器选择（必须在 open 前 —— checkpoint-27 Bug16 根因）
            if !cfg.serial.is_empty() {
                let sn = cfg.serial.parse::<u32>().unwrap_or(0);
                let rc = (self.emu_select_by_usbsn)(sn);
                log(&format!("EMU_SelectByUSBSN({}) rc={}", sn, rc));
            } else if cfg.probe_index >= 0 {
                let rc = (self.emu_select_by_index)(cfg.probe_index);
                log(&format!("EMU_SelectByIndex({}) rc={}", cfg.probe_index, rc));
            }
            // 2) open(NULL) —— 自动探测核心
            let rc = (self.open)(std::ptr::null());
            if rc != 0 {
                return Err(format!("JLINKARM_Open 失败 rc={}", rc));
            }
            // 3) 抑制信息弹窗（需求 21：不弹 "Target device setting"/"Device Selection"）
            let mut res = [0i8; 256];
            let rc = (self.exec_cmd)(c"SuppressInfoDialogs = 1".as_ptr(), res.as_mut_ptr(), res.len() as i32);
            log(&format!("SuppressInfoDialogs rc={}", rc));
            // 4) 接口
            let tif = if cfg.iface_jtag { 0 } else { 1 };
            let rc = (self.tif_select)(tif);
            log(&format!("TIF_Select({}) rc={}", if cfg.iface_jtag { "JTAG" } else { "SWD" }, rc));
            // 5) Device 必须在 open 后（checkpoint-26：open 前设会破坏块读）。
            //    注意：实测 "CORTEX-M4T"/"CORTEX-M4"/"STM32L031K6" 等名称 DLL 均不认识
            //    （rc=-1 "Failed to set device"），只有 "Cortex-M4" 可用。设备名设置失败后
            //    **不得重试** Device 命令——DLL 会话状态会损坏导致整调用挂起（同 Bug16 一类）。
            let dev = if cfg.device.is_empty() { "Cortex-M4" } else { cfg.device.as_str() };
            // 关键：exec_cmd 期望 NUL 结尾的 C 字符串。format! 生成的 String 不保证尾部 NUL，
            // as_ptr() 直接传给 DLL 时读到堆里随机的下一个字节 → 设备名被截断/污染，
            // Device rc 随机为 -1（"Failed to set device"）。必须显式补 NUL。
            let cmd = format!("Device = {}\0", dev);
            let mut res = [0i8; 256];
            let rc = (self.exec_cmd)(cmd.as_ptr() as *const i8, res.as_mut_ptr(), res.len() as i32);
            let resp = cstr_trim(&res);
            log(&format!(
                "'{}' rc={} -> {}",
                cmd.trim_end_matches('\0'),
                rc,
                if resp.is_empty() { "(空)" } else { &resp }
            ));
            // 设备名设置失败时绝不能继续到 JLINKARM_Connect：DLL 会弹 "Device Selection"
            // 设备选择框并挂起等待用户输入（需求：不要有 Device Selection 弹窗）。
            if rc != 0 {
                (self.close)();
                return Err(format!("Device = {} 失败 rc={}: {}", dev, rc, &resp));
            }
            // 6) 速度
            if cfg.speed_khz > 0 {
                let rc = (self.set_speed)(cfg.speed_khz as i32);
                log(&format!("SetSpeed({}) rc={}", cfg.speed_khz, rc));
            }
            // 7) 连接
            let rc = if (self.is_connected)() != 0 { 0 } else { (self.connect)() };
            log(&format!("Connect rc={}", rc));
            if rc != 0 {
                (self.close)();
                return Err(format!("JLINKARM_Connect 失败 rc={}", rc));
            }
        }
        Ok(())
    }

    pub fn disconnect(&self) {
        let _guard = JLINK_LOCK.lock().unwrap_or_else(|e| e.into_inner());
        unsafe { (self.close)() }
    }

    pub fn is_connected(&self) -> bool {
        let _guard = JLINK_LOCK.lock().unwrap_or_else(|e| e.into_inner());
        unsafe { (self.is_connected)() != 0 }
    }

    /// 读内存；返回读取到的字节数（成功=size）。块读失败返回 Err(未读字节数/负错误码)。
    pub fn read_mem(&self, req: &mut MemReq) -> Result<(), i32> {
        let _guard = JLINK_LOCK.lock().unwrap_or_else(|e| e.into_inner());
        unsafe {
            let r = (self.read_mem)(req.address as u32, req.size, req.data.as_mut_ptr());
            if r == 0 {
                Ok(())
            } else {
                Err(r)
            }
        }
    }

    pub fn write_mem(&self, req: &MemReq) -> Result<(), i32> {
        let _guard = JLINK_LOCK.lock().unwrap_or_else(|e| e.into_inner());
        unsafe {
            let r = (self.write_mem)(req.address as u32, req.size, req.data.as_ptr());
            if r == 0 {
                Ok(())
            } else {
                Err(r)
            }
        }
    }

    /// 调试：直接调用 EMU_SelectByIndex。
    pub fn emu_select_by_index_debug(&self, idx: i32) -> i32 {
        let _guard = JLINK_LOCK.lock().unwrap_or_else(|e| e.into_inner());
        unsafe { (self.emu_select_by_index)(idx) }
    }

    /// 调试：直接调用 EMU_SelectByUSBSN。
    pub fn emu_select_by_usbsn_debug(&self, sn: u32) -> i32 {
        let _guard = JLINK_LOCK.lock().unwrap_or_else(|e| e.into_inner());
        unsafe { (self.emu_select_by_usbsn)(sn) }
    }

    pub fn info(&self) -> DriverInfo {
        let _guard = JLINK_LOCK.lock().unwrap_or_else(|e| e.into_inner());
        let mut d = DriverInfo::default();
        d.version = "2.0.0".into();
        d.dll_version = unsafe { (self.get_dll_version)() }.to_string();
        d.hw_version = unsafe { (self.get_hw_version)() };
        if unsafe { (self.is_connected)() } != 0 {
            let mut buf = [0i8; 128];
            unsafe { (self.get_fw_string)(buf.as_mut_ptr(), buf.len() as i32) };
            let s: Vec<u8> = buf.iter().take_while(|c| **c != 0).map(|c| *c as u8).collect();
            d.emulator = String::from_utf8_lossy(&s).into_owned();
        }
        d
    }
}
