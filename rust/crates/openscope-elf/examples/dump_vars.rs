//! 打印 ELF 符号表前 N 个符号名（用于挑选演示变量）
use openscope_elf::open_elf;
fn main() {
    let path = std::env::args().nth(1).unwrap_or_else(|| "tests/linix_stm32l031_v1.2.out".into());
    let elf = match open_elf(std::path::Path::new(&path)) {
        Ok(e) => e,
        Err(e) => { eprintln!("失败: {}", e); std::process::exit(1); }
    };
    let n = elf.var_count();
    println!("共 {} 个符号", n);
    for i in 0..n {
        if let Some(v) = elf.var_at(i) {
            println!("[{:4}] {}  @0x{:08X} [{}B]", i, v.name, v.address, v.symbol_size);
        }
    }
}
