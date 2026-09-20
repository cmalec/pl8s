#!/usr/bin/env python3
"""Generate the pl8s menu icon (25x25) with no external deps.

The launcher icon needs two forms, selected per platform by the SDK's resource
tag mechanism (see `platform-specific resources` in the Pebble guides):

  menu_icon~bw.png     1-bit greyscale, black glyph on opaque white.
                       Aplite's launcher-icon inversion mode requires the
                       resource to be 1Bit and to contain only black and
                       white - transparency and colour are not handled there.

  menu_icon~color.png  RGBA, black glyph on transparent. Colour platforms
                       render launcher icons as non-inverting transparent
                       greyscale, so the alpha is what makes the glyph sit on
                       whatever the launcher draws behind it.

The launcher lists apps on a light background, where a white glyph disappears,
so both variants draw black.
"""
import struct
import zlib

W = H = 25

# Pixel map: rows of '#' = black pixel
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


def is_black(x, y):
    line = ART[y] if y < len(ART) else ""
    return x < len(line) and line[x] == "#"


def chunk(tag, data):
    c = struct.pack(">I", len(data)) + tag + data
    c += struct.pack(">I", zlib.crc32(tag + data) & 0xFFFFFFFF)
    return c


def write_png(path, ihdr_tail, rows):
    raw = b"".join(rows)
    png = b"\x89PNG\r\n\x1a\n"
    png += chunk(b"IHDR", struct.pack(">IIBBBBB", W, H, *ihdr_tail))
    png += chunk(b"IDAT", zlib.compress(raw))
    png += chunk(b"IEND", b"")
    with open(path, "wb") as f:
        f.write(png)
    print(f"wrote {path} ({len(png)} bytes)")


# --- colour variant: RGBA, black glyph, transparent elsewhere -------------
rgba_rows = []
for y in range(H):
    row = bytearray([0])  # filter type 0
    for x in range(W):
        row += bytes([0, 0, 0, 255]) if is_black(x, y) else bytes([0, 0, 0, 0])
    rgba_rows.append(bytes(row))

# bit depth 8, colour type 6 (RGBA)
write_png("resources/images/menu_icon~color.png", (8, 6, 0, 0, 0), rgba_rows)


# --- 1-bit variant: greyscale, 0 = black, 1 = white -----------------------
# Rows are packed MSB-first, 8 pixels per byte, padded to a whole byte.
stride = (W + 7) // 8
bw_rows = []
for y in range(H):
    row = bytearray([0])  # filter type 0
    packed = bytearray(stride)
    for x in range(W):
        if not is_black(x, y):  # white background -> bit set
            packed[x // 8] |= 0x80 >> (x % 8)
    row += packed
    bw_rows.append(bytes(row))

# bit depth 1, colour type 0 (greyscale)
write_png("resources/images/menu_icon~bw.png", (1, 0, 0, 0, 0), bw_rows)
