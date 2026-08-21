@echo off
setlocal
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 (
  echo [ERROR] vcvars64.bat failed
  exit /b 1
)
if not exist tests\bin mkdir tests\bin

python tests\gen_elf_out.py
if errorlevel 1 (
  echo [ERROR] gen_elf_out failed
  exit /b 1
)

python tests\gen_elf_out_v2.py
if errorlevel 1 (
  echo [ERROR] gen_elf_out_v2 failed
  exit /b 1
)

cl /nologo /W2 /utf-8 /I code\src tests\elf_smoke.c ^
    code\src\elf.c code\src\vartree.c code\src\util.c ^
    /Fe:tests\bin\elf_smoke.exe /link user32.lib gdi32.lib
if errorlevel 1 (
  echo [ERROR] elf_smoke build failed
  exit /b 1
)

cl /nologo /W2 /utf-8 /I code\src tests\dump_elf.c ^
    code\src\elf.c code\src\vartree.c code\src\util.c ^
    /Fe:tests\bin\dump_elf.exe /link user32.lib gdi32.lib
if errorlevel 1 (
  echo [ERROR] dump_elf build failed
  exit /b 1
)

cl /nologo /W2 /utf-8 /I code\src tests\enc_smoke.c ^
    code\src\elf.c code\src\vartree.c code\src\util.c ^
    /Fe:tests\bin\enc_smoke.exe /link user32.lib gdi32.lib
if errorlevel 1 (
  echo [ERROR] enc_smoke build failed
  exit /b 1
)

cl /nologo /W2 /utf-8 /I code\src tests\replay_smoke.c code\src\datalog.c ^
    /Fe:tests\bin\replay_smoke.exe /link user32.lib
if errorlevel 1 (
  echo [ERROR] replay_smoke build failed
  exit /b 1
)

echo Running elf smoke...
tests\bin\elf_smoke.exe
if errorlevel 1 (
  echo [ERROR] elf_smoke FAILED
  exit /b 1
)

cl /nologo /W2 /utf-8 /I code\src module\jlink\tests\jlink_smoke.c ^
    /Fe:tests\bin\jlink_smoke.exe
if errorlevel 1 (
  echo [ERROR] jlink_smoke build failed
  exit /b 1
)

echo Running enc smoke...
tests\bin\enc_smoke.exe
if errorlevel 1 (
  echo [ERROR] enc_smoke FAILED
  exit /b 1
)

echo Running replay smoke...
tests\bin\replay_smoke.exe
if errorlevel 1 (
  echo [ERROR] replay_smoke FAILED
  exit /b 1
)

cl /nologo /W2 /utf-8 /I code\src tests\chartview_smoke.c code\src\chartview.c ^
    /Fe:tests\bin\chartview_smoke.exe
if errorlevel 1 (
  echo [ERROR] chartview_smoke build failed
  exit /b 1
)

echo Running chartview smoke...
tests\bin\chartview_smoke.exe
if errorlevel 1 (
  echo [ERROR] chartview_smoke FAILED
  exit /b 1
)

cl /nologo /W2 /utf-8 /I code\src tests\tilecalc_smoke.c code\src\tilecalc.c ^
    /Fe:tests\bin\tilecalc_smoke.exe
if errorlevel 1 (
  echo [ERROR] tilecalc_smoke build failed
  exit /b 1
)

echo Running tilecalc smoke...
tests\bin\tilecalc_smoke.exe
if errorlevel 1 (
  echo [ERROR] tilecalc_smoke FAILED
  exit /b 1
)

echo Running jlink smoke...
tests\bin\jlink_smoke.exe
if errorlevel 1 (
  echo [ERROR] jlink_smoke FAILED
  exit /b 1
)

cl /nologo /W2 /utf-8 /I code\src tests\bug9_smoke.c ^
    /Fe:tests\bin\bug9_smoke.exe
if errorlevel 1 (
  echo [ERROR] bug9_smoke build failed
  exit /b 1
)

cl /nologo /W2 /utf-8 /I code\src module\stlink\tests\stlink_smoke.c ^
    /Fe:tests\bin\stlink_smoke.exe
if errorlevel 1 (
  echo [ERROR] stlink_smoke build failed
  exit /b 1
)

echo Running stlink smoke (ST-Link module bind/scan/get_info)...
tests\bin\stlink_smoke.exe
if errorlevel 1 (
  echo [ERROR] stlink_smoke FAILED
  exit /b 1
)

cl /nologo /W2 /utf-8 /I code\src module\stlink\tests\stlink_target_smoke.c ^
    /Fe:tests\bin\stlink_target_smoke.exe
if errorlevel 1 (
  echo [ERROR] stlink_target_smoke build failed
  exit /b 1
)

echo Running stlink target smoke (ST-Link connect/read/write/restore)...
tests\bin\stlink_target_smoke.exe
if errorlevel 1 (
  echo [ERROR] stlink_target_smoke FAILED
  exit /b 1
)

echo Running bug9 smoke (J-Link read consistency, one speed per fresh process)...
tests\bin\bug9_smoke.exe Cortex-M4 50
if errorlevel 1 (
  echo [ERROR] bug9_smoke FAILED speed 50
  exit /b 1
)
tests\bin\bug9_smoke.exe Cortex-M4 400
if errorlevel 1 (
  echo [ERROR] bug9_smoke FAILED speed 400
  exit /b 1
)
tests\bin\bug9_smoke.exe Cortex-M4 4000
if errorlevel 1 (
  echo [ERROR] bug9_smoke FAILED speed 4000
  exit /b 1
)
tests\bin\bug9_smoke.exe Cortex-M4 12000
if errorlevel 1 (
  echo [ERROR] bug9_smoke FAILED speed 12000
  exit /b 1
)

echo Tests OK.
exit /b 0
