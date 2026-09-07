#!/usr/bin/env python3
"""Compare weight-column pixels between two screenshots, per row."""
import sys

sys.path.insert(0, "tools")
from png_dump import read_png  # noqa: E402


def diff(path_a, path_b, label, x0=55, x1=145):
    a = read_png(path_a)
    b = read_png(path_b)
    w, h, ch = a[0], a[1], a[2]
    rows = {"row1 (90%)": (44, 76), "row2 (80%)": (76, 108),
            "row3 (70%)": (108, 140), "row4 (60%)": (140, 172),
            "row5 (50%)": (172, 204)}
    out = []
    for name, (y0, y1) in rows.items():
        n = 0
        for y in range(y0, min(y1, h)):
            for x in range(x0, x1):
                i = (y * w + x) * ch
                if (abs(a[3][i] - b[3][i]) > 30
                        or abs(a[3][i + 1] - b[3][i + 1]) > 30
                        or abs(a[3][i + 2] - b[3][i + 2]) > 30):
                    n += 1
        out.append(f"{name}: {n}")
    print(label, "|", ", ".join(out))


if __name__ == "__main__":
    diff(sys.argv[1], sys.argv[2], sys.argv[3] if len(sys.argv) > 3 else "")
