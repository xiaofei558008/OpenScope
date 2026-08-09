/* 测试：连接时用 4000，连接后再 SetSpeed(S)——非默认速度是否存活（Bug 9 关键实验）。 */
#include <windows.h>
#include <stdio.h>
#include <string.h>
typedef int (__stdcall *fn_open)(void*);
typedef int (__stdcall *fn_close)(void);
typedef int (__stdcall *fn_connect)(void);
typedef int (__stdcall *fn_isconn)(void);
typedef int (__stdcall *fn_setif)(int);
typedef int (__stdcall *fn_setspeed)(unsigned long);
typedef int (__stdcall *fn_getspeed)(void);
typedef int (__stdcall *fn_exec)(const char*, char*, int);
typedef int (__stdcall *fn_readmem)(unsigned long, unsigned long, unsigned char*);
typedef int (__stdcall *fn_select)(int);
int main(int argc, char** argv)
{
    unsigned long S = (argc>1)?(unsigned long)atoi(argv[1]):2000;
    HMODULE h = LoadLibraryA("dll/JLink_x64.dll");
    fn_open open=(fn_open)GetProcAddress(h,"JLINKARM_Open");
    fn_close close=(fn_close)GetProcAddress(h,"JLINKARM_Close");
    fn_connect connect=(fn_connect)GetProcAddress(h,"JLINKARM_Connect");
    fn_isconn isc=(fn_isconn)GetProcAddress(h,"JLINKARM_IsConnected");
    fn_setif setif=(fn_setif)GetProcAddress(h,"JLINKARM_TIF_Select");
    fn_setspeed ss=(fn_setspeed)GetProcAddress(h,"JLINKARM_SetSpeed");
    fn_getspeed gs=(fn_getspeed)GetProcAddress(h,"JLINKARM_GetSpeed");
    fn_exec exec=(fn_exec)GetProcAddress(h,"JLINKARM_ExecCommand");
    fn_readmem rm=(fn_readmem)GetProcAddress(h,"JLINKARM_ReadMem");
    fn_select sel=(fn_select)GetProcAddress(h,"JLINKARM_EMU_SelectByIndex");
    int i;
    setvbuf(stdout,NULL,_IONBF,0);
    open(NULL); if(sel) sel(0); setif(1);
    { char r[256]=""; exec("Device = Cortex-M4", r, sizeof(r)); }
    ss(4000);                          /* 先用默认 4000 连接 */
    printf("connect(4000) rc=%d isc=%d\n", connect(), isc());
    ss(S);                             /* 连接后再改速度 */
    printf("SetSpeed(%lu) after connect -> GetSpeed=%lu\n", S, gs());
    for (i = 0; i < 15; i++) {
        unsigned char b[4]={0,0,0,0}; unsigned long v=0;
        int rc = rm(0x20000000,4,b); memcpy(&v,b,4);
        printf("read[%02d] rc=%d isc=%d val=0x%08X\n", i, rc, isc(), v);
        Sleep(5);
    }
    close();
    return 0;
}
