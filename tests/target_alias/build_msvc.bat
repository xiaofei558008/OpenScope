@echo off
rem MSVC: run in a VS x64 developer prompt (or after vcvars64.bat)
cd /d %~dp0
cl /nologo /utf-8 /W4 /WX /I..\..\code\src test_target_alias.c ..\..\code\src\target_alias.c /Fe:test_target_alias.exe
if errorlevel 1 exit /b 1
test_target_alias.exe
