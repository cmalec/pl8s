#!/usr/bin/env python3
"""Generate the pl8s menu icon (25x25 PNG) with no external deps.

White barbell glyph on transparent background, drawn as a pixel map.
"""
import struct
import zlib

W = H = 25

# Pixel map: rows of '#' = white pixel
ART = """
.........................
.........................
.........................
.........................
.......##.......##.......
.......##.......##.......
.......##.......##.......
.......##.......##.......
..#....##.......##....#..
..#....##.......##....#..
..###############...#..#
..###############...#..#
..###############...#..#
..#....##.......##....#..
..#....##.......##....#..
.........................
""".strip("\n").split("\n")

rows = []
for y in range(H):
    line = ART[y] if y < len(ART) else "." * W
    row = bytearray([0])  # filter type 0
    for x in range(W):
        if x < len(line) and line[x] == "#":
            row += bytes([255, 255, 255, 255])
        else:
            row += bytes([0, 0, 0, 0])
    rows.append(bytes(row))

raw = b"".join(rows)


def chunk(tag, data):
    c = struct.pack(">I", len(data)) + tag + data
    c += struct.pack(">I", zlib.crc32(tag + data) & 0xFFFFFFFF)
    return c


png = b"\x89PNG\r\n\x1a\n"
png += chunk(b"IHDR", struct.pack(">IIBBBBB", W, H, 8, 6, 0, 0, 0))
png += chunk(b"IDAT", zlib.compress(raw))
png += chunk(b"IEND", b"")

with open("resources/images/menu_icon.png", "wb") as f:
    f.write(png)
print("wrote resources/images/menu_icon.png", len(png), "bytes")
