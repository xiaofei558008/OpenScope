@echo off
rem MSVC: cl from a VS developer prompt. Optional: set OS_TC375_ELF=path\to\tc375_scope.elf
cd /d %~dp0
if not exist line_sample.out python gen_line_elf.py line_sample.out
cl /nologo /utf-8 /W3 /Zi /I..\..\code\src test_line_map.c ..\..\code\src\elf.c ..\..\code\src\dwloc.c /Fe:test_line_map.exe || exit /b 1
test_line_map.exe
