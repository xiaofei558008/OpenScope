#!/bin/sh
# Linux/CI 构建与测试（Windows 端走 xcp.vcxproj；新增文件需加入工程，见补丁 README）
set -e
cd "$(dirname "$0")"
CC=${CC:-gcc}
CF="-std=c99 -Wall -Wextra -Werror -O1 -g"
SAN=${SAN:--fsanitize=address,undefined}
mkdir -p build
$CC $CF $SAN -D_POSIX_C_SOURCE=200809L -o build/xcp_mock_slave xcp/xcp_mock_main.c xcp/xcp_mock.c xcp/xcp_slave.c -lm
$CC $CF $SAN -Ixcp -o build/xcp_mock_test xcp/tests/xcp_mock_test.c xcp/xcp_mock.c xcp/xcp_slave.c
$CC $CF $SAN -include stddef.h -Ixcp -o build/xcp_smoke xcp/tests/xcp_smoke.c xcp/tests/posix_stubs.c \
    xcp/xcp_master.c xcp/xcp_slave.c xcp/xcp_can.c xcp/xcp_transport.c
$CC $CF $SAN -o build/a2l_test a2l/tests/a2l_test.c a2l/a2l_parse.c -lm
build/xcp_smoke
build/xcp_mock_test
build/a2l_test a2l/tests/sample.a2l
python3 xcp/tests/mock_tcp_test.py build/xcp_mock_slave
echo "ALL TESTS PASSED"
