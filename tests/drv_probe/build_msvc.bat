@echo off
rem Windows/MSVC：在 VS x64 开发者命令行运行。真实 windows.h + 假厂商 DLL（/FI pre_probe.h 重定向 LoadLibraryA/GetProcAddress/FreeLibrary）
cd /d %~dp0
cl /nologo /utf-8 /W4 /Zi /I. /I..\..\code\src /I..\..\module\jlink /FIpre_probe.h /DPROBE_JLINK ^
   test_probe.c fake_common.c fake_jlink.c ..\..\module\jlink\jlink.c /Fe:test_jlink.exe
if errorlevel 1 exit /b 1
cl /nologo /utf-8 /W4 /Zi /I. /I..\..\code\src /I..\..\module\stlink /FIpre_probe.h /DPROBE_STLINK ^
   test_probe.c fake_common.c fake_stlink.c ..\..\module\stlink\stlink.c /Fe:test_stlink.exe
if errorlevel 1 exit /b 1
test_jlink.exe
if errorlevel 1 exit /b 1
test_stlink.exe
