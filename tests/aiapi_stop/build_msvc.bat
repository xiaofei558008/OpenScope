@echo off
rem Windows/MSVC：在 VS x64 开发者命令行运行。真实 aiapi.c + netws.c + Winsock；/FI pre_ai.h 把钉住/放开 DLL 改为计数
cd /d %~dp0
cl /nologo /utf-8 /W4 /Zi /I. /I..\..\code\src /I..\..\module\network /FIpre_ai.h /D_CRT_SECURE_NO_WARNINGS /D_WINSOCK_DEPRECATED_NO_WARNINGS ^
   test_aiapi_stop.c ..\..\module\aiapi\aiapi.c ..\..\module\network\netws.c /Fe:test_aiapi_stop.exe /link ws2_32.lib
if errorlevel 1 exit /b 1
test_aiapi_stop.exe
