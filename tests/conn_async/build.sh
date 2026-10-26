#!/bin/sh
# Linux/gcc：conn_async.c 桩测试。用法：sh build.sh [tsan|asan]
set -e
cd "$(dirname "$0")"
SAN=""
case "$1" in tsan) SAN="-fsanitize=thread";; asan) SAN="-fsanitize=address,undefined";; esac
gcc -std=gnu99 -g -O1 -Wall -Wextra -Wno-unused-parameter $SAN -I../drv_reconnect/shim -I../../code/src -include pre_conn.h -I. \
    test_conn_async.c ../../code/src/conn_async.c -lpthread -o test_conn_async${1:+_$1}
gcc -std=gnu99 -g -O1 -Wall -Wextra -Wno-unused-parameter $SAN -I../drv_reconnect/shim -I../../code/src -include pre_conn.h -I. \
    test_conn_race.c ../../code/src/conn_async.c -lpthread -o test_conn_race${1:+_$1}
echo built test_conn_async${1:+_$1} test_conn_race${1:+_$1}
