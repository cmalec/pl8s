#!/usr/bin/env python3
"""Render the appstore icons and marketing banners from vector sources.

The icons are the listing's large (144x144) and small (48x48) images: the
pl8s mark, a barbell carrying gym-colored plates. The banners are the
720x320 marketing images each asset collection gets; they frame that
platform's own screenshot (tools/appstore_shots.py) in a drawn device bezel,
since the screenshots uploaded to the listing itself must stay unframed.

Needs rsvg-convert (librsvg) on PATH, e.g. `brew install librsvg`.
"""
import base64
import os
import subprocess
import sys

SCRIPTS = "appstore/screenshots"
BANNERS = "appstore/banner"

# Gym-standard plate colors, as the app draws them (55 red, 45 blue, 35
# yellow, 25 green, 15 pink, 10 white, 5 cyan, 2.5 grey).
RED, BLUE, YELLOW = "#e53935", "#1e6fd9", "#fdd835"
GREEN, WHITE, CYAN = "#43a047", "#f5f5f5", "#26c6da"

CUES = ("5% and 10% rows", "99 = unlimited")

# Screenshot size per platform, as the appstore stores them.
SHOT_SIZES = {
    "aplite": (144, 168), "basalt": (144, 168), "chalk": (180, 180),
    "diorite": (144, 168), "emery": (200, 228), "flint": (144, 168),
    "gabbro": (260, 260),
}
ROUND = ("chalk", "gabbro")


# Plate stacks per side, outermost first (the last plate loaded), as the app
# colours them: a 25, a 45 and a 55.
STACK = ((GREEN, 60, 11), (BLUE, 78, 13), (RED, 90, 13))


def plates_svg(cx, cy, bar_half, gap, sleeve):
    """Plate stacks either side of the bar, hung from its ends inward."""
    out = []
    for sign in (1, -1):
        edge = cx + (bar_half - sleeve) * sign  # outer edge of the stack
        for color, height, width in STACK:
            x = edge - width if sign > 0 else edge
            out.append(f'<rect x="{x}" y="{cy - height / 2}" width="{width}" '
                       f'height="{height}" rx="3" fill="{color}" '
                       f'stroke="#00000033" stroke-width="1"/>')
            edge -= (width + gap) * sign
    return "".join(out)


def barbell_svg(detail):
    """The barbell mark, drawn in a 144x144 space around the centre."""
    bar_h = 13 if detail == "small" else 11
    gap = 1 if detail == "small" else 2
    sleeve = 0 if detail == "small" else 4
    bar_half = 58 if detail == "small" else 57
    bar = (f'<rect x="{72 - bar_half}" y="{72 - bar_h / 2}" width="{bar_half * 2}" '
           f'height="{bar_h}" rx="{bar_h / 2}" fill="url(#steel)"/>')
    return bar + plates_svg(72, 72, bar_half, gap, sleeve)


def icon_svg(detail):
    return f'''<svg xmlns="http://www.w3.org/2000/svg" width="144" height="144"
     viewBox="0 0 144 144">
  <defs>
    <linearGradient id="bg" x1="0" y1="0" x2="0" y2="1">
      <stop offset="0" stop-color="#242b35"/><stop offset="1" stop-color="#0b0e12"/>
    </linearGradient>
    <linearGradient id="steel" x1="0" y1="0" x2="0" y2="1">
      <stop offset="0" stop-color="#eff3f8"/><stop offset="0.45" stop-color="#a7b0bc"/>
      <stop offset="1" stop-color="#5b636e"/>
    </linearGradient>
  </defs>
  <rect width="144" height="144" rx="30" fill="url(#bg)"/>
  <rect x="1" y="1" width="142" height="142" rx="29" fill="none"
        stroke="#ffffff1f" stroke-width="2"/>
  {barbell_svg(detail)}
</svg>
'''


