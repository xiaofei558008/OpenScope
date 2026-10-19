# MCU 端 XCP 接入说明

与主机 OpenScope `xcp.dll` 配套的精简从机与物理层示例。

## 目录

| 文件 | 说明 |
|------|------|
| `xcp_port.h` / `xcp_port.c` | 协议精简从机 + `XcpPort` 抽象 + 日志钩子 |
| `xcp_uart_example.c` | UART 成帧示例（`[u16 len][payload]`） |
| `xcp_can_example.c` | CAN 骨架（板级完成 ID/多帧） |
| `xcp_tcp_example.c` | TCP 成帧示例（`[u16 len][u16 ctr][payload]`） |

## 接入步骤

1. 把 `xcp_port.*` 与某一 `*_example.c` 加入固件工程。
2. 实现板级 `board_uart_*` / `board_can_*` / `board_tcp_*`。
3. `xcp_mcu_set_cal_mem(标定 RAM, 字节数)`。
4. 可选：`xcp_port_set_log(你的日志函数)`。
5. 主循环：`xcp_mcu_poll(xcp_uart_port())`。

## 主机侧

工具栏 **XCP** 区：传输选「串口」，端口 `COMx`，波特率与 MCU 一致，点「XCP连接」。

探针（J-Link/ST-Link）与网络可同时连接，互不抢通道。
