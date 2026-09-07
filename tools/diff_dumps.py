#!/usr/bin/env python3
"""Diff two ASCII dumps in a crop region to confirm the UI changed."""
import sys

a = open(sys.argv[1]).read().splitlines()
b = open(sys.argv[2]).read().splitlines()
y0, y1, x0, x1 = (int(v) for v in sys.argv[3:7])
diff = 0
for y in range(y0, y1):
    ra = a[y][x0:x1] if y < len(a) else ""
    rb = b[y][x0:x1] if y < len(b) else ""
    if ra != rb:
        diff += 1
        if diff <= 30:
            print(f"y={y} OLD {ra.strip()[:32]!r} NEW {rb.strip()[:32]!r}")
print(f"rows differing in crop ({y0}:{y1},{x0}:{x1}): {diff}")
