@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 (echo vcvars fail & exit /b 1)
cl /nologo /W2 /utf-8 tests\reconnect.c /Fe:tests\bin\reconnect.exe
if errorlevel 1 (echo reconnect build fail & exit /b 1)
echo reconnect build OK.
