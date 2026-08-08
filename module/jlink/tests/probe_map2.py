"""Fresh-connection-per-address probe of RAM write and Flash read boundaries."""
import ctypes
import os
import sys


DLL = os.path.abspath("dll/JLink_x64.dll")
dll = ctypes.WinDLL(DLL)


def connect():
    for name, rt in [
        ("JLINKARM_Open", ctypes.c_int),
        ("JLINKARM_TIF_Select", ctypes.c_int),
        ("JLINKARM_SetSpeed", ctypes.c_int),
        ("JLINKARM_Connect", ctypes.c_int),
        ("JLINKARM_Halt", ctypes.c_int),
        ("JLINKARM_ReadMem", ctypes.c_int),
        ("JLINKARM_WriteMem", ctypes.c_int),
        ("JLINKARM_Close", None),
    ]:
        getattr(dll, name).restype = rt
    dll.JLINKARM_Open(None)
    dll.JLINKARM_TIF_Select(1)
    buf = ctypes.create_string_buffer(4096)
    dll.JLINKARM_ExecCommand(b"Device = STM32L432KB", buf, 4096)
    dll.JLINKARM_SetSpeed(4000)
    dll.JLINKARM_Connect()
    dll.JLINKARM_Halt()


def close():
    dll.JLINKARM_Close()


def ram_test(addr):
    connect()
    v = ctypes.c_uint32(0x1234A000 | (addr & 0xFFF))
    wr = dll.JLINKARM_WriteMem(addr, 4, ctypes.byref(v))
    got = ctypes.c_uint32(0)
    rd = dll.JLINKARM_ReadMem(addr, 4, ctypes.byref(got))
    ok = wr == 4 and rd == 0 and got.value == v.value
    print("RAM  0x%08X wr=%d rd=%d got=0x%08X %s" %
          (addr, wr, rd, got.value, "OK" if ok else "FAIL"), flush=True)
    close()


def flash_test(addr):
    connect()
    b = ctypes.create_string_buffer(4)
    rd = dll.JLINKARM_ReadMem(addr, 4, b)
    ok = rd == 0
    print("FLASH 0x%08X rd=%d data=%s %s" %
          (addr, rd, b.raw.hex(), "OK" if ok else "FAIL"), flush=True)
    close()


def main():
    print("--- RAM write/read boundary ---", flush=True)
    for a in [0x20001000, 0x20001400, 0x20001800, 0x20001C00,
              0x20001FFC, 0x20002000, 0x20003FFC, 0x20004000,
              0x20007FFC, 0x20008000]:
        ram_test(a)
    print("--- Flash read boundary ---", flush=True)
    for a in [0x08000000, 0x0800FFFC, 0x08010000, 0x0801FFFC, 0x08020000]:
        flash_test(a)
    print("--- CPUID / SRAM2 / misc ---", flush=True)
    for a in [0xE000ED00, 0x10000000, 0x10001000, 0x10003FFC, 0x20004000]:
        ram_test(a)
    return 0


if __name__ == "__main__":
    sys.exit(main())
