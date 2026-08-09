@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 (
  echo [ERROR] vcvars64.bat failed
  exit /b 1
)
cl /nologo /W2 /utf-8 /I code\src tests\speed_smoke.c ^
    /Fe:tests\bin\speed_smoke.exe
if errorlevel 1 (
  echo [ERROR] speed_smoke build failed
  exit /b 1
)
echo speed_smoke build OK.
