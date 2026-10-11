@echo off
set JAVA_HOME=D:\OpenScope\JScope_REVERSE\tools\jdk-21.0.12.1+1
set JSCOPE_OUT=D:\OpenScope\JScope_REVERSE\dumps
cd /d D:\OpenScope\JScope_REVERSE\tools
call ghidra_12.1.4_PUBLIC\support\analyzeHeadless.bat "D:\OpenScope\JScope_REVERSE\ghidra_proj" JScopeProj -process JScope.exe -noanalysis -scriptPath "D:\OpenScope\JScope_REVERSE\tools\ghidra_scripts" -postScript CallersDecomp.java "14003fa70,140041710,140042a60,14003ef60,14003eee0,1400427b0,140040980,14003e050" target_decomp.txt 0 20
