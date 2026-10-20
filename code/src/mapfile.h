/*
 * .map 链接映射文件解析（GNU ld -Map / Keil armlink / IAR）
 * 仅提取全局符号名、绝对地址与（可选的）大小，供 XCP 页面手工标定。
 */
#ifndef OS_MAPFILE_H
#define OS_MAPFILE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OS_MapSym {
    char     name[192];
    uint64_t address;
    uint32_t size;      /* 0=未知（调用方按 4 字节处理） */
} OS_MapSym;

/* 解析 map 文件。成功返回符号数（>=0），*out 由调用方 os_map_free 释放。
 * 失败返回 -1，err 写入原因（UTF-8）。 */
int  os_map_load(const char* utf8_path, OS_MapSym** out, char* err, int errcap);
void os_map_free(OS_MapSym* syms);

#ifdef __cplusplus
}
#endif

#endif
