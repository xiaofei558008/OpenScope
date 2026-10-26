@echo off
rem Windows/MSVC：在 VS x64 开发者命令行运行。/FI pre_conn.h 把 GetModuleHandleExW/FreeLibrary/PostMessageW 重定向到测试假实现
cd /d %~dp0
cl /nologo /utf-8 /W4 /Zi /I. /I..\..\code\src /FIpre_conn.h test_conn_async.c ..\..\code\src\conn_async.c /Fe:test_conn_async.exe
if errorlevel 1 exit /b 1
test_conn_async.exe
if errorlevel 1 exit /b 1
rem 第四轮：AI 线程连接 vs UI 连接
cl /nologo /utf-8 /W4 /Zi /I. /I..\..\code\src /FIpre_conn.h test_conn_race.c ..\..\code\src\conn_async.c /Fe:test_conn_race.exe
if errorlevel 1 exit /b 1
test_conn_race.exe
