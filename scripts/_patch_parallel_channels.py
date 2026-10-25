# -*- coding: utf-8 -*-
from pathlib import Path
import re

p = Path(r"D:\OpenScope\code\src\mainwin.c")
t = p.read_text(encoding="utf-8")

# Remove last CB_ADDSTRING before driver_index block (was 网络)
t2, n = re.subn(
    r"(\n\s*)SendMessageW\(h, CB_ADDSTRING, 0, \(LPARAM\)L\"[^\"]*\"\);[^\n]*\n(\s*\{\s*\n\s*int idx = os_modmgr_driver_index)",
    r"\n\2",
    t,
    count=1,
)
print("remove network combo", n)
t = t2

# Strip net_mode branch at start of cmd_select_driver
t2, n = re.subn(
    r"(static void cmd_select_driver\(int idx\)\s*\{\s*)"
    r"if \(idx == os_modmgr_driver_count\(\)\) \{.*?\n\s*return;\s*\}\s*",
    r"\1",
    t,
    count=1,
    flags=re.S,
)
print("cmd_select_driver", n)
t = t2

# connect: remove net_mode divert (best-effort multiline)
t2, n = re.subn(
    r"if \(g_app\.net_mode\) \{\s*cmd_net_action\(OS_CMD_NET_CONNECT\);\s*return;\s*\}\s*",
    "",
    t,
    count=1,
)
print("connect net_mode", n)
t = t2

t2, n = re.subn(
    r"if \(g_app\.net_mode\) \{\s*cmd_net_action\(OS_CMD_NET_STOP\);\s*return;\s*\}\s*",
    "",
    t,
    count=1,
)
print("disconnect net_mode", n)
t = t2

# Also remove acq remapping for net_mode if present later - leave for now (harmless if net_mode always 0)

old_layout = (
    "{ IDD_NET_IP, 96, 0 }, { IDD_NET_PORT, 44, 0 },\n"
    "            { IDD_NET_LISTEN, 0, 0 }, { IDD_NET_UPELF, 0, 0 }, { IDD_NET_DOWNELF, 0, 0 },"
)
new_layout = (
    "{ IDD_NET_IP, 96, 0 }, { IDD_NET_PORT, 44, 0 },\n"
    "            { IDD_NET_LISTEN, 0, 0 }, { IDD_NET_CONNECT, 0, 0 }, { IDD_NET_UPELF, 0, 0 }, { IDD_NET_DOWNELF, 0, 0 },\n"
    "            { IDC_XCP_TRANSPORT, 72, 1 }, { IDC_XCP_PORT, 72, 0 }, { IDC_XCP_BAUD, 56, 0 },\n"
    "            { IDC_XCP_SCAN, 0, 0 }, { IDC_XCP_CONNECT, 0, 0 }, { IDC_XCP_DISCONNECT, 0, 0 },"
)
if old_layout in t:
    t = t.replace(old_layout, new_layout, 1)
    print("layout ok")
else:
    print("layout MISSING")

old_ids = (
    "{ IDC_CFG_DRIVER, IDC_CFG_DEVICE, IDC_CFG_IFACE, IDC_CFG_SPEED, IDC_CFG_EMU, IDC_CFG_REFRESH,\n"
    "                          IDD_NET_IP, IDD_NET_PORT, IDD_NET_LISTEN, IDD_NET_UPELF, IDD_NET_DOWNELF }"
)
new_ids = (
    "{ IDC_CFG_DRIVER, IDC_CFG_DEVICE, IDC_CFG_IFACE, IDC_CFG_SPEED, IDC_CFG_EMU, IDC_CFG_REFRESH,\n"
    "                          IDD_NET_IP, IDD_NET_PORT, IDD_NET_LISTEN, IDD_NET_CONNECT, IDD_NET_UPELF, IDD_NET_DOWNELF,\n"
    "                          IDC_XCP_TRANSPORT, IDC_XCP_PORT, IDC_XCP_BAUD, IDC_XCP_SCAN, IDC_XCP_CONNECT, IDC_XCP_DISCONNECT }"
)
if old_ids in t:
    t = t.replace(old_ids, new_ids, 1)
    print("ids ok")
else:
    print("ids MISSING")

# Insert XCP control creation after network block (before SetParent)
marker = "IDD_NET_DOWNELF"
# Find network button create loop end - insert XCP create before SetParent block
needle = "/* 配置/网络控件挂到主命令栏 */"
# file may be garbled; find SetParent bar for net
m = re.search(
    r"(for \(k = 0; k < 3; k\+\+\) \{\s*c = CreateWindowW\(L\"BUTTON\", nb\[k\]\.t,.*?\n\s*\}\s*\}\s*\})",
    t,
    flags=re.S,
)
# Simpler: after CreateWindow for DOWNELF area, before SetParent of cfg
idx = t.find("IDD_NET_DOWNELF")
# find the SetParent block that lists IDD_NET
sp = t.find("IDD_NET_LISTEN, IDD_NET_UPELF")
if sp < 0:
    sp = t.find("IDC_XCP_TRANSPORT")  # already patched ids
