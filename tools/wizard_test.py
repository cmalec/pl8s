#!/usr/bin/env python3
"""Drive the plate-count wizard in the emery emulator with verification.

Every button step is verified by screenshot diff: if the frame did not
change, the click was dropped by the emulator and is retried. Ends by
setting 2.5 lb plates to 0 and checking the 500 lb row snaps to 495.
"""
import subprocess
import sys
import time

sys.path.insert(0, "tools")
from png_dump import read_png  # noqa: E402

SHOTS = "shots/wiz"
PEBBLE = ["pebble", "--emulator", "emery"]


def run(*args, timeout=30):
    cmd = ["pebble"] + list(args) + ["--emulator", "emery"]
    return subprocess.run(cmd, capture_output=True, text=True, timeout=timeout)


def shot(name):
    p = run("screenshot", "--no-open", name, timeout=60)
    assert "Saved" in p.stdout + p.stderr, p.stdout + p.stderr
    return name


def frame_sig(path):
    w, h, ch, px = read_png(path)
    return (w, h, bytes(px[::7]))  # subsampled signature, fast compare


def click_verified(button, prev_sig, tag, max_tries=4):
    """Click until the frame changes (emulator drops clicks under load)."""
    for attempt in range(1, max_tries + 1):
        run("emu-button", "click", button)
        time.sleep(1.3)
        f = shot(f"{SHOTS}_{tag}.png")
        sig = frame_sig(f)
        if sig != prev_sig:
            return sig, attempt
        print(f"  drop? {button} attempt {attempt} -> retry")
    raise SystemExit(f"frame never changed for {button} ({tag})")


def hold_select():
    run("emu-button", "push", "select")
    time.sleep(0.8)
    run("emu-button", "release", "select")
    time.sleep(1.4)


def main():
    # results screen reference frame (unlimited inventory, max 995)
    base = frame_sig(shot(f"{SHOTS}_base.png"))

    hold_select()
    sig = frame_sig(shot(f"{SHOTS}_open.png"))
    if sig == base:
        raise SystemExit("wizard did not open (long-press dropped)")

    # 8 confirms: step -> 55 -> 45 -> 35 -> 25 -> 15 -> 10 -> 5 -> 2.5
    for step in range(8):
        sig, n = click_verified("select", sig, f"c{step}")
        print(f"step {step + 1}/8 confirm ok ({'retry ' if n > 1 else ''}clicks)")

    # 10 verified downs: 10 -> 0 stored on the 2.5 lb step
    for step in range(10):
        sig, n = click_verified("down", sig, f"d{step}")
        print(f"down {step + 1}/10 ok ({'retry ' if n > 1 else ''}clicks)")

    # confirm applies count 0 and pops to results
    sig, n = click_verified("select", sig, "apply")
    final = shot(f"{SHOTS}_final.png")

    # compare against the unlimited reference: only rows whose 500 lb-class
    # target needed a 2.5 plate should change
    a = read_png(base and f"{SHOTS}_base.png")
    b = read_png(final)
    w, h, ch = a[0], a[1], a[2]
    rows = {"header": (0, 52), "row1": (52, 86), "row2": (86, 120),
            "row3": (120, 154), "row4": (154, 188), "row5": (188, 222)}
    changed = []
    for name, (y0, y1) in rows.items():
        npx = 0
        for y in range(y0, min(y1, h)):
            for x in range(w):
                i = (y * w + x) * ch
                if (abs(a[3][i] - b[3][i]) > 30
                        or abs(a[3][i + 1] - b[3][i + 1]) > 30
                        or abs(a[3][i + 2] - b[3][i + 2]) > 30):
                    npx += 1
        print(f"{name}: {npx} px differ")
        if npx:
            changed.append(name)
    print("changed:", changed or "none")


if __name__ == "__main__":
    main()
