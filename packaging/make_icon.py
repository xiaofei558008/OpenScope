#!/usr/bin/env python3
"""生成 OpenScope 应用/安装包图标 assets/openscope.ico（纯标准库，无第三方依赖）。
设计：深色圆角方块示波器屏幕 + 青色正弦曲线 + 红色采样点。
"""
import math
import os
import struct


ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, "assets", "openscope.ico")
SIZES = [16, 24, 32, 48, 64, 128, 256]
SS = 4  # 超采样倍数，用于抗锯齿

BG = (24, 34, 66)        # 深蓝 #182242
BG_EDGE = (46, 92, 158)  # 边缘高亮
TRACE = (66, 232, 244)   # 青色
GRID = (42, 60, 104)     # 网格线
DOT = (255, 74, 74)      # 红色采样点


def rounded_rect(px, size, radius):
    r = radius
    for y in range(size):
        for x in range(size):
            dx = min(x, size - 1 - x)
            dy = min(y, size - 1 - y)
            if dx < r and dy < r:
                cx, cy = r, r
                if (dx - cx + 0.5) ** 2 + (dy - cy + 0.5) ** 2 > (r - 0.5) ** 2:
                    continue
            px[y][x] = BG


def trace_y(x, size, t):
    # 正弦曲线：左到右 1.2 个周期，幅度 0.32*size
    mid = size / 2
    return mid - math.sin(x / size * math.pi * 2.4 + t) * size * 0.30


def render(size):
    hs = size * SS
    px = [[None] * hs for _ in range(hs)]
    rounded_rect(px, hs, hs * 0.22)
    # 网格：每 1/4 高度一条横线，每 1/5 宽度一条竖线
    for g in range(1, 4):
        gy = round(hs * g / 4)
        for x in range(hs):
            if px[gy][x]:
                px[gy][x] = GRID
    for g in range(1, 5):
        gx = round(hs * g / 5)
        for y in range(hs):
            if px[y][gx]:
                px[y][gx] = GRID
    # 正弦曲线
    for x in range(hs):
        y = int(round(trace_y(x, hs, -0.35)))
        for dy in range(-1, 2):
            yy = y + dy
            if 0 <= yy < hs and px[yy][x]:
                px[yy][x] = TRACE
    # 红色采样点（曲线末端）
    ex = hs - int(hs * 0.10)
    ey = int(round(trace_y(ex, hs, -0.35)))
    dr = max(1, round(hs * 0.035))
    for y in range(max(0, ey - dr), min(hs, ey + dr + 1)):
        for x in range(max(0, ex - dr), min(hs, ex + dr + 1)):
            if (x - ex) ** 2 + (y - ey) ** 2 <= dr * dr and px[y][x]:
                px[y][x] = DOT
    # 超采样降采样（含 alpha 平均）
    out = [[(0, 0, 0, 0)] * size for _ in range(size)]
    for oy in range(size):
        for ox in range(size):
            r = g = b = a = 0
            for sy in range(SS):
                for sx in range(SS):
                    c = px[oy * SS + sy][ox * SS + sx]
                    if c:
                        r += c[0]; g += c[1]; b += c[2]; a += 255
            n = SS * SS
            out[oy][ox] = (r // n, g // n, b // n, a // n)
    return out


def make_ico():
    images = []
    for size in SIZES:
        px = render(size)
        # XOR 位图（自底向上 BGRA）+ AND 掩码（透明=1）
        xor = bytearray()
        for y in range(size - 1, -1, -1):
            for x in range(size):
                r, g, b, a = px[y][x]
                xor += bytes((b, g, r, a))
        mask_row = (size + 31) // 32 * 4
        and_mask = bytearray()
        for y in range(size):
            row = bytearray(mask_row)
            for x in range(size):
                if px[y][x][3] == 0:
                    row[x // 8] |= 0x80 >> (x % 8)
            and_mask += row
        header = struct.pack(
            "<IiiHHIIiiII",
            40, size, size * 2, 1, 32, 0, len(xor) + len(and_mask), 0, 0, 0, 0,
        )
        images.append((size, bytes(header) + bytes(xor) + bytes(and_mask)))
    # ICONDIR + ICONDIRENTRY
    count = len(images)
    data = bytearray(struct.pack("<HHH", 0, 1, count))
    offset = 6 + 16 * count
    for size, blob in images:
        w = size if size < 256 else 0
        data += struct.pack("<BBBBHHII", w, w, 0, 0, 1, 32, len(blob), offset)
        offset += len(blob)
    for _, blob in images:
        data += blob
    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    with open(OUT, "wb") as f:
        f.write(data)
    print(f"[make_icon] OK {OUT} ({len(data)} bytes, {count} sizes)")


if __name__ == "__main__":
    make_ico()
