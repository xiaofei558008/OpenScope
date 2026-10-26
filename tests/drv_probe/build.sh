#!/bin/sh
# Linux/gcc：用 ../drv_reconnect/shim/windows.h 编译真实 jlink.c / stlink.c + 假厂商 DLL。
# 用法：./build.sh [tsan|asan]   产物：test_jlink[_san]、test_stlink[_san]
set -e
cd "$(dirname "$0")"
SRC=../..
SAN=""
case "$1" in tsan) SAN="-fsanitize=thread";; asan) SAN="-fsanitize=address,undefined";; esac
CF="-std=gnu99 -g -O1 -Wall -Wextra -Wno-cast-function-type -Wno-unused-parameter -Wno-format $SAN -I../drv_reconnect/shim -I. -I$SRC/code/src -include pre_probe.h"
gcc $CF -DPROBE_JLINK -I$SRC/module/jlink test_probe.c fake_common.c fake_jlink.c $SRC/module/jlink/jlink.c -lpthread -o test_jlink${1:+_$1}
gcc $CF -DPROBE_STLINK -I$SRC/module/stlink test_probe.c fake_common.c fake_stlink.c $SRC/module/stlink/stlink.c -lpthread -o test_stlink${1:+_$1}
echo built test_jlink${1:+_$1} test_stlink${1:+_$1}
