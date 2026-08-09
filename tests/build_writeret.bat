@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 (echo vcvars fail & exit /b 1)
cl /nologo /W2 /utf-8 tests\writeret.c /Fe:tests\bin\writeret.exe
if errorlevel 1 (echo writeret build fail & exit /b 1)
echo writeret build OK.
