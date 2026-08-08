#ifndef OS_LAYOUT_H
#define OS_LAYOUT_H

#include <windows.h>

/* 启动时从默认路径（%LOCALAPPDATA%\OpenScope\layout.ini，回退 exe 目录）恢复布局 */
void os_layout_restore_auto(void);
/* 退出时保存到默认路径 */
void os_layout_save_auto(void);
/* 另存/加载布局文件（分享用），成功返回 0 */
int  os_layout_save_to(const wchar_t* path);
int  os_layout_load_from(const wchar_t* path);
/* ELF 加载后补挂此前因无 ELF 而未解析的变量 */
void os_layout_apply_pending(void);

#endif
