//! 复现 app 的 scan()→connect() 顺序：先枚举再按序列号连接。
use openscope_jlink::{ConnectCfg, Jlink};

fn main() {
    let dir = "target/debug";
    let j = match Jlink::load(std::path::Path::new(dir)) {
        Ok(j) => j,
        Err(e) => { eprintln!("加载失败: {}", e); std::process::exit(1); }
    };
    eprintln!("[t] scan()...");
    let probes = j.scan();
    eprintln!("[t] scan returned {} probes", probes.len());
    for p in &probes {
        eprintln!("[t] probe SN={} prod={}", p.serial_number, p.product());
    }
    let cfg = ConnectCfg {
        device: "Cortex-M4".into(),
        iface_jtag: false,
        speed_khz: 4000,
        probe_index: -1,
        serial: if let Some(first) = probes.first() {
            first.serial_number.to_string()
        } else {
            "174504925".into()
        },
    };
    eprintln!("[t] connect()...");
    let mut logs: Vec<String> = Vec::new();
    let rc = j.connect(&cfg, &mut |m| logs.push(m.to_string()));
    for l in &logs { eprintln!("[t] {}", l); }
    match rc {
        Ok(_) => {
            eprintln!("[t] CONNECT OK");
            // 退出前必须 Close，否则进程在连接状态退出会卡死仿真器会话（见 device_probe 注释）。
            j.disconnect();
            eprintln!("[t] CLOSED OK");
        }
        Err(e) => eprintln!("[t] CONNECT FAIL: {}", e),
    }
}
