@echo off
rem Windows/MSVC：在 VS x64 开发者命令行运行。真实 jlink.c / stlink.c + 假厂商 DLL（..\drv_probe）+ cmd_async.c
rem cmd_async.c 单独用 /FI pre_cmd.h 编译（钉住/投递改为测试计数），其余用 /FI pre_probe.h（假厂商 DLL）
cd /d %~dp0
set P=..\drv_probe
set INC=/I. /I%P% /I..\..\code\src
cl /nologo /utf-8 /W4 /Zi %INC% /FIpre_cmd.h /c ..\..\code\src\cmd_async.c /Fo:cmd_async_t.obj
if errorlevel 1 exit /b 1
cl /nologo /utf-8 /W4 /Zi %INC% /I..\..\module\jlink /FIpre_probe.h /DPROBE_JLINK ^
   test_stepout.c %P%\fake_common.c %P%\fake_jlink.c ..\..\module\jlink\jlink.c cmd_async_t.obj /Fe:test_stepout_jlink.exe
if errorlevel 1 exit /b 1
cl /nologo /utf-8 /W4 /Zi %INC% /I..\..\module\stlink /FIpre_probe.h /DPROBE_STLINK ^
   test_stepout.c %P%\fake_common.c %P%\fake_stlink.c ..\..\module\stlink\stlink.c cmd_async_t.obj /Fe:test_stepout_stlink.exe
if errorlevel 1 exit /b 1
test_stepout_jlink.exe
if errorlevel 1 exit /b 1
test_stepout_stlink.exe
