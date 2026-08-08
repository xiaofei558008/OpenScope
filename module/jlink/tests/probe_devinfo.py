"""Query J-Link device info (flash/RAM layout) for STM32L432KB."""
import ctypes
import os
import sys


DLL = os.path.abspath("dll/JLink_x64.dll")
dll = ctypes.WinDLL(DLL)


class JLinkFlashArea(ctypes.Structure):
    _fields_ = [("Addr", ctypes.c_uint32), ("Size", ctypes.c_uint32)]


class JLinkRAMArea(JLinkFlashArea):
    pass


class JLinkDeviceInfo(ctypes.Structure):
    _fields_ = [
        ("SizeofStruct", ctypes.c_uint32),
        ("sName", ctypes.POINTER(ctypes.c_char)),
        ("CoreId", ctypes.c_uint32),
        ("FlashAddr", ctypes.c_uint32),
        ("RAMAddr", ctypes.c_uint32),
        ("EndianMode", ctypes.c_char),
        ("FlashSize", ctypes.c_uint32),
        ("RAMSize", ctypes.c_uint32),
        ("sManu", ctypes.POINTER(ctypes.c_char)),
        ("aFlashArea", JLinkFlashArea * 32),
        ("aRAMArea", JLinkRAMArea * 32),
        ("Core", ctypes.c_uint32),
    ]


def main():
    dll.JLINKARM_DEVICE_GetIndex.restype = ctypes.c_int
    dll.JLINKARM_DEVICE_GetInfo.restype = ctypes.c_int
    idx = dll.JLINKARM_DEVICE_GetIndex(b"STM32L432KB")
    print("index:", idx, flush=True)
    if idx <= 0:
        return 1
    info = JLinkDeviceInfo()
    info.SizeofStruct = ctypes.sizeof(info)
    r = dll.JLINKARM_DEVICE_GetInfo(idx, ctypes.byref(info))
    print("GetInfo rc:", r, flush=True)
    print("name:", ctypes.string_at(info.sName).decode(), flush=True)
    print("FlashAddr: 0x%08X FlashSize: %d (0x%X)" %
          (info.FlashAddr, info.FlashSize, info.FlashSize), flush=True)
    print("RAMAddr: 0x%08X RAMSize: %d (0x%X)" %
          (info.RAMAddr, info.RAMSize, info.RAMSize), flush=True)
    for i in range(32):
        fa = info.aFlashArea[i]
        if fa.Size:
            print("  flash area[%d] 0x%08X size 0x%X" %
                  (i, fa.Addr, fa.Size), flush=True)
    for i in range(32):
        ra = info.aRAMArea[i]
        if ra.Size:
            print("  ram area[%d] 0x%08X size 0x%X" %
                  (i, ra.Addr, ra.Size), flush=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())
