@echo off
set JAVA_HOME=D:\OpenScope\JScope_REVERSE\tools\jdk-21.0.12.1+1
cd /d D:\OpenScope\JScope_REVERSE\tools
call ghidra_12.1.4_PUBLIC\support\analyzeHeadless.bat "D:\OpenScope\JScope_REVERSE\ghidra_proj" JScopeProj -import "C:\Program Files\SEGGER\JLink_V966\JScope.exe" -overwrite
