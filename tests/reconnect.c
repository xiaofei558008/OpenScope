/* 验证自动重连恢复：连接后读取，掉线后 close+connect 重连，能否继续读到有效值。 */
#include <windows.h>
#include <stdio.h>
#include <string.h>
typedef int (__stdcall *fn_open)(void*);
typedef int (__stdcall *fn_close)(void);
typedef int (__stdcall *fn_connect)(void);
typedef int (__stdcall *fn_isconn)(void);
typedef int (__stdcall *fn_setif)(int);
typedef int (__stdcall *fn_setspeed)(unsigned long);
typedef int (__stdcall *fn_exec)(const char*, char*, int);
typedef int (__stdcall *fn_readmem)(unsigned long, unsigned long, unsigned char*);
typedef int (__stdcall *fn_select)(int);
int main(int argc, char** argv)
{
    unsigned long S = (argc>1)?(unsigned long)atoi(argv[1]):4000;
    HMODULE h = LoadLibraryA("dll/JLink_x64.dll");
    fn_open open=(fn_open)GetProcAddress(h,"JLINKARM_Open");
    fn_close close=(fn_close)GetProcAddress(h,"JLINKARM_Close");
    fn_connect connect=(fn_connect)GetProcAddress(h,"JLINKARM_Connect");
    fn_isconn isc=(fn_isconn)GetProcAddress(h,"JLINKARM_IsConnected");
    fn_setif setif=(fn_setif)GetProcAddress(h,"JLINKARM_TIF_Select");
    fn_setspeed ss=(fn_setspeed)GetProcAddress(h,"JLINKARM_SetSpeed");
    fn_exec exec=(fn_exec)GetProcAddress(h,"JLINKARM_ExecCommand");
    fn_readmem rm=(fn_readmem)GetProcAddress(h,"JLINKARM_ReadMem");
    fn_select sel=(fn_select)GetProcAddress(h,"JLINKARM_EMU_SelectByIndex");
    int cycle, ok_total=0, fail_total=0;
    setvbuf(stdout,NULL,_IONBF,0);
    for (cycle = 0; cycle < 4; cycle++) {
        int i, ok=0, fail=0;
        open(NULL); if(sel) sel(0); setif(1);
        { char r[256]=""; exec("Device = Cortex-M4", r, sizeof(r)); }
        ss(S);
        printf("--- cycle %d: connect rc=%d ---\n", cycle, connect());
        for (i = 0; i < 25; i++) {
            unsigned char b[4]={0,0,0,0}; unsigned long v=0;
            int rc = rm(0x20000000,4,b); memcpy(&v,b,4);
            if (rc==0 && v!=0) { ok++; }
            else { fail++; if (isc()==0) break; }   /* 掉线则退出本周期 */
            Sleep(5);
        }
        ok_total+=ok; fail_total+=fail;
        printf("  cycle %d: ok=%d fail=%d isc=%d\n", cycle, ok, fail, isc());
        close(); Sleep(200);
    }
    printf("TOTAL ok=%d fail=%d\n", ok_total, fail_total);
    return 0;
}
