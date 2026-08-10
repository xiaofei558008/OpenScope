//! OpenScope J-Link 驱动（Rust 重写）
//!
//! 对应 C 版 module/jlink/jlink.c：运行时动态加载 JLink_x64.dll（LoadLibrary +
//! GetProcAddress），扫描/连接/读写 MCU 内存。连接序列遵守 AD-JLINK（EMU 选择在
//! open 前、Device 在 open 后、SuppressInfoDialogs=1）。

pub mod dll;

/// 连接配置（对应 C 版 OS_ConnectCfg）。
#[derive(Debug, Clone, Default)]
pub struct ConnectCfg {
    pub device: String,     // 核心名，如 "Cortex-M4"
    pub iface_jtag: bool,   // true=JTAG, false=SWD
    pub speed_khz: u32,     // 0=自动
    pub probe_index: i32,   // >=0 显式选仿真器
    pub serial: String,     // 显式序列号（空=用 index）
}

/// 驱动信息（对应 C 版 OS_DriverInfo）。
#[derive(Debug, Clone, Default)]
pub struct DriverInfo {
    pub version: String,
    pub dll_version: String,
    pub hw_version: u32,
    pub emulator: String,
    pub connected: bool,
}

/// 内存读请求（对应 C 版 OS_MemReq）。
#[derive(Debug, Clone)]
pub struct MemReq {
    pub address: u64,
    pub size: u32,
    pub data: Vec<u8>,
}

impl MemReq {
    pub fn new(address: u64, size: u32) -> Self {
        MemReq {
            address,
            size,
            data: vec![0u8; size as usize],
        }
    }
}

pub use dll::Jlink;
