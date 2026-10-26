@echo off
rem Bug4 回归：构建假慢探针驱动 tests\bin\slowdrv\dll\slowdrv.dll（隔离目录，不进 dll\）
setlocal
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if not exist "%~dp0bin\slowdrv\dll" mkdir "%~dp0bin\slowdrv\dll"
if not exist "%~dp0bin\obj" mkdir "%~dp0bin\obj"
cl /nologo /O2 /LD /utf-8 /W3 /Fo"%~dp0bin\obj\\" "%~dp0slowdrv.c" /Fe"%~dp0bin\slowdrv\dll\slowdrv.dll" /link /NOLOGO user32.lib >nul
if errorlevel 1 ( echo [ERROR] slowdrv build failed & exit /b 1 )
echo slowdrv OK
exit /b 0
