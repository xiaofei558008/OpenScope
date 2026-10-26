@echo off
rem Windows/MSVC：在 VS x64 开发者命令行运行。真实 windows.h + 假 MCD（/FI pre.h 重定向 LoadLibraryExA/GetProcAddress）
cd /d %~dp0
cl /nologo /utf-8 /W4 /Zi /I. /I..\..\code\src /I..\..\module\das /FIpre.h ^
   test_drv_reconnect.c fake_mcd.c ..\..\module\das\das.c ..\..\module\das\das_util.c ^
   /Fe:test_drv_reconnect.exe
if errorlevel 1 exit /b 1
test_drv_reconnect.exe
