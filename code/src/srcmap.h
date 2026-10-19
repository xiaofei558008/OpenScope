/*
 * ELF ↔ 源码目录关联：编译路径映射到本机目录（供源码窗迭代2使用）。
 */
#ifndef OS_SRCMAP_H
#define OS_SRCMAP_H

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OS_SRCMAP_MAX_ROOTS 8
#define OS_SRCMAP_MAX_MAPS  4

typedef struct OS_SrcMap {
    wchar_t roots[OS_SRCMAP_MAX_ROOTS][MAX_PATH];
    int     root_count;
    wchar_t from_prefix[OS_SRCMAP_MAX_MAPS][MAX_PATH]; /* 编译机路径前缀 */
    wchar_t to_prefix[OS_SRCMAP_MAX_MAPS][MAX_PATH];   /* 本机路径前缀 */
    int     map_count;
    wchar_t elf_path[MAX_PATH]; /* 绑定的 ELF（空=全局默认） */
} OS_SrcMap;

void os_srcmap_init(OS_SrcMap* m);
/* 按当前 ELF 路径加载/保存（%LOCALAPPDATA%\OpenScope\srcmap_*.ini） */
void os_srcmap_load_for_elf(OS_SrcMap* m, const wchar_t* elf_path);
void os_srcmap_save(const OS_SrcMap* m);

/* 将编译单元中的路径解析为本机绝对路径；成功返回 1 */
int  os_srcmap_resolve(const OS_SrcMap* m, const char* compile_path_utf8,
                       wchar_t* out_local, int out_cap);

/* 模态配置对话框；改动写回 *m 并 save */
void os_srcmap_dialog(HWND owner, OS_SrcMap* m);

#ifdef __cplusplus
}
#endif

#endif
