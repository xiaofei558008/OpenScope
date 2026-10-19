@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist tests\bin mkdir tests\bin
cl /nologo /W2 /utf-8 /I module\xcp module\xcp\tests\xcp_smoke.c module\xcp\xcp_slave.c module\xcp\xcp_master.c module\xcp\xcp_transport.c module\xcp\xcp_serial.c module\xcp\xcp_tcp.c module\xcp\xcp_can.c /Fe:tests\bin\xcp_smoke.exe /link ws2_32.lib
if errorlevel 1 exit /b 1
tests\bin\xcp_smoke.exe
exit /b %ERRORLEVEL%
