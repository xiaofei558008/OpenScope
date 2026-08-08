"""Generate tests/elf_sample.out: a minimal ARM ELF32 (.out) with DWARF4.

Contents:
  - g_counter : int        @ 0x20000000 (DWARF + symtab)
  - g_cfg     : struct cfg_t { int a; int b; } @ 0x20000010 (DWARF + symtab)
  - g_raw     : 4 bytes    @ 0x20000020 (symtab only, no DWARF)
  - main      : function   @ 0x08000000 (ignored by parser)

Used by tests/elf_smoke.c to verify .out parsing (globals + struct expansion).
"""
import struct
from pathlib import Path


def uleb(n):
    out = bytearray()
    while True:
        b = n & 0x7F
        n >>= 7
        if n:
            out.append(b | 0x80)
        else:
            out.append(b)
            return bytes(out)


def die(code, payload):
    return uleb(code) + payload


# ---------------- DWARF abbrev table ----------------
abbrev = bytearray()
abbrev += uleb(1) + uleb(0x11) + b"\x01"  # CU, has children
abbrev += uleb(0x03) + uleb(0x08)          # DW_AT_name       DW_FORM_string
abbrev += uleb(0x13) + uleb(0x05)          # DW_AT_language    DW_FORM_data2
abbrev += b"\x00\x00"
abbrev += uleb(2) + uleb(0x24) + b"\x00"  # base_type
abbrev += uleb(0x03) + uleb(0x08)          # name
abbrev += uleb(0x0B) + uleb(0x0B)          # byte_size data1
abbrev += uleb(0x3E) + uleb(0x0B)          # encoding data1
abbrev += b"\x00\x00"
abbrev += uleb(3) + uleb(0x13) + b"\x01"  # structure_type, has children
abbrev += uleb(0x03) + uleb(0x08)          # name
abbrev += uleb(0x0B) + uleb(0x0B)          # byte_size
abbrev += b"\x00\x00"
abbrev += uleb(4) + uleb(0x0D) + b"\x00"  # member
abbrev += uleb(0x03) + uleb(0x08)          # name
abbrev += uleb(0x49) + uleb(0x13)          # type ref4
abbrev += uleb(0x38) + uleb(0x0B)          # data_member_location data1
abbrev += b"\x00\x00"
abbrev += uleb(5) + uleb(0x34) + b"\x00"  # variable
abbrev += uleb(0x03) + uleb(0x08)          # name
abbrev += uleb(0x49) + uleb(0x13)          # type ref4
abbrev += uleb(0x02) + uleb(0x18)          # location exprloc
abbrev += b"\x00\x00"
abbrev += b"\x00"

# ---------------- DWARF .debug_info ----------------
body = bytearray()

def add_die(code, payload):
    off = 0x0B + len(body)
    body.extend(die(code, payload))
    return off  # absolute offset of this DIE (after 11-byte unit header)

body.extend(die(1, b"cu.out\x00" + struct.pack("<H", 0x1C)))  # DW_LANG_C99
int_off = add_die(2, b"int\x00" + b"\x04\x05")                # size4 signed
cfg_off = add_die(3, b"cfg_t\x00" + b"\x08")                  # struct, size8
body.extend(die(4, b"a\x00" + struct.pack("<I", int_off) + b"\x00"))
body.extend(die(4, b"b\x00" + struct.pack("<I", int_off) + b"\x04"))
body += b"\x00"                                                # end struct children
body.extend(die(5, b"g_counter\x00" + struct.pack("<I", int_off) +
                 b"\x05\x03" + struct.pack("<I", 0x20000000)))  # DW_OP_addr
body.extend(die(5, b"g_cfg\x00" + struct.pack("<I", cfg_off) +
                 b"\x05\x03" + struct.pack("<I", 0x20000010)))
body += b"\x00"                                                # end CU children

info = bytearray()
info += struct.pack("<I", 7 + len(body))  # unit_length (version..end)
info += struct.pack("<H", 4)              # DWARF version
info += struct.pack("<I", 0)              # abbrev offset
info += b"\x04"                           # address size
info += body

# ---------------- symtab / strtab ----------------
strtab = b"\x00g_counter\x00g_cfg\x00g_raw\x00main\x00"
off_g_counter = strtab.index(b"g_counter")
off_g_cfg = strtab.index(b"g_cfg")
off_g_raw = strtab.index(b"g_raw")
off_main = strtab.index(b"main")

symtab = bytearray(b"\x00" * 16)  # null symbol
for name, val, size, info_byte in [
    (off_g_counter, 0x20000000, 4, 0x11),  # STB_GLOBAL | STT_OBJECT
    (off_g_cfg,     0x20000010, 8, 0x11),
    (off_g_raw,     0x20000020, 4, 0x11),
    (off_main,      0x08000000, 4, 0x12),  # STT_FUNC
]:
    symtab += struct.pack("<IIIBBH", name, val, size, info_byte, 0, 1)

# ---------------- section name strings ----------------
shstr = b"\x00.text\x00.debug_info\x00.debug_abbrev\x00.symtab\x00.strtab\x00.shstrtab\x00"
n_shstr = shstr.index(b".shstrtab")
n_text = shstr.index(b".text")
n_dbg_info = shstr.index(b".debug_info")
n_dbg_abbrev = shstr.index(b".debug_abbrev")
n_symtab = shstr.index(b".symtab")
n_strtab = shstr.index(b".strtab")

# ---------------- layout ----------------
ehsize = 52
shentsize = 40
shnum = 7
text = b"\x00\x00\x00\x00"

off = ehsize
sh_text = (n_text, 1, 0x6, 0x08000000, off, len(text), 0, 0, 4, 0)
off += len(text)
sh_dbg_info = (n_dbg_info, 1, 0, 0, off, len(info), 0, 0, 1, 0)
off += len(info)
sh_dbg_abbrev = (n_dbg_abbrev, 1, 0, 0, off, len(abbrev), 0, 0, 1, 0)
off += len(abbrev)
sh_symtab = (n_symtab, 2, 0, 0, off, len(symtab), 5, 1, 4, 16)  # link=strtab idx
off += len(symtab)
sh_strtab = (n_strtab, 3, 0, 0, off, len(strtab), 0, 0, 1, 0)
off += len(strtab)
sh_shstr = (n_shstr, 3, 0, 0, off, len(shstr), 0, 0, 1, 0)
off += len(shstr)
shoff = off

sections = [
    (0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
    sh_text, sh_dbg_info, sh_dbg_abbrev, sh_symtab, sh_strtab, sh_shstr,
]

out = bytearray()
out += b"\x7fELF" + bytes([1, 1, 1, 0]) + b"\x00" * 8
out += struct.pack("<HHIIIIIHHHHHH",
                   2,      # ET_EXEC
                   40,     # EM_ARM
                   1,      # version
                   0x08000000,  # entry
                   0,      # phoff
                   shoff,
                   0x05000000,  # flags (ABI v5)
                   ehsize, 0, 0, shentsize, shnum, 6)
out += text + info + abbrev + symtab + strtab + shstr
for s in sections:
    out += struct.pack("<IIIIIIIIII", *s)

Path("tests/elf_sample.out").write_bytes(bytes(out))
print("wrote tests/elf_sample.out (%d bytes)" % len(out))
