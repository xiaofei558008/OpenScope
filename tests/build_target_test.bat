@echo off
setlocal
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist tests\bin mkdir tests\bin
cl /nologo /W2 /utf-8 /I code\src module\jlink\tests\target_smoke.c ^
    /Fe:tests\bin\target_smoke.exe
if errorlevel 1 exit /b 1
tests\bin\target_smoke.exe
exit /b %errorlevel%
