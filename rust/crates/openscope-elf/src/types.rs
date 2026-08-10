//! ELF 变量/类型数据模型（对应 C 版 elf.h 的 OS_Variable/OS_Type/OS_TypeKind）。

/// 类型种类（对应 C 版 OS_TypeKind）。
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum TypeKind {
    Void,
    Int,
    Uint,
    Float,
    Bool,
    Struct,
    Union,
    Array,
    Enum,
    Ptr,
    String,
    Other,
}

impl TypeKind {
    pub fn as_str(&self) -> &'static str {
        match self {
            TypeKind::Void => "void",
            TypeKind::Int => "int",
            TypeKind::Uint => "uint",
            TypeKind::Float => "float",
            TypeKind::Bool => "bool",
            TypeKind::Struct => "struct",
            TypeKind::Union => "union",
            TypeKind::Array => "array",
            TypeKind::Enum => "enum",
            TypeKind::Ptr => "ptr",
            TypeKind::String => "string",
            TypeKind::Other => "other",
        }
    }
}

/// 类型节点（对应 C 版 OS_Type）。子节点（结构体成员/数组元素/枚举值）经 children 组织。
#[derive(Debug, Clone)]
pub struct Type {
    pub name: String,
    pub kind: TypeKind,
    pub size: u32,          // 字节数，未知为 0
    pub is_signed: bool,
    pub is_ptr: bool,
    pub is_bitfield: bool,
    pub bit_offset: u8,     // 位域：存储单元内 LSB 起始位
    pub bit_size: u8,       // 位域位宽
    pub member_offset: i64, // 结构体成员相对首地址字节偏移
    pub enum_value: i64,    // 枚举子节点取值
    pub array_count: i32,   // 数组元素个数
    pub children: Vec<Type>,
}

impl Default for Type {
    fn default() -> Self {
        Type {
            name: String::new(),
            kind: TypeKind::Other,
            size: 0,
            is_signed: false,
            is_ptr: false,
            is_bitfield: false,
            bit_offset: 0,
            bit_size: 0,
            member_offset: 0,
            enum_value: 0,
            array_count: 0,
            children: Vec::new(),
        }
    }
}

/// 顶层全局变量（对应 C 版 OS_Variable）。
#[derive(Debug, Clone)]
pub struct Variable {
    pub name: String,
    pub address: u64,
    pub symbol_size: u64,
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn type_kind_str() {
        assert_eq!(TypeKind::Uint.as_str(), "uint");
        assert_eq!(TypeKind::Struct.as_str(), "struct");
    }

    #[test]
    fn default_type() {
        let t = Type::default();
        assert_eq!(t.kind, TypeKind::Other);
        assert_eq!(t.size, 0);
    }
}
