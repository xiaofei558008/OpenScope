/* 验证 JLINKARM_WriteMem 返回值约定：写回原值，观察 rc。 */
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
typedef int (__stdcall *fn_writemem)(unsigned long, unsigned long, const unsigned char*);
typedef int (__stdcall *fn_select)(int);
int main(void)
{
    HMODULE h = LoadLibraryA("dll/JLink_x64.dll");
    fn_open open=(fn_open)GetProcAddress(h,"JLINKARM_Open");
    fn_close close=(fn_close)GetProcAddress(h,"JLINKARM_Close");
    fn_connect connect=(fn_connect)GetProcAddress(h,"JLINKARM_Connect");
    fn_setif setif=(fn_setif)GetProcAddress(h,"JLINKARM_TIF_Select");
    fn_setspeed ss=(fn_setspeed)GetProcAddress(h,"JLINKARM_SetSpeed");
    fn_exec exec=(fn_exec)GetProcAddress(h,"JLINKARM_ExecCommand");
    fn_readmem rm=(fn_readmem)GetProcAddress(h,"JLINKARM_ReadMem");
    fn_writemem wm=(fn_writemem)GetProcAddress(h,"JLINKARM_WriteMem");
    fn_select sel=(fn_select)GetProcAddress(h,"JLINKARM_EMU_SelectByIndex");
    unsigned char b[4]={0,0,0,0}; int rw, rc;
    setvbuf(stdout,NULL,_IONBF,0);
    open(NULL); if(sel) sel(0); setif(1);
    { char r[256]=""; exec("Device = Cortex-M4", r, sizeof(r)); }
    ss(4000);
    printf("connect rc=%d\n", connect());
    rc = rm(0x20000000,4,b);
    printf("read rc=%d\n", rc);
    rw = wm(0x20000000,4,b);   /* 写回原值 */
    printf("WriteMem rc=%d (0=成功, 4=字节数?)  val_written=0x%02X%02X%02X%02X\n",
           rw, b[3],b[2],b[1],b[0]);
    close();
    return 0;
}
