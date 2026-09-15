#!/usr/bin/env python3
"""Drive the plate-count wizard on the emery emulator and check what it shows.

The host unit test (tools/test_plate_math.c) covers the plate math; the
emulator is the only place the real app runs, so this covers the wizard, the
persistence and the rendering. Assertions are on the weights the screen shows,
read back with tools/ocr_weights.py -- not just on "the frame changed".

Needs the Pebble SDK and a build (`pebble build`). Run with
`npm run test:emulator`. It restarts the emulator, killing whichever one is
running, so the app starts from a clean state.
"""
import json
import subprocess
import sys
import time

sys.path.insert(0, "tools")
from ocr_weights import learn, recognize  # noqa: E402
from png_dump import read_png  # noqa: E402

EMULATOR = "emery"
SHOTS = "shots/wiz"
PLATE_KEYS = ("PLATE_55", "PLATE_45", "PLATE_35", "PLATE_25", "PLATE_15",
              "PLATE_10", "PLATE_5", "PLATE_2P5")
UNLIMITED = 10

# What the app shows with every size unlimited: the plain nearest-2.5 lb
# rounding of each percentage, per (max, rows). These are also the frames the
# digit templates are learned from. Maxima stay at 500 and under: a heavier
# row stacks plates over the weight text.
REFERENCES = [(495, ["445", "395", "345", "295", "250"]),
              (315, ["285", "250", "220", "190", "160"])]

# Wizard run at that max: 10% steps stay, every plate size stays unlimited
# except the 2.5 lb plates, which go to none. Only the 50% row is affected:
# 495 lb * 50% is 247.5 lb, and with no 2.5 lb plates the heaviest loadable
# weight under it is 245 lb (45 bar + 100 per side, all in even 5 lb steps).
MAX_LB = 495
AFTER_WIZARD = ["445", "395", "345", "295", "245"]


def check(condition, message):
    if not condition:
        raise SystemExit(f"FAIL: {message}")
    print(f"ok - {message}")


def pebble(*args, timeout=180):
    return subprocess.run(["pebble", *args, "--emulator", EMULATOR],
                          capture_output=True, text=True, timeout=timeout)


def button(click):
    pebble("emu-button", "click", click)


def hold_select():
    pebble("emu-button", "push", "select")
    time.sleep(0.8)
    pebble("emu-button", "release", "select")
    time.sleep(1.4)


def shot(name):
    path = f"{SHOTS}_{name}.png"
    result = pebble("screenshot", "--no-open", path, timeout=90)
    check("Saved" in result.stdout + result.stderr, f"screenshot {path}")
    return path


def frame_sig(path):
    w, h, ch, px = read_png(path)
    return (w, h, bytes(px[::7]))  # subsampled signature, fast compare


def results_screen(path):
    """Is the app's results screen in front?

    The title strip is the only orange-red thing the app draws, and it is the
    one thing the number entry windows (white panels), the watchface and the
    launcher never show. Rows full of plates push the black coverage down, so
    the darkness test stays loose.
    """
    w, h, ch, px = read_png(path)
    title = 0
    for y in range(min(52, h)):
        for x in range(w):
            i = (y * w + x) * ch
            r, g, b = px[i], px[i + 1], px[i + 2]
            if r > 180 and 80 < g < 150 and 80 < b < 150:
                title += 1
    dark = sum(1 for i in range(0, w * h * ch, ch) if px[i] < 40) / (w * h)
    return title > 50 and dark > 0.25


def send(**values):
    """Push settings to the running app, keyed by the SDK's generated ids."""
    with open("build/js/message_keys.json") as f:
        keys = json.load(f)
    # One --int flag: the CLI keeps a single key=value group per flag.
    pairs = [f"{keys[name]}={value}" for name, value in values.items()]
    result = pebble("send-app-message", "--int", *pairs)
    check(result.returncode == 0 and not result.stderr.strip(),
          f"app message {values} delivered ({result.stderr.strip()})")


def rows(path, templates):
    return recognize(path, templates)


def clicks_until_change(click, previous, tag, max_tries=4):
    """Click until the frame changes: the emulator drops clicks under load."""
    for attempt in range(1, max_tries + 1):
        button(click)
        time.sleep(1.3)
        path = shot(tag)
        if frame_sig(path) != previous:
            return frame_sig(path), attempt
        print(f"  (frame unchanged, retry {attempt})")
    raise SystemExit(f"FAIL: {click} never changed the screen ({tag})")


def launch_app():
    """Bring the app to the front, from wherever the emulator is."""
    for _ in range(12):
        path = shot("front")
        if results_screen(path):
            return
        button("back")   # dismisses the number window, or leaves the app
        time.sleep(1.0)
    for _ in range(4):
        button("select")  # watchface -> launcher -> the app it ran last
        time.sleep(1.5)
        if results_screen(shot("front")):
            return
    raise SystemExit("FAIL: could not bring pl8s to the results screen")


def normalize(max_lb):
    """Set the app to unlimited plates, 10% steps and the given max."""
    send(STEP=10, MAX_LB=max_lb, **{key: UNLIMITED for key in PLATE_KEYS})
    time.sleep(1.5)


def main():
    print(f"restarting the {EMULATOR} emulator")
    pebble("kill", timeout=60)
    time.sleep(2)
    result = pebble("install")
    check("succeeded" in result.stdout + result.stderr, "app installed")
    time.sleep(6)
    launch_app()

    # Reference frames: learn the digit templates from known good screens.
    normalize(REFERENCES[0][0])
    base = shot("base")
    check(results_screen(base), "results screen is up")

    refs = []
    for max_lb, expected in REFERENCES:
        normalize(max_lb)
        refs.append((shot(f"ref{max_lb}"), expected))
    templates = learn(refs)
    needed = set("".join(AFTER_WIZARD)).union(
        *(set("".join(rows_)) for _, rows_ in REFERENCES))
    check(needed <= set(templates),
          f"digit templates cover {''.join(sorted(needed))} "
          f"(got {''.join(sorted(templates))})")
    for path, expected in [(base, REFERENCES[0][1])] + refs:
        check(rows(path, templates) == expected,
              f"{path} shows {expected}")

    # Hold SEL: the wizard walks pct step, then one screen per plate size.
    normalize(MAX_LB)
    base = shot("base")
    signature = frame_sig(base)
    hold_select()
    signature = frame_sig(shot("open"))
    check(signature != frame_sig(base), "hold SEL opened the wizard")

    for step in range(8):  # step size + the 55..5 lb plate screens
        signature = clicks_until_change("select", signature, f"c{step}")[0]

    for step in range(10):  # 10 plates -> none, on the 2.5 lb screen
        signature = clicks_until_change("down", signature, f"d{step}")[0]

    clicks_until_change("select", signature, "apply")
    time.sleep(1.0)
    final = shot("final")
    check(results_screen(final), "confirming the last plate closes the wizard")
    check(rows(final, templates) == AFTER_WIZARD,
          f"without 2.5 lb plates the rows are {AFTER_WIZARD}")

    print(f"\nPASS - screenshots in {SHOTS}_*.png")


if __name__ == "__main__":
    main()