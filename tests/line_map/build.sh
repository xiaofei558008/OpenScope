#!/bin/sh
# Linux/gcc：真实 elf.c + dwloc.c。用法：./build.sh [tsan|asan]
set -e
cd "$(dirname "$0")"
SRC=../..
SAN=""
case "$1" in tsan) SAN="-fsanitize=thread";; asan) SAN="-fsanitize=address,undefined";; esac
[ -f line_sample.out ] || python3 gen_line_elf.py line_sample.out
gcc -std=gnu99 -g -O1 -Wall -Wno-unused-parameter $SAN -I$SRC/code/src test_line_map.c $SRC/code/src/elf.c $SRC/code/src/dwloc.c -lm -o test_line_map${1:+_$1}
echo built test_line_map${1:+_$1}
