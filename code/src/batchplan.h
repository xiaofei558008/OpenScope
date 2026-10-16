#ifndef OS_BATCHPLAN_H
#define OS_BATCHPLAN_H

#include <stdint.h>

/* 高速采集块读合并（纯函数，无全局状态，可单元测试）。
 *
 * 一次 J-Link/ST-Link 读事务的固定开销约 250µs；线上时间 = 字节数 × 10bit / 时钟。
 * 时钟单位是 kHz（4000 = 4MHz）。把 kHz 当成 MHz 用会把合并空隙放大 1000 倍，
 * 上限被夹到 4096 字节后，分散变量之间的空洞会被一起读出来，未映射地址返回
 * ReadMem 失败（ret=-5），整块里的有效变量也一起丢掉。
 *
 * 空隙 G 字节的线上时间（µs）= G * 10 * 1000 / speed_khz。
 * 合并划算条件：该时间 ≤ 250µs → G ≤ speed_khz / 40。
 *   4MHz → 100B；12MHz → 300B；50MHz → 1250B（硬上限 4096）。
 * 块读总长用同一预算，下限 256（保证一小簇连续变量仍能一次读完）。 */

#define OS_TX_OVERHEAD_US  250
#define OS_MERGE_WIRE_BIT  10
#define OS_BATCH_RUN_MIN   256
#define OS_BATCH_RUN_MAX   4096
#define OS_BATCH_GAP_MIN   8

static int os_bp_wire_budget(int speed_khz)
{
    int s = speed_khz > 0 ? speed_khz : 4000;
    return s * OS_TX_OVERHEAD_US / (OS_MERGE_WIRE_BIT * 1000);
}

static int os_bp_gap_limit(int speed_khz)
{
    int lim = os_bp_wire_budget(speed_khz);
    if (lim < OS_BATCH_GAP_MIN) lim = OS_BATCH_GAP_MIN;
    if (lim > OS_BATCH_RUN_MAX) lim = OS_BATCH_RUN_MAX;
    return lim;
}

static int os_bp_run_limit(int speed_khz)
{
    int lim = os_bp_wire_budget(speed_khz);
    if (lim < OS_BATCH_RUN_MIN) lim = OS_BATCH_RUN_MIN;
    if (lim > OS_BATCH_RUN_MAX) lim = OS_BATCH_RUN_MAX;
    return lim;
}

/* 1 = 下一叶必须新开一块（空隙或总长超出预算）。地址已按升序。 */
static int os_bp_should_split(uint64_t run_addr, uint64_t run_end,
                              uint64_t next_addr, uint32_t next_size,
                              int gap_lim, int run_lim)
{
    uint64_t end;
    if (gap_lim < 0) gap_lim = 0;
    if (run_lim < 1) run_lim = 1;
    if (next_addr < run_addr) return 1;
    end = next_addr + (uint64_t)next_size;
    if (next_addr > run_end + (uint64_t)gap_lim) return 1;
    if (end < run_addr) return 1; /* 溢出 */
    if (end - run_addr > (uint64_t)run_lim) return 1;
    return 0;
}

#endif
