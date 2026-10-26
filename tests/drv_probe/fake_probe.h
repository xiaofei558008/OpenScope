/* 假探针 DLL（JLink_x64.dll / CubeProgrammer_API.dll）的公共控制量与统计 */
#ifndef FAKE_PROBE_H
#define FAKE_PROBE_H
extern volatile LONG g_fk_open_delay_ms;   /* 连接（JLINKARM_Open / connectStLink）卡住多久 */
extern volatile LONG g_fk_read_delay_ms;   /* 大块读（>256B）在 DLL 内耗时 */
extern volatile LONG g_fk_fail;            /* 1 = 目标掉线：读失败、IsConnected/checkDeviceConnection=0 */
extern volatile LONG g_fk_inside, g_fk_max_inside;  /* DLL 内并发调用数（必须始终 ≤1） */
extern volatile LONG g_fk_open, g_fk_close;         /* 会话打开/关闭次数 */
extern volatile LONG g_fk_stale;           /* 无会话或 DLL 已卸载时被调用的次数（必须 0） */
extern volatile LONG g_fk_load, g_fk_free;  /* LoadLibrary / FreeLibrary 次数 */
extern volatile LONG g_fk_bp_set, g_fk_run_ctl;
/* 目标运行状态（StepOut 测试）：Go 后 g_fk_run_ms 毫秒自动停住（命中断点）；0 = 立刻停（默认，旧测试语义）；-1 = 一直跑 */
extern volatile LONG g_fk_run_ms, g_fk_bp_live;
int  fk_halted(void);
void fk_go(void);
void fk_halt(void);
#define FK_SET(v, x) InterlockedExchange(&(v), (LONG)(x))
#define FK_GET(v)    InterlockedCompareExchange(&(v), 0, 0)
void fk_enter(int need_session);
void fk_leave(void);
void fk_nap(volatile LONG* ms);
#endif
