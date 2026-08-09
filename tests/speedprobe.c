/*
 * OpenScope J-Link 速度探针 — 直连 dll/JLink_x64.dll，观察：
 *   - SetSpeed(S) 后 GetSpeed()（连接前/后）
 *   - 连接后内存读取是否成功（Bug 9：非4000速度读值为0）
 * 用 GetProcAddress 直接绑定，绕开模块层。
 * Build: cl /nologo /W2 /utf-8 tests\speedprobe.c /Fe:tests\bin\speedprobe.exe
 */
#include <windows.h>
#include <stdio.h>
#include <string.h>

typedef int (__stdcall *fn_open)(void*);
typedef int (__stdcall *fn_close)(void);
typedef int (__stdcall *fn_connect)(void);
typedef int (__stdcall *fn_isconnected)(void);
typedef int (__stdcall *fn_setif)(int);
typedef int (__stdcall *fn_setspeed)(unsigned long);
typedef unsigned long (__stdcall *fn_getspeed)(void);
typedef int (__stdcall *fn_exec)(const char*, char*, int);
typedef int (__stdcall *fn_readmem)(unsigned long, unsigned long, unsigned char*);
typedef int (__stdcall *fn_select)(int);

int main(void)
{
    HMODULE h = LoadLibraryA("dll/JLink_x64.dll");
    unsigned long S;
    int rc, i, fail = 0;
    static const unsigned long speeds[] = { 1000, 2000, 4000, 5000, 8000, 10000, 12000 };
    fn_open open = (fn_open)GetProcAddress(h, "JLINKARM_Open");
    fn_close close = (fn_close)GetProcAddress(h, "JLINKARM_Close");
    fn_connect connect = (fn_connect)GetProcAddress(h, "JLINKARM_Connect");
    fn_isconnected isc = (fn_isconnected)GetProcAddress(h, "JLINKARM_IsConnected");
    fn_setif setif = (fn_setif)GetProcAddress(h, "JLINKARM_TIF_Select");
    fn_setspeed ss = (fn_setspeed)GetProcAddress(h, "JLINKARM_SetSpeed");
    fn_getspeed gs = (fn_getspeed)GetProcAddress(h, "JLINKARM_GetSpeed");
    fn_exec exec = (fn_exec)GetProcAddress(h, "JLINKARM_ExecCommand");
    fn_readmem rm = (fn_readmem)GetProcAddress(h, "JLINKARM_ReadMem");
    fn_select sel = (fn_select)GetProcAddress(h, "JLINKARM_EMU_SelectByIndex");
    if (!h || !open || !gs || !ss) { printf("FAIL bind err=%lu\n", GetLastError()); return 1; }
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("JLink_x64.dll v%lu\n\n", ((unsigned long(__stdcall*)(void))GetProcAddress(h,"JLINKARM_GetDLLVersion"))());
    for (i = 0; i < (int)(sizeof(speeds)/sizeof(speeds[0])); i++) {
        unsigned char buf[4] = {0,0,0,0};
        unsigned long actual_before, actual_after;
        char res[256] = "";
        unsigned long v;
        S = speeds[i];
        rc = open(NULL); if (rc) { printf("speed=%lu OPEN fail rc=%d\n", S, rc); fail++; continue; }
        if (sel) sel(0);
        setif(1); /* SWD */
        exec("Device = Cortex-M4", res, sizeof(res));
        ss(S);
        actual_before = gs();
        rc = connect();
        actual_after = gs();
        rc = rm(0x20000000, 4, buf);
        memcpy(&v, buf, 4);
        printf("speed=%lu set -> before_conn=%lu after_conn=%lu conn_rc=%d isc=%d read=%s val=0x%08X\n",
               S, actual_before, actual_after, rc, isc(), rc==0 ? "ok" : "fail", v);
        if (v == 0) fail++;
        close();
        Sleep(300);
    }
    printf("%s\n", fail ? "FAILURES (含0读)" : "ALL PASS");
    FreeLibrary(h);
    return 0;
}
