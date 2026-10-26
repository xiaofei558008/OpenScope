#!/bin/sh
# Linux/gcc：真实 aiapi.c + netws.c，POSIX socket shim。用法：sh build.sh [tsan|asan]
set -e
cd "$(dirname "$0")"
SRC=../..
SAN=""
case "$1" in tsan) SAN="-fsanitize=thread";; asan) SAN="-fsanitize=address,undefined";; esac
gcc -std=gnu99 -g -O1 -Wall -Wextra -Wno-unused-parameter -Wno-sign-compare -Wno-format-truncation $SAN \
    -Ishim -I../drv_reconnect/shim -I. -I$SRC/code/src -I$SRC/module/network -include pre_ai.h \
    test_aiapi_stop.c $SRC/module/aiapi/aiapi.c $SRC/module/network/netws.c -lpthread -o test_aiapi_stop${1:+_$1}
echo built test_aiapi_stop${1:+_$1}
