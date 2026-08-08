@echo off
setlocal
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 (
  echo [ERROR] vcvars64.bat failed
  exit /b 1
)
if not exist tests\bin mkdir tests\bin

cl /nologo /W2 /utf-8 /I code\src tests\replay_smoke.c code\src\datalog.c ^
    /Fe:tests\bin\replay_smoke.exe /link user32.lib
if errorlevel 1 (
  echo [ERROR] replay_smoke build failed
  exit /b 1
)

cl /nologo /W2 /utf-8 /I code\src module\jlink\tests\jlink_smoke.c ^
    /Fe:tests\bin\jlink_smoke.exe
if errorlevel 1 (
  echo [ERROR] jlink_smoke build failed
  exit /b 1
)

echo Running replay smoke...
tests\bin\replay_smoke.exe
if errorlevel 1 (
  echo [ERROR] replay_smoke FAILED
  exit /b 1
)

echo Running jlink smoke...
tests\bin\jlink_smoke.exe
if errorlevel 1 (
  echo [ERROR] jlink_smoke FAILED
  exit /b 1
)

echo Tests OK.
exit /b 0