print("sp", sp)

create_xcp = r'''
        /* XCP 并行通道（与探针/网络独立） */
        {
            HWND c;
            c = CreateWindowW(L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_TABSTOP,
                              1400, 5, 72, 120, hwnd, (HMENU)IDC_XCP_TRANSPORT, g_app.hInst, NULL);
            SendMessageW(c, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
            combo_install_theme(c);
            SendMessageW(c, CB_ADDSTRING, 0, (LPARAM)L"串口");
            SendMessageW(c, CB_ADDSTRING, 0, (LPARAM)L"CAN");
            SendMessageW(c, CB_ADDSTRING, 0, (LPARAM)L"TCP");
            SendMessageW(c, CB_SETCURSEL, 0, 0);
            c = CreateWindowW(L"EDIT", L"COM3", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                              1480, 5, 72, 21, hwnd, (HMENU)IDC_XCP_PORT, g_app.hInst, NULL);
            SendMessageW(c, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
            c = CreateWindowW(L"EDIT", L"115200", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
                              1560, 5, 56, 21, hwnd, (HMENU)IDC_XCP_BAUD, g_app.hInst, NULL);
            SendMessageW(c, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
            { static const struct { int id; const wchar_t* t; } xb[] = {
                { IDC_XCP_SCAN, L"XCP扫描" }, { IDC_XCP_CONNECT, L"XCP连接" }, { IDC_XCP_DISCONNECT, L"XCP断开" } };
              int k;
              for (k = 0; k < 3; k++) {
                c = CreateWindowW(L"BUTTON", xb[k].t, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                  1620 + k * 72, 5, 70, 24, hwnd, (HMENU)(INT_PTR)xb[k].id, g_app.hInst, NULL);
                SendMessageW(c, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
              }
            }
            c = CreateWindowW(L"BUTTON", L"网络连接", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                              0, 5, 72, 24, hwnd, (HMENU)IDD_NET_CONNECT, g_app.hInst, NULL);
            SendMessageW(c, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
        }
'''

if "IDC_XCP_TRANSPORT" in t and "CreateWindowW(L\"COMBOBOX\"" in t and t.count("IDC_XCP_TRANSPORT") < 3:
    # insert before SetParent block containing XCP ids
    m = re.search(r"/\*[^*]*\*/\s*\{\s*\n\s*HWND bar = os_dockbar_hwnd\(OS_DOCKBAR_MAIN\);", t)
    if not m:
        m = re.search(r"\{\s*\n\s*HWND bar = os_dockbar_hwnd\(OS_DOCKBAR_MAIN\);", t)
    if m and "XCP扫描" not in t:
        t = t[: m.start()] + create_xcp + "\n        " + t[m.start() :]
        print("created XCP controls")
    elif "XCP扫描" in t:
        print("XCP controls already")
    else:
        print("FAIL insert create")
else:
    if "XCP扫描" not in t:
        m = re.search(r"\{\s*\n\s*HWND bar = os_dockbar_hwnd\(OS_DOCKBAR_MAIN\);", t)
        if m:
            t = t[: m.start()] + create_xcp + "\n        " + t[m.start() :]
            print("created XCP controls (fallback)")
        else:
            print("FAIL no bar marker")
    else:
        print("skip create")

# Add command handlers - find IDD_NET_LISTEN case area
handler = '''
        case IDD_NET_CONNECT: cmd_net_action(OS_CMD_NET_CONNECT); break;
        case IDC_XCP_SCAN: cmd_xcp_scan(); break;
        case IDC_XCP_CONNECT: cmd_xcp_connect(); break;
        case IDC_XCP_DISCONNECT: cmd_xcp_disconnect(); break;
'''

