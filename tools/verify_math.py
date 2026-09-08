#!/usr/bin/env python3
"""Mirror of the C math in src/c/main.c (try_load / calc_row).

Verifies across plate inventories (unlimited, limited, gappy, empty) and the
full max range that every displayed weight is exactly loadable with the
configured inventory, and that the default inventory reproduces the original
nearest-rounding behavior for the old 475 lb cap.
"""
import itertools

BAR_LB = 45
PLATE_UNITS = [22, 18, 14, 10, 6, 4, 2, 1]  # 55/45/35/25/15/10/5/2.5 lb
NUM_ROWS = 5
PERCENTS = [90, 80, 70, 60, 50]
PERCENTS_FINE = [90, 85, 80, 75, 70, 65, 60]  # 5% step mode
MAX_LB_LIMIT = 995
MAX_SIDE_UNITS = 180
COUNT_UNLIMITED = 10


def try_load(units, counts):
    remaining = units
    out = []
    for i, p in enumerate(PLATE_UNITS):
        limit = remaining // p + 1 if counts[i] >= COUNT_UNLIMITED else counts[i]
        n = min(remaining // p, limit)
        out.append(n)
        remaining -= n * p
    return remaining == 0, out


def calc_row(max_lb, pct, counts):
    target_half = max_lb * pct * 2 // 100
    target_units = (target_half - BAR_LB * 2 + 5) // 10
    target_units = max(0, min(MAX_SIDE_UNITS, target_units))
    ok, out = try_load(target_units, counts)
    while not ok and target_units > 0:
        target_units -= 1
        ok, out = try_load(target_units, counts)
    shown = BAR_LB + target_units * 5
    # loadability: rebuild the per-side load greedily and check exact fit
    load_units = sum(PLATE_UNITS[i] * out[i] for i in range(len(PLATE_UNITS)))
    assert load_units == target_units, (max_lb, pct, counts, out)
    rebuild_ok, _ = try_load(load_units, counts)
    assert rebuild_ok, (max_lb, pct, counts, load_units)
    return shown, target_units


def run_suite(name, counts, max_lo, max_hi):
    fails = []
    rows = 0
    for pcts in (PERCENTS, PERCENTS_FINE):
        for max_lb in range(max_lo, max_hi + 1):
            for pct in pcts:
                rows += 1
                shown, units = calc_row(max_lb, pct, counts)
                # displayed weight must be buildable from this inventory
                ok, rebuilt = try_load(units, counts)
                if not ok or sum(PLATE_UNITS[i] * rebuilt[i] for i in range(8)) != units:
                    fails.append((max_lb, pct, shown, units))
    print(f"{name}: {'OK' if not fails else fails[:5]} ({max_hi - max_lo + 1} maxima x {rows // (max_hi - max_lo + 1)} rows)")
    return not fails


allok = True

# 1. Default (unlimited) inventory: nearest-rounding parity with the original
#    app over its old 475 lb cap, plus the full new range.
unlimited = [COUNT_UNLIMITED] * 8
for max_lb in range(45, 476):
    for pct in PERCENTS:
        target_half = max_lb * pct * 2 // 100
        old_units = max(0, (target_half - BAR_LB * 2 + 5) // 10)
        shown, units = calc_row(max_lb, pct, unlimited)
        if shown != BAR_LB + min(old_units, MAX_SIDE_UNITS) * 5:
            print(f"PARITY FAIL max={max_lb} pct={pct}: {shown} vs {BAR_LB + old_units * 5}")
            allok = False
print("parity with original rounding (unlimited, max<=475):", "OK" if allok else "FAIL")
allok = run_suite("unlimited inventory, full 45..995", unlimited, 45, 995) and allok

# 2. One of each size (the old physical assumption, now as an explicit limit)
allok = run_suite("one of each size", [1] * 8, 45, 995) and allok

# 3. Typical gym: pairs of 45/35/25/10/5, no 55/15, several 2.5
typical = [0, 2, 2, 2, 0, 2, 2, 6]
allok = run_suite("typical gym inventory", typical, 45, 995) and allok

# 4. Gappy / minimal inventories (each single size, pairs, empty)
for n in range(9):
    inv = [0] * 8
    if n < 8:
        inv[n] = 4
    allok = run_suite(f"single-size x4 ({'55 45 35 25 15 10 5 2.5'.split()[n] if n < 8 else 'empty'})", inv, 45, 995) and allok

# 5. Exhaustive small inventories: every count combo 0..3 across all 8 sizes
#    is 4^8 = 65536 inventories; sample every 7th to keep runtime sane while
#    covering correlated and gappy patterns.
count = 0
bad = 0
for combo in itertools.product(range(4), repeat=8):
    count += 1
    if count % 7:
        continue
    for max_lb in (45, 135, 225, 315, 495, 995):
        for pct in PERCENTS + PERCENTS_FINE:
            shown, units = calc_row(max_lb, pct, list(combo))
            ok, rebuilt = try_load(units, list(combo))
            if not ok or sum(PLATE_UNITS[i] * rebuilt[i] for i in range(8)) != units:
                bad += 1
                if bad <= 3:
                    print("EXHAUSTIVE FAIL", combo, max_lb, pct, shown, units)
print(f"sampling of 4^8 inventories: {'OK' if bad == 0 else str(bad) + ' failures'} (9372 inventories x 72 rows)")
allok = allok and bad == 0

print("ALL OK" if allok else "FAILURES FOUND")
