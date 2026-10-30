#!/bin/sh
# Linux 上构建并运行全部单测：plain / ASan+UBSan / TSan；迁移前后对比；mingw -Werror 交叉编译。
# 用法：tests/build.sh [原始代码目录，默认 ../../code/xcp 或 $ORIG]
set -e
cd "$(dirname "$0")/.."
HERE=$(pwd)
ORIG=${ORIG:-${1:-$HERE/../../code/xcp}}
OUT=${OUT:-/tmp/xcpA}
mkdir -p "$OUT"
CORE="xcp_plat.c xcp_eth.c xcp_session.c xcp_checksum.c"
LEG="xcp_master.c xcp_slave.c xcp_transport.c xcp_can.c tests/legacy_posix_tr.c"
W="-std=c99 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wvla -pedantic -Werror"
SHIM="-Itests/shim -I.."
build() { # $1 后缀 $2 选项
  gcc $W $2 $CORE tests/xcp_session_test.c tests/xcp_fake_slave.c -o "$OUT/t_$1" -lpthread
  gcc $W $2 $CORE $LEG tests/xcp_smoke.c -o "$OUT/smoke_$1" -lpthread
  # xcp.c 依赖 Win32 头：用 tests/shim 的最小映射在 Linux 上跑（不进产品构建）
  gcc -std=gnu99 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wvla -Werror -Wno-unused-parameter -Wno-format-truncation $2 -DXCP_NEW $SHIM \
      $CORE $LEG xcp.c tests/xcp_plugin_test.c tests/xcp_fake_slave.c -o "$OUT/plugin_$1" -lpthread
}
build plain "-O2 -g"
build asan "-O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer -fno-sanitize-recover=undefined"
build tsan "-O1 -g -fsanitize=thread"

# 迁移前基线：原 xcp.c / xcp_master.c / smoke（同一份 plugin 测试、假从机）
B="$OUT/orig"; rm -rf "$B"; mkdir -p "$B/xcp/tests"
cp "$ORIG"/*.c "$ORIG"/*.h "$B/xcp/"; cp "$ORIG/tests/xcp_smoke.c" "$B/xcp/tests/"
cp xcp_plat.c xcp_plat.h xcp_checksum.c xcp_checksum.h "$B/xcp/"
cp -r tests/shim tests/legacy_posix_tr.c tests/xcp_fake_slave.c tests/xcp_fake_slave.h tests/xcp_plugin_test.c "$B/xcp/tests/"
cp ../module_api.h "$B/"
( cd "$B/xcp" && gcc -std=gnu99 -D_POSIX_C_SOURCE=200809L -O2 -g -w -include stddef.h -I. tests/xcp_smoke.c xcp_master.c xcp_slave.c xcp_transport.c xcp_can.c tests/legacy_posix_tr.c -o "$OUT/smoke_orig" \
  && gcc -std=gnu99 -D_POSIX_C_SOURCE=200809L -O2 -g -w -include stddef.h -Itests/shim -I.. xcp_plat.c xcp_checksum.c xcp.c xcp_master.c xcp_transport.c xcp_can.c tests/legacy_posix_tr.c tests/xcp_plugin_test.c tests/xcp_fake_slave.c -o "$OUT/plugin_orig" -lpthread )

if command -v x86_64-w64-mingw32-gcc >/dev/null; then
  MW="-std=c99 -Wall -Wextra -Wvla -Werror -O2"
  x86_64-w64-mingw32-gcc $MW $CORE tests/xcp_session_test.c tests/xcp_fake_slave.c -o "$OUT/t_win.exe" -lws2_32 -lwinmm
  x86_64-w64-mingw32-gcc $MW -Wno-unknown-pragmas -I.. -shared $CORE xcp.c xcp_master.c xcp_transport.c xcp_serial.c xcp_tcp.c xcp_can.c xcp_slave.c -o "$OUT/xcp.dll" -lws2_32
  x86_64-w64-mingw32-gcc $MW -Wno-unknown-pragmas -I. $CORE xcp_master.c xcp_slave.c xcp_transport.c xcp_serial.c xcp_tcp.c xcp_can.c tests/xcp_smoke.c -o "$OUT/smoke_win.exe" -lws2_32
  echo "mingw: OK"
fi

for v in plain asan tsan; do
  echo "== $v"
  "$OUT/t_$v" > "$OUT/t_$v.log" 2>&1; tail -1 "$OUT/t_$v.log"
  "$OUT/smoke_$v" > "$OUT/smoke_$v.log" 2>&1; tail -1 "$OUT/smoke_$v.log"
  "$OUT/plugin_$v" > "$OUT/plugin_$v.log" 2>&1; tail -1 "$OUT/plugin_$v.log"
  grep -c "WARNING: ThreadSanitizer\|ERROR: AddressSanitizer\|runtime error" "$OUT/t_$v.log" "$OUT/smoke_$v.log" "$OUT/plugin_$v.log" || true
done
echo "== 迁移前后对比"
"$OUT/smoke_orig" > "$OUT/smoke_orig.log" 2>&1 || true
"$OUT/plugin_orig" > "$OUT/plugin_orig.log" 2>&1 || true
grep -E "^(PASS|FAIL) " "$OUT/smoke_orig.log" > "$OUT/smoke_orig.steps"; grep -E "^(PASS|FAIL) " "$OUT/smoke_plain.log" > "$OUT/smoke_new.steps"
grep "^STEP" "$OUT/plugin_orig.log" > "$OUT/plugin_orig.steps"; grep "^STEP" "$OUT/plugin_plain.log" | grep -v "STEP NEW " > "$OUT/plugin_new.steps"
if diff -u "$OUT/smoke_orig.steps" "$OUT/smoke_new.steps"; then echo "smoke: 迁移前后 $(wc -l < "$OUT/smoke_new.steps") 条结果一致"; else echo "smoke: 有差异"; fi
if diff -u "$OUT/plugin_orig.steps" "$OUT/plugin_new.steps"; then echo "plugin: 迁移前后 $(wc -l < "$OUT/plugin_new.steps") 步结果一致"; else echo "plugin: 有差异"; fi
grep "INFO" "$OUT/plugin_orig.log" "$OUT/plugin_plain.log" || true
