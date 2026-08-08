#ifndef OS_UTIL_H
#define OS_UTIL_H

#include <windows.h>
#include <stdint.h>
#include "elf.h"

/* 枚举值表项（用于叶子变量的枚举名显示/解析） */
typedef struct OS_EnumVal {
    char    name[64];
    int64_t value;
} OS_EnumVal;

/* Unix 纪元（1970-01-01）微秒 */
int64_t os_time_us(void);
/* "2026-08-08 15:30:00.123456"（本地时区） */
void os_time_iso(int64_t us, char* out, int outlen);

/* 日志：level 见 module_api.h OS_LOG_*；UTF-8 文本 */
void os_log_set(void (*fn)(int level, const wchar_t* line));
void os_log(int level, const char* fmt, ...);
/* 日志文件（应用启动时调用）：优先 exe 目录 openscope.log，写不进去回退 %LOCALAPPDATA%\OpenScope */
void os_log_file_auto_open(void);
/* 给定路径打开日志文件（追加模式，空文件写 UTF-8 BOM） */
void os_log_file_open(const wchar_t* path);
/* 崩溃处理器用：不经过 UI 回调，直接写日志文件（同步刷新） */
void os_log_file_write_raw(const char* line);
/* 把窗口客户区保存为 32bpp BMP（调试/截图用） */
void os_save_window_bmp(HWND hwnd, const wchar_t* path);

/* UTF-8 <-> UTF-16 转换 */
wchar_t* os_utf8_to_wide(const char* s);
char*    os_wide_to_utf8(const wchar_t* s);
void     os_utf8_to_wide_buf(const char* s, wchar_t* out, int outlen);
void     os_wide_to_utf8_buf(const wchar_t* s, char* out, int outlen);

/* 数值解码：raw 为小端字节（<=8），按类型解释。
 * bitfield 时 size 为存储字节数，从 bit_offset 提取 bit_size 位。
 * 返回 double 解释值；ok 置 0 表示无法解释。 */
double os_decode_value(const uint8_t* raw, int size, OS_TypeKind kind, int is_signed,
                       int is_bitfield, int bit_offset, int bit_size, int* ok);

/* 格式化：out 输出文本；out_val 返回数值解释（可 NULL） */
void os_format_raw(char* out, int outlen, const uint8_t* raw, int size,
                   OS_TypeKind kind, int is_signed, int is_ptr, int is_bitfield, int bit_offset, int bit_size,
                   double* out_val, const OS_EnumVal* enums, int enum_count);

/* 文本解析为字节（小端）。max_size 为输出缓冲上限；*out_size 返回实际字节数。
 * bitfield 时先清零 max_size 字节再写入移位后的值（调用方需自行做读-改-写合并）。
 * 返回 1 成功，0 解析失败。 */
int os_parse_text(const char* text, uint8_t* out, int max_size, int* out_size,
                  OS_TypeKind kind, int is_signed, int is_bitfield, int bit_offset, int bit_size,
                  const OS_EnumVal* enums, int enum_count);

/* 文件修改时间（毫秒，1601 纪元），失败返回 0 */
uint64_t os_file_mtime_ms(const wchar_t* path);

#endif
