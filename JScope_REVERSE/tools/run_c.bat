@echo off
set JAVA_HOME=D:\OpenScope\JScope_REVERSE\tools\jdk-21.0.12.1+1
set JSCOPE_OUT=D:\OpenScope\JScope_REVERSE\dumps
cd /d D:\OpenScope\JScope_REVERSE\tools
call ghidra_12.1.4_PUBLIC\support\analyzeHeadless.bat "D:\OpenScope\JScope_REVERSE\ghidra_proj" JScopeProj -process JScope.exe -noanalysis -scriptPath "D:\OpenScope\JScope_REVERSE\tools\ghidra_scripts" -postScript XrefDecomp.java "Symbol type is not supported|Preparing Symbols|Finishing Symbols|parsing subkey|size of selected" elf_syms.txt 20
