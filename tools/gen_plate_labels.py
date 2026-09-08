#!/usr/bin/env python3
"""Generate rotated plate-label PNGs for pl8s.

Each label is the plate weight rendered vertically (rotated so it reads
bottom-to-top along the plate face), on a transparent background, in a
contrasting color for its plate class:

  big plates (55/45/35, orange fill)  -> white text
  mid  plates (25/15/10, white fill)  -> black text
  small plates (5/2.5, light-grey fill) -> black text

The PNGs are baked into the app as resources (see package.json "media")
and composited over the drawn plate rectangles at runtime with GCompOpSet
(alpha compositing), so the plate color coding stays runtime-driven.

Requires Pillow. If Pillow is missing, create a throwaway venv:

    python3 -m venv /tmp/lblenv && /tmp/lblenv/bin/pip install pillow
    /tmp/lblenv/bin/python tools/gen_plate_labels.py
"""
import os

from PIL import Image, ImageDraw, ImageFont

HERE = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(HERE, "..", "resources", "images")

# font candidates, first that exists wins
FONT_CANDIDATES = [
    "/System/Library/Fonts/ArialHB.ttc",
    "/System/Library/Fonts/Supplemental/Arial Bold.ttf",
    "/Library/Fonts/DejaVu Sans Bold.ttf",
    "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
]

# weight -> (font_size_pt, text_color, plate_w, plate_h)
SPECS = {
    "55": (14, (255, 255, 255, 255), 18, 28),
    "45": (14, (255, 255, 255, 255), 18, 28),
    "35": (14, (0, 0, 0, 255), 18, 28),
    "25": (10, (0, 0, 0, 255), 15, 20),
    "15": (10, (0, 0, 0, 255), 15, 20),
    "10": (10, (0, 0, 0, 255), 15, 20),
    "5": (9, (0, 0, 0, 255), 14, 18),
    "2.5": (8, (0, 0, 0, 255), 14, 18),
}

# Compact set for 5% step (7 rows on large displays): smaller discs so the
# taller row stack fits the screen. Emitted with a "c" suffix.
SPECS_COMPACT = {
    "55": (9, (255, 255, 255, 255), 14, 20),
    "45": (9, (255, 255, 255, 255), 14, 20),
    "35": (9, (0, 0, 0, 255), 14, 20),
    "25": (7, (0, 0, 0, 255), 12, 16),
    "15": (7, (0, 0, 0, 255), 12, 16),
    "10": (7, (0, 0, 0, 255), 12, 16),
    "5": (6, (0, 0, 0, 255), 11, 14),
    "2.5": (5, (0, 0, 0, 255), 11, 14),
}


def load_font():
    for p in FONT_CANDIDATES:
        if os.path.exists(p):
            return p
    raise SystemExit("no suitable bold font found; add one to FONT_CANDIDATES")


def make_label(lbs, size_pt, color, plate_w, plate_h, font_path):
    """Rendered label: weight rotated -90 (reads bottom-to-top), centered in
    an image of exactly (plate_w x plate_h) pixels with transparent padding."""
    font = ImageFont.truetype(font_path, size_pt)
    # shrink the font until the rotated text fits the plate with a 1px margin
    while size_pt > 1:
        tw, th = font.getbbox(lbs)[2:4]
        resized = tw <= plate_w - 2 and th <= plate_h - 2
        if resized:
            break
        size_pt -= 1
        font = ImageFont.truetype(font_path, size_pt)
    if not resized:
        raise SystemExit(f"label {lbs!r} too wide even at 1pt for {plate_w}x{plate_h}")

    horizontal = Image.new("RGBA", (tw + 2, th + 2), (0, 0, 0, 0))
    ImageDraw.Draw(horizontal).text((1, 1), lbs, font=font, fill=color)
    rotated = horizontal.rotate(-90, expand=True, resample=Image.BICUBIC)

    out = Image.new("RGBA", (plate_w, plate_h), (0, 0, 0, 0))
    x = (plate_w - rotated.size[0]) // 2
    y = (plate_h - rotated.size[1]) // 2
    out.paste(rotated, (x, y), rotated)
    return out


def main():
    font_path = load_font()
    os.makedirs(OUT, exist_ok=True)
    import re
    for suffix, specs in (("", SPECS), ("c", SPECS_COMPACT)):
        for lbs, (size_pt, color, w, h) in specs.items():
            img = make_label(lbs, size_pt, color, w, h, font_path)
            name = "label_" + re.sub(r"\W", "p", lbs) + suffix
            path = os.path.join(OUT, f"{name}.png")
            img.save(path)
            print(f"{path} ({img.size[0]}x{img.size[1]})")


if __name__ == "__main__":
    main()