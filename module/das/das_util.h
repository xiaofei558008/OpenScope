/*
 * das_util.h — DAS 驱动模块的纯逻辑辅助（无 Windows/MCD 依赖，便于单元测试）
 */
#ifndef OS_DAS_UTIL_H
#define OS_DAS_UTIL_H
#include <stdint.h>

/* 单个 MCD 事务最大字节数、单个 txlist 最大事务数 */
#define DAS_TX_MAX_BYTES 1024u
#define DAS_TXLIST_MAX   64u

/* 写保护区（AURIX TC3xx）：
 *  0x80000000..0x8FFFFFFF  PFLASH（cached）
 *  0xA0000000..0xAFFFFFFF  PFLASH/DFLASH/UCB（non-cached，含 UCB 0xAF400000..）
 * 返回 1 表示 [addr, addr+size) 与任一保护区有交集（禁止经 WRITE_MEM 写入）。 */
int das_range_is_protected(uint64_t addr, uint32_t size);

/* 是否外设/SFR 区（0xF0000000 以上）：此区需 access_width=4 的对齐单字访问 */
int das_addr_is_sfr(uint64_t addr);

/* 将 [addr, addr+size) 拆成不超过 max_bytes 的块；返回块数（最多 max_chunks），
 * 超出 max_chunks 返回 -1。out_addr/out_len 可为 NULL（只计数）。 */
int das_split_chunks(uint64_t addr, uint32_t size, uint32_t max_bytes,
                     uint64_t* out_addr, uint32_t* out_len, int max_chunks);

/* 4 字节对齐窗口：把任意 [addr,size) 扩展为覆盖它的 4 字节对齐区间 */
void das_align4(uint64_t addr, uint32_t size, uint64_t* a_addr, uint32_t* a_size);

/* MCD core state → OpenScope 状态文案（RUNNING / STOP / RESET / —），halted 输出 1/0/-1 */
const char* das_state_text(uint32_t mcd_state, int* halted);

/* 解析连接配置 device 字段中的核号："core1" / "CPU2" / "tc0" / "1"，空或无法识别 → 0 */
int das_parse_core_index(const char* device);

#endif
