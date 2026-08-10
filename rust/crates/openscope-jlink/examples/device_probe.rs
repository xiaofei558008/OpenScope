//! 探测 J-Link 设备名识别：`cargo run -p openscope-jlink --example device_probe -- <dll目录> <设备名> [serial]`
use openscope_jlink::{ConnectCfg, Jlink};

fn main() {
    let dir = std::env::args().nth(1).unwrap_or_else(|| "target/debug".into());
    let device = std::env::args().nth(2).unwrap_or_else(|| "Cortex-M4".into());
    let serial = std::env::args().nth(3).unwrap_or_else(|| "174504925".into());

    let j = match Jlink::load(std::path::Path::new(&dir)) {
        Ok(j) => j,
        Err(e) => {
            eprintln!("加载失败: {}", e);
            std::process::exit(1);
        }
    };
    let cfg = ConnectCfg {
        device: device.clone(),
        iface_jtag: false,
        speed_khz: 4000,
        probe_index: -1,
        serial: serial.clone(),
    };
    let rc = j.connect(&cfg, &mut |m| eprintln!("[d] {}", m));
    match rc {
        Ok(_) => {
            println!("CONNECT OK");
            // 关键：退出前必须 JLINKARM_Close —— 进程在连接状态下直接退出会卡死仿真器
            // 会话（等价 TerminateProcess），导致后续任何连接在 Device 步骤挂起。
            j.disconnect();
            println!("CLOSED OK");
        }
        Err(e) => println!("CONNECT FAIL: {}", e),
    }
}
