"""ctypes probe: J-Link connect sequence diagnostics for STM32L432.

Run: python probe_connect.py
"""
import ctypes
import os
import sys


DLL = os.path.abspath("dll/JLink_x64.dll")
dll = ctypes.WinDLL(DLL)


def set_restype(name, rt):
    f = getattr(dll, name)
    f.restype = rt
    return f


def exec_cmd(cmd, bufsize=4096):
    buf = ctypes.create_string_buffer(bufsize)
    f = set_restype("JLINKARM_ExecCommand", ctypes.c_int)
    r = f(cmd.encode("ascii"), buf, bufsize)
    s = buf.value.decode("latin1", "replace")
    print("  EXEC %-32r -> rc=%d out=%r" % (cmd, r, s))
    return r, s


def main():
    openf = set_restype("JLINKARM_Open", ctypes.c_int)
    closef = set_restype("JLINKARM_Close", None)
    connf = set_restype("JLINKARM_Connect", ctypes.c_int)
    iscf = set_restype("JLINKARM_IsConnected", ctypes.c_int)
    sel = set_restype("JLINKARM_EMU_SelectByUSBSN", ctypes.c_int)
    speed = set_restype("JLINKARM_SetSpeed", ctypes.c_int)
    tif = set_restype("JLINKARM_TIF_Select", ctypes.c_int)

    print("open:", openf(None))
    print("select_by_usbsn(174504925):", sel(174504925))
    print("tif_select(SWD=1):", tif(1))
    exec_cmd("Device = STM32L432KB")
    print("speed(4000):", speed(4000))
    print("connect (device STM32L432KB, 4000):", connf())
    print("is_connected:", iscf())
    closef()

    print("--- retry device STM32L432KC ---")
    print("open:", openf(None))
    print("select_by_usbsn(174504925):", sel(174504925))
    print("tif_select(SWD=1):", tif(1))
    exec_cmd("Device = STM32L432KC")
    print("speed(4000):", speed(4000))
    print("connect (STM32L432KC):", connf())
    print("is_connected:", iscf())
    closef()

    print("--- retry device Cortex-M4 ---")
    print("open:", openf(None))
    print("select_by_usbsn(174504925):", sel(174504925))
    print("tif_select(SWD=1):", tif(1))
    exec_cmd("Device = Cortex-M4")
    print("speed(4000):", speed(4000))
    print("connect (Cortex-M4):", connf())
    print("is_connected:", iscf())
    closef()
    return 0


if __name__ == "__main__":
    sys.exit(main())
