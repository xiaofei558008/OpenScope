@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist tests\bin mkdir tests\bin
cl /nologo /W2 /utf-8 /I code\src tests\f303_hw.c /Fe:tests\bin\f303_hw.exe
if errorlevel 1 exit /b 1
tests\bin\f303_hw.exe
exit /b %ERRORLEVEL%
