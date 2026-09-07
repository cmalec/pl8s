#!/usr/bin/env python3
"""Print a crop of an ASCII dump file: rows y0..y1, cols x0..x1 (1-indexed lines)."""
import sys

path, y0, y1, x0, x1 = sys.argv[1], int(sys.argv[2]), int(sys.argv[3]), int(sys.argv[4]), int(sys.argv[5])
lines = open(path).read().splitlines()
for i in range(y0, min(y1 + 1, len(lines))):
    print(lines[i][x0 - 1:x1])
