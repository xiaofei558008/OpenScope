import re, sys, struct, pefile
from capstone import *
from capstone.x86 import *
P=r'C:\Program Files\SEGGER\JLink_V966\JScope.exe'
pe=pefile.PE(P)
IB=pe.OPTIONAL_HEADER.ImageBase
secs={}
for s in pe.sections:
    n=s.Name.decode().rstrip('\0')
    secs[n]=(IB+s.VirtualAddress, s.PointerToRawData, s.Misc_VirtualSize, s.SizeOfRawData)
text_va,text_off,text_vs,_=secs['.text']
text=pe.__data__[text_off:text_off+text_vs]
# decode whole .text linearly-ish: use capstone in skipdata mode
md=Cs(CS_ARCH_X86, CS_MODE_64); md.detail=True; md.skipdata=True
insns={}
for i in md.disasm(text, IB+pe.sections[[s.Name.decode().rstrip('\0') for s in pe.sections].index('.text')].VirtualAddress):
    insns[i.address]=i
print('decoded insns:', len(insns), file=sys.stderr)
# collect rip-relative operands
refs={}   # target_va -> list of insn addr
for a,i in insns.items():
    if i.id==0: continue   # skipdata pseudo-insn
    try: ops=i.operands
    except Exception: continue
    for op in ops:
        if op.type==CS_OP_MEM and op.mem.base==X86_REG_RIP:
            tgt=i.address+i.size+op.mem.disp
            refs.setdefault(tgt,[]).append((a,i))
import pickle
pickle.dump({k:[x[0] for x in v] for k,v in refs.items()}, open(r'D:\OpenScope\JScope_REVERSE\dumps\refs.pkl','wb'))
print('unique targets:', len(refs), file=sys.stderr)
