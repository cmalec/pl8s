#!/usr/bin/env python3
"""Capture the appstore screenshots for every platform pl8s supports.

Boots each platform's emulator into a factory-fresh state, drives the app to
the screens the listing shows and writes them to
appstore/screenshots/<platform>-<screen>.png. The captures are raw
device-resolution frames, unframed, which is what the appstore listing wants;
the marketing banners from tools/make_appstore_assets.py frame them.

Every screen here is reached with button presses, not AppMessage: the
screenshots then show the app's own defaults, and the flow works the same on
platforms whose emulator drops the app-message bridge (aplite does).

Needs the Pebble SDK and a build (`pebble build`). It kills the running
emulator, so don't run it next to `npm run test:emulator`. Name platforms on
the command line to capture only those.
"""
import os
import subprocess
import sys
import time

sys.path.insert(0, "tools")
from png_dump import read_png  # noqa: E402

PLATFORMS = ("aplite", "basalt", "chalk", "diorite", "emery", "flint",
             "gabbro")
OUT = "appstore/screenshots"
DARK = 0.5  # below this the results screen is not on the glass


def pebble(emulator, *args, timeout=180):
    return subprocess.run(["pebble", *args, "--emulator", emulator],
                          capture_output=True, text=True, timeout=timeout)


def install(emulator):
    result = pebble(emulator, "install")
    if "succeeded" not in result.stdout + result.stderr:
        raise SystemExit(f"FAIL: install on {emulator}: "
                         f"{result.stdout}{result.stderr}")


def button(emulator, click):
    pebble(emulator, "emu-button", "click", click)
    time.sleep(1.5)


def hold_select(emulator):
    pebble(emulator, "emu-button", "push", "select")
    time.sleep(0.8)
    pebble(emulator, "emu-button", "release", "select")
    time.sleep(1.5)


def shot(emulator, path):
    os.makedirs(os.path.dirname(path) or ".", exist_ok=True)
    result = pebble(emulator, "screenshot", "--no-open", path, timeout=90)
    if "Saved" not in result.stdout + result.stderr:
        raise SystemExit(f"FAIL: screenshot {path}: {result.stdout}{result.stderr}")
    print(f"  {path}")
    return path


def frame(path):
    return read_png(path)[3]


def dark_ratio(path):
    w, h, ch, px = read_png(path)
    return sum(1 for i in range(0, w * h * ch, ch) if px[i] < 40) / (w * h)


def reset(emulator):
    """Factory-fresh emulator, so the captures show the app's own defaults.

    `pebble wipe` only clears an emulator that is running, so the app is
    installed first to start one. The installs after the wipe register the app
    in the launcher this flow launches it from.
    """
    pebble(emulator, "kill", timeout=60)
    time.sleep(2)
    install(emulator)
    time.sleep(6)
    subprocess.run(["pebble", "wipe"], capture_output=True, timeout=120)
    pebble(emulator, "kill", timeout=60)
    time.sleep(2)
    install(emulator)
    time.sleep(6)
    install(emulator)
    time.sleep(8)


def launch(emulator):
    """Land on the results screen, from wherever a fresh emulator starts.

    Two BACKs clear anything that is up, the app itself included: some
    platforms run the installed app straight away and others sit on the
    watchface. SELECT then starts it - right away on the platforms whose
    watchface launches the last app, through the launcher elsewhere.
    """
    button(emulator, "back")
    button(emulator, "back")
    for _ in range(3):
        button(emulator, "select")
        if dark_ratio(shot(emulator, "shots/appstore_probe.png")) >= DARK:
            return
    raise SystemExit(f"FAIL: never reached the results screen on {emulator}")


def results_shot(emulator, name):
    path = shot(emulator, f"{OUT}/{emulator}-{name}.png")
    dark = dark_ratio(path)
    if dark < DARK:
        raise SystemExit(f"FAIL: {emulator} results screen not up "
                         f"(dark={dark:.2f}) for {name}")
    return path


def set_step(emulator, click):
    """Set the percentage step through the wizard.

    The step screen opens on whatever the app has persisted, so the capture
    does not assume a value: UP is the top of the screen's range (10%), DOWN
    is the bottom (5%). SELECT applies it and moves to the first plate size;
    BACK then leaves the wizard.
    """
    hold_select(emulator)
    button(emulator, click)
    button(emulator, "select")
    button(emulator, "back")


def capture(emulator):
    print(f"\n== {emulator}")
    reset(emulator)
    launch(emulator)
    set_step(emulator, "up")
    ten = results_shot(emulator, "results-10pct")

    # Settings wizard (hold SEL): pct step first, then one screen per plate
    # size. The count screens open on 99, the sentinel for unlimited.
    hold_select(emulator)
    button(emulator, "select")
    shot(emulator, f"{OUT}/{emulator}-settings-plates.png")
    button(emulator, "back")   # leave the wizard

    button(emulator, "select")  # max entry NumberWindow
    shot(emulator, f"{OUT}/{emulator}-max-entry.png")
    button(emulator, "back")

    set_step(emulator, "down")
    five = results_shot(emulator, "results-5pct")

    if frame(ten) == frame(five):
        raise SystemExit(f"FAIL: {emulator} never left the 10% rows - "
                         "was the app in front?")


def main():
    for platform in sys.argv[1:] or PLATFORMS:
        capture(platform)
    print(f"\nPASS - screenshots in {OUT}/")


if __name__ == "__main__":
    main()
