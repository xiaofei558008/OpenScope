#!/bin/sh
# Linux/gcc：真实 jlink.c / stlink.c + 假厂商 DLL（../drv_probe）+ cmd_async.c。用法：sh build.sh [tsan|asan]
set -e
cd "$(dirname "$0")"
SRC=../..
P=../drv_probe
SAN=""
case "$1" in tsan) SAN="-fsanitize=thread";; asan) SAN="-fsanitize=address,undefined";; esac
CF="-std=gnu99 -g -O1 -Wall -Wextra -Wno-cast-function-type -Wno-unused-parameter -Wno-format $SAN -I../drv_reconnect/shim -I$P -I. -I$SRC/code/src"
O=obj${1:+_$1}; mkdir -p $O
gcc $CF -include pre_cmd.h -c $SRC/code/src/cmd_async.c -o $O/cmd_async.o
for d in jlink stlink; do
  D=$(echo $d | tr a-z A-Z)
  gcc $CF -include pre_probe.h -DPROBE_$D -I$SRC/module/$d test_stepout.c $P/fake_common.c $P/fake_$d.c $SRC/module/$d/$d.c \
      $O/cmd_async.o -lpthread -o test_stepout_$d${1:+_$1}
  gcc $CF -include pre_probe.h -DPROBE_$D -I$SRC/module/$d test_ai_step.c $SRC/code/src/ai_step.c $P/fake_common.c $P/fake_$d.c $SRC/module/$d/$d.c \
      $O/cmd_async.o -lpthread -o test_ai_step_$d${1:+_$1}
done
rm -rf $O
echo built test_stepout_{jlink,stlink}${1:+_$1} test_ai_step_{jlink,stlink}${1:+_$1}
