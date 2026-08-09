/* 直连 JLink_x64.dll，连接一次后连续读 30 次，观察是否在模块层产生退化。
 * 对比 read_smoke（经模块层）。 */
#include <windows.h>
#include <stdio.h>
#include <string.h>

typedef int (__stdcall *fn_open)(void*);
typedef int (__stdcall *fn_close)(void);
typedef int (__stdcall *fn_connect)(void);
typedef int (__stdcall *fn_setif)(int);
typedef int (__stdcall *fn_setspeed)(unsigned long);
typedef int (__stdcall *fn_exec)(const char*, char*, int);
typedef int (__stdcall *fn_readmem)(unsigned long, unsigned long, unsigned char*);
typedef int (__stdcall *fn_select)(int);

int main(int argc, char** argv)
{
    unsigned long S = (argc > 1) ? (unsigned long)atoi(argv[1]) : 4000;
    int nreads = (argc > 2) ? atoi(argv[2]) : 30;
    const char* dev = (argc > 3) ? argv[3] : "Cortex-M4";
    HMODULE h = LoadLibraryA("dll/JLink_x64.dll");
    fn_open open=(fn_open)GetProcAddress(h,"JLINKARM_Open");
    fn_close close=(fn_close)GetProcAddress(h,"JLINKARM_Close");
    fn_connect connect=(fn_connect)GetProcAddress(h,"JLINKARM_Connect");
    fn_setif setif=(fn_setif)GetProcAddress(h,"JLINKARM_TIF_Select");
    fn_setspeed ss=(fn_setspeed)GetProcAddress(h,"JLINKARM_SetSpeed");
    fn_exec exec=(fn_exec)GetProcAddress(h,"JLINKARM_ExecCommand");
    fn_readmem rm=(fn_readmem)GetProcAddress(h,"JLINKARM_ReadMem");
    fn_select sel=(fn_select)GetProcAddress(h,"JLINKARM_EMU_SelectByIndex");
    int i, ok=0, fail=0, delay = (argc > 4) ? atoi(argv[4]) : 0;
    setvbuf(stdout, NULL, _IONBF, 0);
    if (open(NULL)) { printf("open fail\n"); return 1; }
    if (sel) sel(0);
    setif(1);
    { char r[256]=""; exec("Device = Cortex-M4", r, sizeof(r)); }
    ss(S);
    if (connect()) { printf("connect fail at %lu\n", S); close(); return 1; }
    for (i = 0; i < nreads; i++) {
        unsigned char buf[4]={0,0,0,0}; unsigned long v=0;
        int rc = rm(0x20000000, 4, buf);
        memcpy(&v, buf, 4);
        if (rc == 0 && v != 0) ok++; else fail++;
        if (delay) Sleep(delay);
        if (i < 12) printf("read[%02d] rc=%d val=0x%08X\n", i, rc, v);
    }
    printf("raw device=%s speed=%lu: reads=%d ok=%d fail=%d\n", dev, S, nreads, ok, fail);
    close();
    FreeLibrary(h);
    return 0;
}
