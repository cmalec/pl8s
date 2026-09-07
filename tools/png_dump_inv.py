#!/usr/bin/env python3
"""Dump PNG rows as ASCII, inverted (dark pixels -> '#'), for light screens."""
import sys
sys.path.insert(0, "tools")
from png_dump import read_png  # noqa: E402

path = sys.argv[1]
y0 = int(sys.argv[2]) if len(sys.argv) > 2 else 0
y1 = int(sys.argv[3]) if len(sys.argv) > 3 else 10**9
w, h, ch, px = read_png(path)
for y in range(max(0, y0), min(h, y1)):
    row = ""
    for x in range(w):
        i = (y * w + x) * ch
        lum = (px[i] * 299 + px[i + 1] * 587 + px[i + 2] * 114) // 1000
        row += "#" if lum < 160 else " "
    print(f"{y:3d} {row}")
