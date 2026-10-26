#!/bin/sh
# Linux/gcc：用 shim/windows.h 编译真实 das.c + 假 MCD。用法：./build.sh [tsan|asan]
set -e
cd "$(dirname "$0")"
SRC=../..
SAN=""
case "$1" in tsan) SAN="-fsanitize=thread";; asan) SAN="-fsanitize=address,undefined";; esac
gcc -std=gnu99 -g -O1 -Wall -Wextra -Wno-cast-function-type -Wno-unused-parameter $SAN \
    -Ishim -I. -I$SRC/code/src -I$SRC/module/das -include pre.h \
    test_drv_reconnect.c fake_mcd.c $SRC/module/das/das.c $SRC/module/das/das_util.c \
    -lpthread -o test_drv_reconnect${1:+_$1}
echo built test_drv_reconnect${1:+_$1}
