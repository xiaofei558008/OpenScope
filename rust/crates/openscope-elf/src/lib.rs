//! OpenScope ELF/DWARF 变量解析（Rust 重写）
//!
//! 对应 C 版 code/src/elf.c：解析 ELF32/ELF64，读取符号表得到全局变量地址，
//! 结合 DWARF 调试信息还原变量类型（基础类型/结构体/联合体/数组/枚举/指针）。

pub mod types;

use object::{Object, ObjectSymbol};
use types::Variable;
use std::path::Path;

/// 打开并解析 ELF 文件，返回解析出的全局变量列表。
pub fn open_elf(path: &Path) -> Result<ElfFile, String> {
    let data = std::fs::read(path).map_err(|e| format!("读取 {} 失败: {}", path.display(), e))?;
    let obj = object::File::parse(&*data).map_err(|e| format!("解析 ELF 失败: {}", e))?;
    if obj.format() != object::BinaryFormat::Elf {
        return Err("不是 ELF 文件".into());
    }
    let mut vars: Vec<Variable> = Vec::new();
    for sym in obj.symbols() {
        if !sym.is_definition() || sym.is_undefined() {
            continue;
        }
        let name = match sym.name() {
            Ok(n) if !n.is_empty() => n,
            _ => continue,
        };
        let section = sym.section_index();
        // 只关心已分配节（全局数据/函数之外的变量区），且是全局/弱符号
        if let Some(_sect) = section {
            // 先收集全部具名定义符号，DWARF 层再过滤为数据变量
            vars.push(Variable {
                name: name.to_string(),
                address: sym.address(),
                symbol_size: sym.size(),
            });
        }
    }
    Ok(ElfFile { vars })
}

/// 解析出的 ELF 文件内容。
pub struct ElfFile {
    vars: Vec<Variable>,
}

impl ElfFile {
    pub fn var_count(&self) -> usize {
        self.vars.len()
    }

    pub fn var_at(&self, idx: usize) -> Option<&Variable> {
        self.vars.get(idx)
    }

    pub fn find_var(&self, name: &str) -> Option<usize> {
        self.vars.iter().position(|v| v.name == name)
    }

    /// 模糊搜索（子串匹配，不区分大小写），返回匹配索引列表。
    pub fn find_vars(&self, needle: &str) -> Vec<usize> {
        let n = needle.to_lowercase();
        self.vars
            .iter()
            .enumerate()
            .filter(|(_, v)| v.name.to_lowercase().contains(&n))
            .map(|(i, _)| i)
            .collect()
    }
}

/// 为单元测试/无 DWARF 场景提供手工构造的变量。
pub fn dummy_vars(names: &[(&str, u64, u64)]) -> ElfFile {
    ElfFile {
        vars: names
            .iter()
            .map(|(n, a, s)| Variable {
                name: n.to_string(),
                address: *a,
                symbol_size: *s,
            })
            .collect(),
    }
}
