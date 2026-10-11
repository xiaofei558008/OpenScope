#!/usr/bin/env python3
"""Whole-image RIP-relative reference scanner for JScope.exe.

Finds every instruction that references a given absolute address via RIP-relative
addressing (Ghidra misses some of these when the referrer isn't a recognized function),
and classifies read vs write.

Usage:
  python rip_scan.py 0x14285eb9c 0x14085d1b0 ...          # specific targets
  python rip_scan.py --range 0x1400a5d1f0 0x14285d1f0     # any target inside range
"""
import sys
import pefile
from capstone import Cs, CS_ARCH_X86, CS_MODE_64
from capstone.x86 import X86_OP_MEM, X86_REG_RIP

EXE = r'C:\Program Files\SEGGER\JLink_V966\JScope.exe'


def handle(insn, hits, targets, ranges):
    for i, op in enumerate(insn.operands):
        if op.type != X86_OP_MEM or op.mem.base != X86_REG_RIP:
            continue
        tgt = insn.address + insn.size + op.mem.disp
        inside = any(lo <= tgt < hi for lo, hi in ranges)
        if not inside and tgt not in targets:
            continue
        read_only_mnemonics = ('cmp', 'test', 'push', 'call', 'jmp')
        if i == 0 and not insn.mnemonic.startswith(read_only_mnemonics):
            kind = 'W'
        elif i == 0:
            kind = 'R'
        else:
            kind = 'R' if i == 1 else f'op{i}'
        hits.append((tgt, insn.address, kind, insn.mnemonic, insn.op_str))


def main():
    args = sys.argv[1:]
    ranges = []
    targets = set()
    if args and args[0] == '--range':
        lo, hi = int(args[1], 16), int(args[2], 16)
        ranges.append((lo, hi))
    else:
        targets = {int(a, 16) for a in args}

    pe = pefile.PE(EXE)
    base = pe.OPTIONAL_HEADER.ImageBase
    text = next(s for s in pe.sections if s.Name.rstrip(b'\0') == b'.text')
    data = text.get_data()
    va = base + text.VirtualAddress

    md = Cs(CS_ARCH_X86, CS_MODE_64)
    md.detail = True

    # linear sweep with resync: capstone stops at the first undecodable byte (in-code data,
    # jump tables, padding), so restart one byte later and keep going.
    hits = []
    off = 0
    total = 0
    while off < len(data):
        progressed = False
        for insn in md.disasm(data[off:], va + off):
            progressed = True
            total += 1
            off = insn.address + insn.size - va
            handle(insn, hits, targets, ranges)
        if not progressed:
            off += 1
    print(f'# disassembled {total} instructions', file=sys.stderr)

    # group by target
    by = {}
    for tgt, a, kind, mn, ops in hits:
        by.setdefault(tgt, []).append((a, kind, mn, ops))
    for tgt in sorted(by):
        print(f"\n=== target 0x{tgt:x}  ({len(by[tgt])} refs) ===")
        for a, kind, mn, ops in sorted(by[tgt]):
            print(f"  {kind}  0x{a:x}  {mn} {ops}")
    if not by:
        print("no references found")


if __name__ == '__main__':
    main()
