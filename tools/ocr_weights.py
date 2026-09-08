#!/usr/bin/env python3
"""Tiny OCR for pl8s screenshots: template-match weight digits.

Learns digit templates from a reference screenshot with known row weights,
then reads the weight column of a target screenshot.
"""
import sys

sys.path.insert(0, "tools")
from png_dump import read_png  # noqa: E402

ROW_H = 34
TOP_Y = 52
W_X0, W_X1 = 33, 82  # weight text zone: WEIGHT_X=33, gothic24 3 digits ~44px
N_ROWS = 5
# 5% step mode: 7 rows at 24px (compact layout).
ROW_H_FINE = 24
TOP_Y_FINE = 52
N_ROWS_FINE = 7


def bitmap(path, x0, x1, y0, y1, thresh=110):
    """Binarize a crop: 1 = bright text pixel, 0 = background."""
    w, h, ch, px = read_png(path)
    rows = []
    for y in range(y0, min(y1, h)):
        row = []
        for x in range(x0, min(x1, w)):
            i = (y * w + x) * ch
            lum = (px[i] * 299 + px[i + 1] * 587 + px[i + 2] * 114) // 1000
            row.append(1 if lum > thresh else 0)
        rows.append(row)
    return rows


def split_digits(rows):
    """Split a binarized band into digit crops via connected components (4-neigh)."""
    h = len(rows)
    w = len(rows[0]) if h else 0
    seen = [[False] * w for _ in range(h)]
    comps = []
    for sy in range(h):
        for sx in range(w):
            if rows[sy][sx] and not seen[sy][sx]:
                stack = [(sy, sx)]
                seen[sy][sx] = True
                pts = []
                while stack:
                    y, x = stack.pop()
                    pts.append((y, x))
                    for dy, dx in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                        ny, nx = y + dy, x + dx
                        if 0 <= ny < h and 0 <= nx < w and rows[ny][nx] and not seen[ny][nx]:
                            seen[ny][nx] = True
                            stack.append((ny, nx))
                ys = [p[0] for p in pts]
                xs = [p[1] for p in pts]
                y0, y1 = min(ys), max(ys)
                x0, x1 = min(xs), max(xs)
                comps.append((x0, [[1 if rows[y][x] else 0 for x in range(x0, x1 + 1)]
                                   for y in range(y0, y1 + 1)]))
    comps.sort(key=lambda c: c[0])
    return [c[1] for c in comps]


def score(a, b):
    """Similarity of two bitmaps (allowing small size differences)."""
    h = max(len(a), len(b))
    w = max(len(r) for r in a + b) if a and b else 0
    if h == 0 or w == 0:
        return 0.0
    hits = 0
    total = 0
    for y in range(h):
        for x in range(w):
            va = a[y][x] if y < len(a) and x < len(a[y]) else 0
            vb = b[y][x] if y < len(b) and x < len(b[y]) else 0
            hits += va == vb
            total += 1
    return hits / total


def learn(ref_path, top_y=TOP_Y, row_h=ROW_H, n_rows=N_ROWS):
    known = ["205", "180", "160", "135", "115"]
    templates = {}
    for row, text in enumerate(known[:n_rows]):
        y0 = top_y + row * row_h
        band = bitmap(ref_path, W_X0, W_X1, y0 + 6, y0 + row_h - 2)
        digits = split_digits(band)
        if len(digits) != len(text):
            print(f"ref row{row}: expected {len(text)} digits, got {len(digits)}",
                  file=sys.stderr)
        for ch, bmp in zip(text, digits):
            templates.setdefault(ch, []).append(bmp)
    return templates


def recognize(path, templates, top_y=TOP_Y, row_h=ROW_H, n_rows=N_ROWS):
    out = []
    for row in range(n_rows):
        y0 = top_y + row * row_h
        band = bitmap(path, W_X0, W_X1, y0 + 6, y0 + row_h - 2)
        digits = split_digits(band)
        s = ""
        for d in digits:
            best, bs = "?", -1
            for ch, tl in templates.items():
                sc = max(score(d, t) for t in tl)
                if sc > bs:
                    bs, best = sc, ch
            s += best if bs > 0.8 else "?"
        out.append(s)
    return out


if __name__ == "__main__":
    ref, target = sys.argv[1], sys.argv[2]
    # optional: --fine selects the 5% step geometry (7 rows, 24px)
    fine = len(sys.argv) > 3 and sys.argv[3] == "--fine"
    top_y, row_h, n_rows = (TOP_Y_FINE, ROW_H_FINE, N_ROWS_FINE) if fine \
        else (TOP_Y, ROW_H, N_ROWS)
    t = learn(ref, top_y, row_h, n_rows)
    print("digits learned:", sorted(t.keys()))
    for row, s in enumerate(recognize(target, t, top_y, row_h, n_rows)):
        print(f"row{row + 1}: {s}")
