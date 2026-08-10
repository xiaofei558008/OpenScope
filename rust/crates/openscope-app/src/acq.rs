//! 采集线程 + 环形缓冲（对应 C 版 acq.c / datasrv.c）。
//!
//! 设计：主线程维护 AcqState（叶变量 + 各自环形缓冲）。"开始"后派生采集线程，
//! 线程持有 AcqState 锁，对每个叶变量块读 J-Link 内存 → 解码 → 推入环形缓冲。
//! UI 用 WM_TIMER 节流刷新波形窗口（从同一 AcqState 读取最新样本绘制）。

use std::sync::{Arc, Mutex};

/// 每叶变量环形缓冲容量（对应 C 版 OS_RING_CAP=8192）。
pub const RING_CAP: usize = 8192;

/// 叶变量规格（由变量树勾选生成）。
#[derive(Clone, Debug)]
pub struct LeafSpec {
    pub name: String,
    pub address: u64,
    pub size: u32,
    pub is_signed: bool,
    pub is_float: bool,
}

impl LeafSpec {
    pub fn new(name: &str, address: u64, size: u32) -> Self {
        LeafSpec {
            name: name.to_string(),
            address,
            size,
            is_signed: true,
            is_float: false,
        }
    }
}

/// 单叶变量的环形缓冲（容量固定，满则覆盖最旧样本）。
#[derive(Clone)]
pub struct LeafBuf {
    pub name: String,
    pub address: u64,
    pub size: u32,
    pub is_signed: bool,
    pub is_float: bool,
    pub samples: Vec<f32>,
    /// 下一写入下标（0..RING_CAP）。
    pub head: usize,
    /// 有效样本数（首次填充前 < RING_CAP）。
    pub count: usize,
    /// 最近一次采样值。
    pub latest: f32,
}

impl LeafBuf {
    fn new(spec: &LeafSpec) -> Self {
        LeafBuf {
            name: spec.name.clone(),
            address: spec.address,
            size: spec.size,
            is_signed: spec.is_signed,
            is_float: spec.is_float,
            samples: vec![0.0; RING_CAP],
            head: 0,
            count: 0,
            latest: 0.0,
        }
    }

    /// 由 ELF 符号创建（读块大小取符号大小，上限 8 字节；无类型信息时按 4 字节）。
    pub fn from_elf(name: String, address: u64, symbol_size: u32) -> Self {
        let mut size = symbol_size.max(1).min(8);
        if size == 0 {
            size = 4;
        }
        LeafBuf {
            name,
            address,
            size,
            is_signed: true,
            is_float: false,
            samples: vec![0.0; RING_CAP],
            head: 0,
            count: 0,
            latest: 0.0,
        }
    }

    fn push(&mut self, v: f32) {
        self.samples[self.head] = v;
        self.head = (self.head + 1) % RING_CAP;
        if self.count < RING_CAP {
            self.count += 1;
        }
        self.latest = v;
    }
}

/// 采集共享状态。
#[derive(Default)]
pub struct AcqState {
    pub leaves: Vec<LeafBuf>,
    pub running: bool,
    /// 采集轮数（用于估算采样率）。
    pub rounds: u64,
}

/// 启动采集：在 state 中登记叶子，派生采集线程。
/// 返回线程句柄。若已在运行返回 None。
pub fn start(
    jlink: Arc<openscope_jlink::Jlink>,
    state: Arc<Mutex<AcqState>>,
    specs: Vec<LeafSpec>,
) -> Option<std::thread::JoinHandle<()>> {
    {
        let mut s = state.lock().unwrap();
        if s.running {
            return None;
        }
        s.leaves.clear();
        for spec in &specs {
            s.leaves.push(LeafBuf::new(spec));
        }
        s.running = true;
        s.rounds = 0;
    }
    Some(std::thread::spawn(move || acq_loop(jlink, state)))
}

/// 停止采集：置 running=false 并等待线程退出。
pub fn stop(state: &Arc<Mutex<AcqState>>, handle: Option<std::thread::JoinHandle<()>>) {
    {
        let mut s = state.lock().unwrap();
        s.running = false;
    }
    if let Some(h) = handle {
        let _ = h.join();
    }
}

/// 采集线程主体：自由运行，对每个叶变量块读并解码。
fn acq_loop(jlink: Arc<openscope_jlink::Jlink>, state: Arc<Mutex<AcqState>>) {
    loop {
        let mut s = match state.lock() {
            Ok(s) => s,
            Err(e) => e.into_inner(),
        };
        if !s.running {
            break;
        }
        for leaf in s.leaves.iter_mut() {
            let mut req = openscope_jlink::MemReq::new(leaf.address, leaf.size);
            if let Ok(_) = jlink.read_mem(&mut req) {
                let v = decode_value(&req.data, leaf.is_signed, leaf.is_float);
                leaf.push(v);
            }
        }
        s.rounds += 1;
    }
}

/// 把读到的原始字节解码为 f32 显示值。
pub fn decode_value(data: &[u8], is_signed: bool, is_float: bool) -> f32 {
    if is_float && data.len() >= 4 {
        let mut b = [0u8; 4];
        b.copy_from_slice(&data[..4]);
        return f32::from_le_bytes(b);
    }
    match data.len() {
        1 => {
            let v = data[0];
            if is_signed {
                v as i8 as f32
            } else {
                v as f32
            }
        }
        2 => {
            let v = u16::from_le_bytes([data[0], data[1]]);
            if is_signed {
                v as i16 as f32
            } else {
                v as f32
            }
        }
        4 => {
            let v = u32::from_le_bytes([data[0], data[1], data[2], data[3]]);
            if is_signed {
                v as i32 as f32
            } else {
                v as f32
            }
        }
        8 => {
            let v = u64::from_le_bytes([
                data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
            ]);
            if is_signed {
                v as i64 as f32
            } else {
                v as f32
            }
        }
        _ => 0.0,
    }
}
