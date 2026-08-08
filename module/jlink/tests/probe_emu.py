"""ctypes probe for JLink_x64.dll emulator-enumeration APIs.

Usage: python probe_emu.py <test>
Tests:
  num         JLINKARM_EMU_GetNumDevices()            (no args)
  legacy2     JLINKARM_EMU_GetDeviceInfo(0, buf)      (2 args)
  new3        JLINK_EMU_GetDeviceInfo(0, 0, buf)      (3 args)
  new4        JLINK_EMU_GetDeviceInfo(0, 0, buf, 4096) (4 args)
  sel         JLINKARM_EMU_SelectByIndex(0)
  num_open    same as num, but after JLINKARM_Open(NULL)
  legacy2_open / new3_open / new4_open   after JLINKARM_Open(NULL)
  list        JLINKARM_EMU_GetList(USB_OR_IP=3, NULL, 0) then fill
  list_usb    same with host=USB (1)

Each test runs in its own process so a bad signature only crashes that probe.
"""
import ctypes
import os
import sys


DLL_PATH = os.path.abspath("dll/JLink_x64.dll")


class JLinkConnectInfo(ctypes.Structure):
    _fields_ = [
        ("SerialNumber", ctypes.c_uint32),
        ("Connection", ctypes.c_ubyte),
        ("sUSBAddr", ctypes.c_ubyte * 7),
        ("sIPAddr", ctypes.c_char * 16),
        ("Time", ctypes.c_int),
        ("Time_us", ctypes.c_uint64),
        ("HWVersion", ctypes.c_uint32),
        ("abMACAddr", ctypes.c_ubyte * 6),
        ("acProduct", ctypes.c_char * 32),
        ("acNickname", ctypes.c_char * 32),
        ("acFWString", ctypes.c_char * 112),
        ("IsDHCPAssignedIP", ctypes.c_ubyte),
        ("IsDHCPAssignedIPIsValid", ctypes.c_ubyte),
        ("NumIPConnections", ctypes.c_ubyte),
        ("NumIPConnectionsIsValid", ctypes.c_ubyte),
        ("aPadding", ctypes.c_ubyte * 34),
    ]


def open_dll(dll):
    f = dll.JLINKARM_Open
    f.restype = ctypes.c_int
    print("call JLINKARM_Open(NULL)")
    sys.stdout.flush()
    rc = f(None)
    print("open rc =", rc)
    sys.stdout.flush()
    return rc


def dump(tag, buf, n=512):
    raw = buf.raw[:n]
    print(tag + " hex:", raw.hex(" "))
    print(tag + " ascii:", "".join(chr(c) if 32 <= c < 127 else "." for c in raw))


def main():
    test = sys.argv[1] if len(sys.argv) > 1 else "num"
    dll = ctypes.WinDLL(DLL_PATH)
    print("dll:", DLL_PATH)
    do_open = test.endswith("_open")
    base = test[:-5] if do_open else test
    if do_open:
        open_dll(dll)

    if base == "num":
        f = dll.JLINKARM_EMU_GetNumDevices
        f.restype = ctypes.c_uint32
        print("call JLINKARM_EMU_GetNumDevices()")
        sys.stdout.flush()
        print("rc =", f())
        return 0

    if base == "legacy2":
        f = dll.JLINKARM_EMU_GetDeviceInfo
        f.restype = ctypes.c_int
        buf = ctypes.create_string_buffer(4096)
        print("call JLINKARM_EMU_GetDeviceInfo(0, buf)")
        sys.stdout.flush()
        rc = f(0, buf)
        print("rc =", rc)
        dump("legacy2", buf)
        return 0

    if base == "new3":
        f = dll.JLINK_EMU_GetDeviceInfo
        f.restype = ctypes.c_int
        buf = ctypes.create_string_buffer(4096)
        print("call JLINK_EMU_GetDeviceInfo(0, 0, buf)")
        sys.stdout.flush()
        rc = f(0, 0, buf)
        print("rc =", rc)
        dump("new3", buf)
        return 0

    if base == "new4":
        f = dll.JLINK_EMU_GetDeviceInfo
        f.restype = ctypes.c_int
        buf = ctypes.create_string_buffer(4096)
        print("call JLINK_EMU_GetDeviceInfo(0, 0, buf, 4096)")
        sys.stdout.flush()
        rc = f(0, 0, buf, 4096)
        print("rc =", rc)
        dump("new4", buf)
        return 0

    if base == "sel":
        f = dll.JLINKARM_EMU_SelectByIndex
        f.restype = ctypes.c_int
        print("call JLINKARM_EMU_SelectByIndex(0)")
        sys.stdout.flush()
        print("rc =", f(0))
        return 0

    if base in ("list", "list_usb"):
        host = 1 if base == "list_usb" else 3
        f = dll.JLINKARM_EMU_GetList
        f.restype = ctypes.c_int
        print("sizeof(JLinkConnectInfo) =", ctypes.sizeof(JLinkConnectInfo))
        print("call JLINKARM_EMU_GetList(host=%d, NULL, 0)" % host)
        sys.stdout.flush()
        n = f(host, None, 0)
        print("first rc =", n)
        sys.stdout.flush()
        if n <= 0:
            return 0
        cap = min(n, 8)
        arr = (JLinkConnectInfo * cap)()
        print("call JLINKARM_EMU_GetList(host=%d, arr, %d)" % (host, cap))
        sys.stdout.flush()
        n2 = f(host, arr, cap)
        print("second rc =", n2)
        for i in range(cap):
            it = arr[i]
            print(
                "  [%d] SN=%u conn=%u usb=%s product=%r nick=%r fw=%r"
                % (
                    i,
                    it.SerialNumber,
                    it.Connection,
                    bytes(it.sUSBAddr).hex(),
                    it.acProduct.decode("latin1").rstrip("\x00"),
                    it.acNickname.decode("latin1").rstrip("\x00"),
                    it.acFWString.decode("latin1").rstrip("\x00"),
                )
            )
        return 0

    print("unknown test", test)
    return 2


if __name__ == "__main__":
    sys.exit(main())
