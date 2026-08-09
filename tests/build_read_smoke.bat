@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 (
  echo [ERROR] vcvars64.bat failed
  exit /b 1
)
cl /nologo /W2 /utf-8 /I code\src tests\read_smoke.c ^
    /Fe:tests\bin\read_smoke.exe
if errorlevel 1 (
  echo [ERROR] read_smoke build failed
  exit /b 1
)
echo read_smoke build OK.
