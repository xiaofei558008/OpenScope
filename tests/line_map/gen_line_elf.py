"""第五轮：生成 line_sample.out —— 最小 ELF32（只有 .debug_line，DWARF 3），复现 TC375 实板 ELF 的行表形态：
  - 序列 A 的 end_sequence 地址 == 序列 B 的首地址（旧代码把结束行当真实行：B 的首行 PC→行 报成 A 的末行）
  - 零长度行（同地址先后两行：14 的代码被并入 16）
  - 非 is_stmt 行（17 先有一条 non-stmt）
  - 空行 12/15（无记录）→ 应取同一函数内下一条有代码的行
  - 序列外地址（0x80001020 之后）→ 无行
用法：python gen_line_elf.py [out]"""
import struct, sys

def uleb(n):
    out = bytearray()
    while True:
        b = n & 0x7F; n >>= 7
        out.append(b | 0x80 if n else b)
        if not n: return bytes(out)

def sleb(n):
    out = bytearray()
    while True:
        b = n & 0x7F; n >>= 7
        done = (n == 0 and not (b & 0x40)) or (n == -1 and (b & 0x40))
        out.append(b if done else b | 0x80)
        if done: return bytes(out)

DW_LNS_copy, DW_LNS_advance_pc, DW_LNS_advance_line, DW_LNS_negate_stmt = 1, 2, 3, 6

class Prog:
    def __init__(s): s.b = bytearray(); s.line = 1; s.addr = 0
    def set_addr(s, a): s.b += bytes([0, 5, 2]) + struct.pack('<I', a); s.addr = a
    def row(s, line, addr):
        if addr != s.addr: s.b += bytes([DW_LNS_advance_pc]) + uleb((addr - s.addr) // 2); s.addr = addr
        if line != s.line: s.b += bytes([DW_LNS_advance_line]) + sleb(line - s.line); s.line = line
        s.b += bytes([DW_LNS_copy])
    def negate(s): s.b += bytes([DW_LNS_negate_stmt])
    def end(s, addr):
        if addr != s.addr: s.b += bytes([DW_LNS_advance_pc]) + uleb((addr - s.addr) // 2); s.addr = addr
        s.b += bytes([0, 1, 1]); s.line = 1

p = Prog()
A = 0x80001000
p.set_addr(A)
p.row(10, A + 0x0); p.row(11, A + 0x4); p.row(13, A + 0x8)
p.row(14, A + 0xC); p.row(16, A + 0xC)            # 14 零长度，指令归 16
p.negate(); p.row(17, A + 0xE); p.negate()         # 17 的 non-stmt 行
p.row(17, A + 0x10)
p.end(A + 0x14)                                    # 序列 A 结束 @0x80001014
p.set_addr(A + 0x14)                               # 序列 B 从同一地址开始
p.row(30, A + 0x14); p.row(31, A + 0x18)
p.end(A + 0x20)

hdr = bytearray()
hdr += bytes([2, 1, 0xFB & 0xFF, 14, 13])          # min_inst=2, default_is_stmt=1, line_base=-5, line_range=14, opcode_base=13
hdr[2] = (-5) & 0xFF
hdr += bytes([0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1]) # std_opcode_lengths
hdr += b'\0'                                       # include_directories: 空
hdr += b'main.c\0' + uleb(0) + uleb(0) + uleb(0) + b'\0'
body = struct.pack('<H', 3) + struct.pack('<I', len(hdr)) + hdr + bytes(p.b)
debug_line = struct.pack('<I', len(body)) + body

# .symtab：一个全局变量（os_elf_open 要求至少有一个变量）
strtab = b'\0g_dummy\0'
symtab = bytes(16) + struct.pack('<IIIBBH', 1, 0x70000000, 4, 0x11, 0, 0xFFF1)   # STB_GLOBAL|STT_OBJECT, SHN_ABS
shstr = b'\0.debug_line\0.shstrtab\0.symtab\0.strtab\0'
off_line = 52
off_sym = off_line + len(debug_line); off_sym = (off_sym + 3) & ~3
off_str = off_sym + len(symtab)
off_shstr = off_str + len(strtab)
off_sh = (off_shstr + len(shstr) + 3) & ~3
eh = b'\x7fELF' + bytes([1, 1, 1, 0]) + bytes(8)
eh += struct.pack('<HHIIIIIHHHHHH', 2, 44, 1, 0, 0, off_sh, 0, 52, 0, 0, 40, 5, 2)   # ET_EXEC, EM_TRICORE(44)
def sh(name, typ, off, size, link=0, info=0, ent=0, align=1): return struct.pack('<IIIIIIIIII', name, typ, 0, 0, off, size, link, info, align, ent)
img = bytearray(eh) + debug_line
img += bytes(off_sym - len(img)) + symtab + strtab + shstr
img += bytes(off_sh - len(img))
img += bytes(40) + sh(1, 1, off_line, len(debug_line)) + sh(13, 3, off_shstr, len(shstr)) \
     + sh(23, 2, off_sym, len(symtab), link=4, info=1, ent=16, align=4) + sh(31, 3, off_str, len(strtab))
out = sys.argv[1] if len(sys.argv) > 1 else 'line_sample.out'
open(out, 'wb').write(img)
print('wrote', out, len(img), 'bytes')