def banner_svg(platform, screenshot):
    """One 720x320 banner: copy down the left, the platform's screen on the
    right, in a drawn bezel (the listing screenshots themselves stay raw)."""
    width, height = SHOT_SIZES[platform]
    scale = 250 / height
    sw, sh = width * scale, height * scale
    bezel = 18
    bx = 664 - sw - bezel * 2
    by = (320 - sh - bezel * 2) / 2
    cx, cy = bx + bezel + sw / 2, by + bezel + sh / 2
    radius = min(sw, sh) / 2
    shot = base64.b64encode(open(screenshot, "rb").read()).decode()

    if platform in ROUND:
        body = (f'<circle cx="{cx}" cy="{cy}" r="{radius + bezel}" '
                f'fill="#2b313a" stroke="#454d58" stroke-width="2"/>')
        shadow = (f'<circle cx="{cx + 8}" cy="{cy + 10}" r="{radius + bezel}" '
                  f'fill="#000000" opacity="0.45"/>')
        screen = (f'<circle cx="{cx}" cy="{cy}" r="{radius}" fill="#000"/>'
                  f'<clipPath id="screen"><circle cx="{cx}" cy="{cy}" '
                  f'r="{radius}"/></clipPath>')
    else:
        body = (f'<rect x="{bx}" y="{by}" width="{sw + bezel * 2}" '
                f'height="{sh + bezel * 2}" rx="{32 + bezel}" fill="#2b313a" '
                f'stroke="#454d58" stroke-width="2"/>')
        shadow = (f'<rect x="{bx + 8}" y="{by + 10}" width="{sw + bezel * 2}" '
                  f'height="{sh + bezel * 2}" rx="{32 + bezel}" '
                  f'fill="#000000" opacity="0.45"/>')
        screen = (f'<rect x="{bx + bezel}" y="{by + bezel}" width="{sw}" '
                  f'height="{sh}" rx="28" fill="#000"/>'
                  f'<clipPath id="screen"><rect x="{bx + bezel}" '
                  f'y="{by + bezel}" width="{sw}" height="{sh}" rx="28"/></clipPath>')

    # Chips are sized from their text (Helvetica at 15px is about 7.4px per
    # character) and kept inside the copy column, x 52..366.
    cues, x = [], 52
    for cue in CUES:
        w = 30 + len(cue) * 7.4
        cues.append(f'<rect x="{x:.0f}" y="236" width="{w:.0f}" height="34" '
                    f'rx="17" fill="#ffffff14" stroke="#ffffff3d"/>'
                    f'<text x="{x + 15:.0f}" y="258" font-size="15" '
                    f'font-family="Helvetica, Arial, sans-serif" '
                    f'fill="#dfe5ec">{cue}</text>')
        x += w + 12

    return f'''<svg xmlns="http://www.w3.org/2000/svg" width="720" height="320"
     viewBox="0 0 720 320">
  <defs>
    <linearGradient id="bg" x1="0" y1="0" x2="1" y2="1">
      <stop offset="0" stop-color="#161b22"/><stop offset="1" stop-color="#0a0c10"/>
    </linearGradient>
  </defs>
  <rect width="720" height="320" fill="url(#bg)"/>
  <g opacity="0.09">
    <circle cx="648" cy="60" r="150" fill="{RED}"/>
    <circle cx="452" cy="300" r="110" fill="{BLUE}"/>
    <circle cx="710" cy="308" r="90" fill="{GREEN}"/>
  </g>
  <g opacity="0.05" transform="translate(268 240) scale(1.35) rotate(-8 72 72)">
    {barbell_svg("full")}
  </g>
  <text x="52" y="122" font-family="Helvetica, Arial, sans-serif" font-size="74"
        font-weight="bold" fill="#ffffff">pl8s</text>
  <text x="54" y="168" font-family="Helvetica, Arial, sans-serif" font-size="24"
        fill="#aab2bd">Barbell percentages and</text>
  <text x="54" y="198" font-family="Helvetica, Arial, sans-serif" font-size="24"
        fill="#aab2bd">exact plates per side</text>
  {''.join(cues)}
  {shadow}
  {body}
  {screen}
  <g clip-path="url(#screen)">
    <image x="{bx + bezel}" y="{by + bezel}" width="{sw}" height="{sh}"
           href="data:image/png;base64,{shot}"/>
  </g>
</svg>
'''


def render(svg, path, width, height):
    tmp = f"{path}.svg"
    with open(tmp, "w") as f:
        f.write(svg)
    subprocess.run(["rsvg-convert", "-w", str(width), "-h", str(height),
                    "-o", path, tmp], check=True)
    os.remove(tmp)
    print(f"wrote {path} ({width}x{height})")


def main():
    render(icon_svg("full"), "appstore/icon-144.png", 144, 144)
    render(icon_svg("small"), "appstore/icon-48.png", 48, 48)
    missing = []
    for platform, (w, h) in SHOT_SIZES.items():
        screenshot = f"{SCRIPTS}/{platform}-results-10pct.png"
        if not os.path.exists(screenshot):
            missing.append(platform)
            continue
        render(banner_svg(platform, screenshot), f"{BANNERS}/{platform}.png",
               720, 320)
    if missing:
        print(f"\nno screenshot yet for: {', '.join(missing)}", file=sys.stderr)


if __name__ == "__main__":
    main()
