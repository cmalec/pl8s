#!/usr/bin/env python3
"""Read the weight column of pl8s screenshots.

Digits are split out of a row's weight band by column projection and matched
against templates learned from reference screenshots whose weights are known
(see learn()). The app draws the same glyphs at the same place every time, so
matching is exact rather than fuzzy.

Used by tools/wizard_test.py to assert what the watch actually shows; standalone
use for debugging the results screen:

    python3 tools/ocr_weights.py shots/wiz_base.png shots/wiz_final.png
"""
import sys

sys.path.insert(0, "tools")
from png_dump import read_png  # noqa: E402

# Geometry of the results screen on large layouts (emery/gabbro): five 10%-step
# rows under a two-line header, weight text left-aligned at the app's
# WEIGHT_X (34) and three digits wide. The percent column ends before it, and
# the band stops short of the plates (a heavy row's stack starts around x=84,
# reaching left from the collar) so only glyphs land in it.
TOP_Y = 52
ROW_H = 34
N_ROWS = 5
X0, X1 = 34, 78
Y_PAD = 3        # trim the band's top/bottom borders off the font box
INK_LUM = 110    # lit-pixel threshold; the weight text is drawn white
MIN_COL_GAP = 1  # blank columns that separate two digits
MIN_DIGIT_W = 2  # anything narrower is antialiasing, not a digit
MATCH_MIN = 0.90


def band(path, y0, y1, x0=X0, x1=X1):
    """Binarized crop of one row's weight column: 1 = lit text pixel."""
    w, h, ch, px = read_png(path)
    rows = []
    for y in range(y0, min(y1, h)):
        row = []
        for x in range(x0, min(x1, w)):
            i = (y * w + x) * ch
            lum = (px[i] * 299 + px[i + 1] * 587 + px[i + 2] * 114) // 1000
            row.append(1 if lum > INK_LUM else 0)
        rows.append(row)
    return rows


def segments(rows):
    """Split a band into digit crops using its lit-column profile."""
    if not rows:
        return []
    width = len(rows[0])
    lit = [any(r[x] for r in rows) for x in range(width)]

    crops = []
    start = None
    gap = 0
    for x in range(width):
        if lit[x]:
            if start is None:
                start = x
            gap = 0
        elif start is not None:
            gap += 1
            if gap >= MIN_COL_GAP:
                crops.append((start, x - gap + 1))
                start = None
    if start is not None:
        crops.append((start, width))

    out = []
    for x0, x1 in crops:
        if x1 - x0 < MIN_DIGIT_W:
            continue
        ys = [y for y in range(len(rows)) if any(rows[y][x0:x1])]
        out.append([rows[y][x0:x1] for y in range(min(ys), max(ys) + 1)])
    return out


def row_band(path, row, top_y=TOP_Y, row_h=ROW_H):
    y0 = top_y + row * row_h
    return band(path, y0 + Y_PAD, y0 + row_h - Y_PAD)


def score(a, b):
    """Best pixel agreement between two digit crops over small offsets."""
    ha, wa = len(a), len(a[0])
    hb, wb = len(b), len(b[0])
    best = 0.0
    for dy in range(-2, 3):
        for dx in range(-2, 3):
            hits = total = 0
            for y in range(max(ha, hb + dy)):
                for x in range(max(wa, wb + dx)):
                    va = a[y][x] if 0 <= y < ha and 0 <= x < wa else 0
                    yb, xb = y - dy, x - dx
                    vb = b[yb][xb] if 0 <= yb < hb and 0 <= xb < wb else 0
                    hits += va == vb
                    total += 1
            best = max(best, hits / total)
    return best


def learn(refs):
    """Templates for each digit, from (screenshot, [known row weights]).

    Raises when a reference row does not split into exactly as many digits as
    its known weight has, which is how a changed layout shows up.
    """
    templates = {}
    for path, known in refs:
        for row, text in enumerate(known):
            crops = segments(row_band(path, row))
            if len(crops) != len(text):
                raise ValueError(
                    f"{path} row {row + 1}: {len(crops)} digit crops for {text!r}")
            for ch, crop in zip(text, crops):
                templates.setdefault(ch, []).append(crop)
    return templates


def recognize(path, templates, n_rows=N_ROWS):
    """Row weights as strings; '?' marks a digit no template matched."""
    out = []
    for row in range(n_rows):
        text = ""
        for crop in segments(row_band(path, row)):
            best, best_score = "?", 0.0
            for ch, crops in templates.items():
                s = max(score(crop, t) for t in crops)
                if s > best_score:
                    best, best_score = ch, s
            text += best if best_score >= MATCH_MIN else "?"
        out.append(text)
    return out


if __name__ == "__main__":
    # Debug helper: read the frames the emulator test leaves behind.
    #   python3 tools/ocr_weights.py <ref.png> <known,rows> [target.png ...]
    if len(sys.argv) < 3:
        raise SystemExit(__doc__)
    reference, known = sys.argv[1], sys.argv[2].split(",")
    targets = sys.argv[3:] or [reference]
    templates = learn([(reference, known)])
    print(f"digits learned: {''.join(sorted(templates))}")
    for path in targets:
        for row, text in enumerate(recognize(path, templates), start=1):
            print(f"{path} row{row}: {text}")
