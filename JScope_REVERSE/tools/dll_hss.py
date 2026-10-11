#!/usr/bin/env python3
"""Targeted disassembly of JLINK_HSS_* / RTT exports inside JLink_x64.dll.

Goal: recover the real semantics of
  - JLINK_HSS_Start(blocks, n, period_us, flags)  -> what does `flags` do?
  - JLINK_HSS_GetCaps(&caps8)                     -> what are the 8 bytes?
and locate the code that emits the "HSS started: ..." log line.

Usage: python dll_hss.py <ExportName> [maxInsns] [--follow]   (--follow also disassembles call targets)
"""
import sys
import pefile
from capstone import Cs, CS_ARCH_X86, CS_MODE_64
from capstone.x86 import X86_OP_MEM, X86_OP_IMM, X86_REG_RIP

DLL = r'C:\Program Files\SEGGER\JLink_V966\JLink_x64.dll'


def load():
    pe = pefile.PE(DLL, fast_load=True)
    pe.parse_data_directories(directories=[pefile.DIRECTORY_ENTRY['IMAGE_DIRECTORY_ENTRY_EXPORT']])
    base = pe.OPTIONAL_HEADER.ImageBase
    exports = {}
    for e in pe.DIRECTORY_ENTRY_EXPORT.symbols:
        if e.name:
            exports[e.name.decode()] = base + e.address
    return pe, base, exports


def disasm(pe, base, va, max_insns=200, stop_at_ret=True):
    md = Cs(CS_ARCH_X86, CS_MODE_64)
    md.detail = True
    off = pe.get_offset_from_rva(va - base)
    data = pe.__data__[off:off + max_insns * 16]
    out = []
    n = 0
    for insn in md.disasm(data, va):
        out.append(insn)
        n += 1
        if n >= max_insns:
            break
        if stop_at_ret and insn.mnemonic.startswith('ret'):
            break
    return out


def annotate(insn):
    notes = []
    for op in insn.operands:
        if op.type == X86_OP_MEM and op.mem.base == X86_REG_RIP:
            notes.append(f'-> 0x{insn.address + insn.size + op.mem.disp:x}')
        elif op.type == X86_OP_IMM and insn.mnemonic in ('call', 'jmp'):
            notes.append(f'call/jmp 0x{op.imm:x}')
    return ('   ; ' + ', '.join(notes)) if notes else ''


def main():
    name = sys.argv[1]
    max_insns = int(sys.argv[2]) if len(sys.argv) > 2 else 200
    pe, base, exports = load()
    if name.lower().startswith('0x'):
        va = int(name, 16)
    else:
        va = exports.get(name)
    if va is None:
        cands = [k for k in exports if name.lower() in k.lower()]
        print('not found; candidates:', cands[:20])
        return 1
    print(f'=== {name} @ 0x{va:x} ===')
    for insn in disasm(pe, base, va, max_insns):
        print(f'  0x{insn.address:x}  {insn.mnemonic:8s} {insn.op_str}{annotate(insn)}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
