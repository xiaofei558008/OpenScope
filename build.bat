@echo off
setlocal
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 (
  echo [ERROR] vcvars64.bat failed
  exit /b 1
)
msbuild OpenScope.sln /p:Configuration=Release /p:Platform=x64 /m /v:m
if errorlevel 1 (
  echo [ERROR] Build failed
  exit /b 1
)
echo.
echo Build OK.
echo   Framework : bin\Release\OpenScope.exe
echo   Modules   : dll\jlink.dll dll\scope.dll
echo   J-Link DLL: dll\JLink_x64.dll
echo.
call tests\build_tests.bat
if errorlevel 1 (
  echo [ERROR] Tests failed
  exit /b 1
)
exit /b 0
