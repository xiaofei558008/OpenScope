@echo off
set JAVA_HOME=D:\OpenScope\JScope_REVERSE\tools\jdk-21.0.12.1+1
set JSCOPE_OUT=D:\OpenScope\JScope_REVERSE\dumps
cd /d D:\OpenScope\JScope_REVERSE\tools
call ghidra_12.1.4_PUBLIC\support\analyzeHeadless.bat "D:\OpenScope\JScope_REVERSE\ghidra_proj" JScopeProj -process JScope.exe -noanalysis -scriptPath "D:\OpenScope\JScope_REVERSE\tools\ghidra_scripts" -postScript DecompAt.java orphans.txt "0x14003cea0,0x14003d7c0,0x14003d930,0x14003d940,0x14003dc70,0x14003d9a0,0x140042eb0,0x140042ee0"
