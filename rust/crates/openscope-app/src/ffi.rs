//! 手动 FFI 声明（windows-rs 未包裹的 comdlg32 公共对话框）。

#![allow(non_snake_case)]
#![allow(dead_code)]

use std::ffi::c_void;
use windows::Win32::Foundation::HWND;

/// 与 comdlg32 的 OPENFILENAMEW 布局一致（x64，lStructSize=152）。
#[repr(C)]
#[derive(Clone)]
pub struct OPENFILENAMEW {
    pub lStructSize: u32,
    pub hwndOwner: HWND,
    pub hInstance: windows::Win32::Foundation::HINSTANCE,
    pub lpstrFilter: *const u16,
    pub lpstrCustomFilter: *mut u16,
    pub nMaxCustFilter: u32,
    pub nFilterIndex: u32,
    pub lpstrFile: *mut u16,
    pub nMaxFile: u32,
    pub lpstrFileTitle: *mut u16,
    pub nMaxFileTitle: u32,
    pub lpstrInitialDir: *const u16,
    pub lpstrTitle: *const u16,
    pub Flags: u32,
    pub nFileOffset: u16,
    pub nFileExtension: u16,
    pub lpstrDefExt: *const u16,
    pub lCustData: isize,
    pub lpfnHook: *const c_void,
    pub lpTemplateName: *const c_void,
    // Vista+
    pub lpEditInfo: *mut c_void,
    pub lpstrPrompt: *const u16,
    pub pvReserved: *mut c_void,
    pub dwReserved: u32,
    pub FlagsEx: u32,
}

impl Default for OPENFILENAMEW {
    fn default() -> Self {
        unsafe { std::mem::zeroed() }
    }
}

#[link(name = "comdlg32")]
unsafe extern "system" {
    fn GetOpenFileNameW(lpofn: *mut OPENFILENAMEW) -> i32;
}

pub const OFN_FILEMUSTEXIST: u32 = 0x0000_1000;
pub const OFN_PATHMUSTEXIST: u32 = 0x0000_0800;

/// 弹出“打开 ELF 文件”对话框。选中返回完整路径（UTF-16），取消返回 None。
pub fn open_elf_dialog(owner: HWND) -> Option<String> {
    unsafe {
        let mut buf = vec![0u16; 4096];
        let filter: Vec<u16> = "ELF 文件\0*.elf;*.axf;*.out\0所有文件\0*.*\0"
            .encode_utf16()
            .collect();
        let mut ofn = OPENFILENAMEW::default();
        ofn.lStructSize = std::mem::size_of::<OPENFILENAMEW>() as u32;
        ofn.hwndOwner = owner;
        ofn.lpstrFilter = filter.as_ptr();
        ofn.lpstrFile = buf.as_mut_ptr();
        ofn.nMaxFile = buf.len() as u32;
        ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
        let rc = GetOpenFileNameW(&mut ofn);
        if rc == 0 {
            return None;
        }
        let mut len = 0;
        while len < buf.len() && buf[len] != 0 {
            len += 1;
        }
        Some(String::from_utf16_lossy(&buf[..len]))
    }
}
