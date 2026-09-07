#!/usr/bin/env python3
"""Dump a Pebble emulator screenshot PNG as ASCII art (stdlib only)."""
import struct
import sys
import zlib


def read_png(path):
    with open(path, "rb") as f:
        data = f.read()
    assert data[:8] == b"\x89PNG\r\n\x1a\n", "not a PNG"
    pos = 8
    w = h = bitd = ctype = None
    idat = b""
    while pos < len(data):
        ln = struct.unpack(">I", data[pos:pos + 4])[0]
        tag = data[pos + 4:pos + 8]
        payload = data[pos + 8:pos + 8 + ln]
        if tag == b"IHDR":
            w, h, bitd, ctype = struct.unpack(">IIBB", payload[:10])
        elif tag == b"IDAT":
            idat += payload
        pos += 12 + ln
    raw = zlib.decompress(idat)
    channels = {0: 1, 2: 3, 4: 2, 6: 4}[ctype]
    stride = w * channels
    # Undo filters
    out = bytearray()
    prev = bytearray(stride)
    p = 0
    for y in range(h):
        ftype = raw[p]
        p += 1
        line = bytearray(raw[p:p + stride])
        p += stride
        if ftype == 1:
            for i in range(channels, stride):
                line[i] = (line[i] + line[i - channels]) & 0xFF
        elif ftype == 2:
            for i in range(stride):
                line[i] = (line[i] + prev[i]) & 0xFF
        elif ftype == 3:
            for i in range(stride):
                a = line[i - channels] if i >= channels else 0
                line[i] = (line[i] + (a + prev[i]) // 2) & 0xFF
        elif ftype == 4:
            for i in range(stride):
                a = line[i - channels] if i >= channels else 0
                b = prev[i]
                c = prev[i - channels] if i >= channels else 0
                pp = a + b - c
                pa, pb, pc = abs(pp - a), abs(pp - b), abs(pp - c)
                pr = a if (pa <= pb and pa <= pc) else (b if pb <= pc else c)
                line[i] = (line[i] + pr) & 0xFF
        out += line
        prev = line
    return w, h, channels, bytes(out)


def main(path):
    w, h, ch, px = read_png(path)
    print(f"{w}x{h} channels={ch}")
    ramp = " .:-=+*#%@"
    for y in range(h):
        row = ""
        for x in range(w):
            i = (y * w + x) * ch
            if ch >= 3:
                r, g, b = px[i], px[i + 1], px[i + 2]
                lum = (r * 299 + g * 587 + b * 114) // 1000
                alpha = px[i + 3] if ch == 4 else 255
            else:
                lum, alpha = px[i], 255
            if alpha < 128:
                row += " "
            else:
                row += ramp[min(lum * len(ramp) // 256, len(ramp) - 1)]
        print(row)


if __name__ == "__main__":
    main(sys.argv[1])
