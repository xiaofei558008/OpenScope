//! 命令行扫描 J-Link 仿真器：`cargo run -p openscope-jlink --example scan -- <dll目录>`
use openscope_jlink::Jlink;

fn main() {
    let dir = std::env::args()
        .nth(1)
        .unwrap_or_else(|| "target/debug".into());
    let j = match Jlink::load(std::path::Path::new(&dir)) {
        Ok(j) => j,
        Err(e) => {
            eprintln!("加载失败: {}", e);
            std::process::exit(1);
        }
    };
    let infos = j.scan();
    println!("找到 {} 个 J-Link 仿真器:", infos.len());
    for (i, info) in infos.iter().enumerate() {
        println!(
            "[{}] SN={} product='{}' nick='{}' fw='{}' hw={}",
            i,
            info.serial_number,
            info.product(),
            info.nickname(),
            info.fw_string(),
            info.hw_version
        );
    }
    // 直接测试 EMU 选择 API 的返回值
    let rc = j.emu_select_by_index_debug(0);
    println!("EMU_SelectByIndex(0) rc={}", rc);
    let rc = j.emu_select_by_usbsn_debug(174504925);
    println!("EMU_SelectByUSBSN(174504925) rc={}", rc);
}
