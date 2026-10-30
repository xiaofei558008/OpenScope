#!/bin/sh
# 在 Linux 上构建并运行单测：plain / asan / tsan，以及 mingw 交叉编译检查。
set -e
cd "$(dirname "$0")/.."
OUT=${OUT:-/tmp/xcpA}
mkdir -p "$OUT"
SRC="xcp_plat.c xcp_eth.c xcp_session.c tests/xcp_session_test.c tests/xcp_fake_slave.c"
W="-std=c99 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wvla -pedantic -Werror"
gcc $W -O2 -g $SRC -o "$OUT/t_plain" -lpthread
gcc $W -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer $SRC -o "$OUT/t_asan" -lpthread
gcc $W -O1 -g -fsanitize=thread $SRC -o "$OUT/t_tsan" -lpthread
if command -v x86_64-w64-mingw32-gcc >/dev/null; then
  x86_64-w64-mingw32-gcc -std=c99 -Wall -Wextra -Wvla -Werror -O2 $SRC -o "$OUT/t_win.exe" -lws2_32
fi
for v in plain asan tsan; do echo "== $v"; "$OUT/t_$v" | tail -1; done