if "cmd_xcp_connect" not in t:
    # insert functions before cmd_select_driver
    funcs = r'''
static const char* xcp_transport_name_ui(void)
{
    LRESULT i = SendMessageW(main_ctrl(IDC_XCP_TRANSPORT), CB_GETCURSEL, 0, 0);
    if (i == 1) return "can";
    if (i == 2) return "tcp";
    return "serial";
}

static void cmd_xcp_scan(void)
{
    OS_ScanReq req;
    OS_DeviceInfo items[32];
    int i;
    if (!g_app.xcpmod || !g_app.xcpmod->command) {
        os_log(OS_LOG_WARN, "XCP: 未加载模块");
        return;
    }
    memset(&req, 0, sizeof(req));
    req.items = items;
    req.capacity = 32;
    g_app.xcpmod->command(g_app.xcpmod_ctx, OS_CMD_SCAN, &req, NULL);
    os_log(OS_LOG_INFO, "XCP 扫描结果 %d 个", req.count);
    for (i = 0; i < req.count; i++)
        os_log(OS_LOG_INFO, "  [%d] %s (%s)", i, items[i].name, items[i].serial);
    if (req.count > 0 && items[0].name[0]) {
        /* serial:COMx -> 填端口框 */
        const char* p = strchr(items[0].name, ':');
        wchar_t w[64];
        os_utf8_to_wide_buf(p ? p + 1 : items[0].name, w, 64);
        SetWindowTextW(main_ctrl(IDC_XCP_PORT), w);
    }
}

static void cmd_xcp_connect(void)
{
    OS_ConnectCfg cfg;
    wchar_t wp[64], wb[32];
    char port8[64], baud8[32];
    int rc, c = 0;
    if (!g_app.xcpmod || !g_app.xcpmod->command) {
        os_log(OS_LOG_ERROR, "XCP: 未加载模块");
        return;
    }
    memset(&cfg, 0, sizeof(cfg));
    cfg.probe_index = 0;
    _snprintf(cfg.device, sizeof(cfg.device), "%s", xcp_transport_name_ui());
    GetWindowTextW(main_ctrl(IDC_XCP_PORT), wp, 64);
    GetWindowTextW(main_ctrl(IDC_XCP_BAUD), wb, 32);
    os_wide_to_utf8_buf(wp, port8, sizeof(port8));
    os_wide_to_utf8_buf(wb, baud8, sizeof(baud8));
    _snprintf(cfg.serial, sizeof(cfg.serial), "%s", port8);
    cfg.speed_khz = atoi(baud8);
    if (cfg.speed_khz <= 0) cfg.speed_khz = 115200;
    rc = g_app.xcpmod->command(g_app.xcpmod_ctx, OS_CMD_CONNECT, &cfg, NULL);
    if (rc != OS_ERR_OK) {
        os_log(OS_LOG_ERROR, "XCP 连接失败 rc=%d", rc);
        g_app.xcp_connected = 0;
        return;
    }
    g_app.xcpmod->command(g_app.xcpmod_ctx, OS_CMD_IS_CONNECTED, NULL, &c);
    g_app.xcp_connected = c;
    os_log(OS_LOG_INFO, "XCP 连接成功（与探针/网络并行）");
    refresh_status();
}

static void cmd_xcp_disconnect(void)
{
    if (!g_app.xcpmod || !g_app.xcpmod->command) return;
    g_app.xcpmod->command(g_app.xcpmod_ctx, OS_CMD_DISCONNECT, NULL, NULL);
    g_app.xcp_connected = 0;
    os_log(OS_LOG_INFO, "XCP 已断开");
    refresh_status();
}

'''
    m = re.search(r"static void cmd_select_driver\(int idx\)", t)
    if m:
        t = t[: m.start()] + funcs + t[m.start() :]
        print("funcs inserted")
    else:
        print("FAIL funcs")

if "case IDC_XCP_CONNECT:" not in t:
    m = re.search(r"case IDD_NET_LISTEN\s*:", t)
    if m:
        # insert after this case's break line
        br = t.find("break;", m.end())
        if br > 0:
            br = t.find("\n", br) + 1
            t = t[:br] + handler + t[br:]
            print("handlers inserted")
    else:
        # try IDD_NET_UPELF
        m = re.search(r"case IDD_NET_UPELF\s*:", t)
        if m:
            br = t.find("break;", m.end())
            br = t.find("\n", br) + 1
            t = t[:br] + handler + t[br:]
            print("handlers after UPELF")
        else:
            print("FAIL handlers")

# Ensure net_mode cleared on driver select
if "g_app.net_mode = 0;" not in t[t.find("cmd_select_driver") : t.find("cmd_select_driver") + 400]:
    t2, n = re.subn(
        r"(static void cmd_select_driver\(int idx\)\s*\{\s*)",
        r"\1g_app.net_mode = 0;\n    ",
        t,
        count=1,
    )
    print("force net_mode0", n)
    t = t2

# update_buttons: disable exclusive net_mode remapping by forcing condition false
# Safer: change `if (g_app.net_mode)` first branch in update_buttons to `if (0 && g_app.net_mode)`
t2, n = re.subn(
    r"(void os_mainwin_update_buttons\(void\)\s*\{.*?)\nif \(g_app\.net_mode\) \{",
    r"\1\nif (0 && g_app.net_mode) {",
    t,
    count=1,
    flags=re.S,
)
print("update_buttons disable net exclusive", n)
t = t2

p.write_text(t, encoding="utf-8", newline="\n")
print("saved", p)
