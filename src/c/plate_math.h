/**
 * Plate math: how much to load and which plates go on the bar.
 *
 * Pure integer arithmetic with no Pebble SDK dependency, so this source
 * builds both into the watchapp and into the host unit test
 * (test/test_plate_math.c), which pins the behavior down.
 */
#pragma once

// 45 lb Olympic bar.
#define BAR_LB 45

// Plate weights in 2.5 lb units (55 lb = 22 units), heaviest first.
#define PLATE_N 8
extern const int PLATE_UNITS[PLATE_N];
// Human-readable plate sizes, same order as PLATE_UNITS.
extern const char *const PLATE_LB_STRS[PLATE_N];

// Per-size plate inventory, 0..COUNT_UNLIMITED. COUNT_UNLIMITED means
// "unlimited" (the default; glyph overflow is indicated by a clip marker).
#define COUNT_UNLIMITED 10

// UI cap for the exercise max. With unlimited plates every 2.5 lb step up to
// this max is loadable; with a limited inventory the math snaps down to the
// heaviest achievable weight below each target.
#define MAX_LB_LIMIT 995

// Per-side load cap in 2.5 lb units: (995 * 0.9 - 45) / 2 / 2.5 = 170 units
// at the heaviest row, with margin.
#define MAX_SIDE_UNITS 180

/**
 * Heaviest load <= units (per-side, in 2.5 lb units) the inventory can build
 * exactly.
 *
 * Plates are handed out heaviest first whenever that can express the load, so
 * the counts are the fill a lifter expects and not a pile of small plates
 * that merely adds up. The glyph draws these counts, so the choice is
 * user-visible.
 *
 * @param counts per-size inventory, indexed like PLATE_UNITS
 * @param units per-side target load in 2.5 lb units
 * @param out receives the per-side plate counts used, 0 where unused
 * @return the load in 2.5 lb units (0 when nothing fits)
 */
int plate_math_best_load(const int counts[PLATE_N], int units, int out[PLATE_N]);

/**
 * Compute the displayed load for one percentage row.
 *
 * The per-side target (max_lb * pct - bar) / 2 is rounded to the nearest
 * 2.5 lb step; with a complete inventory every step is loadable. If the
 * inventory cannot build that exactly, the weight snaps DOWN to the heaviest
 * buildable weight, so every displayed weight is always loadable.
 *
 * @param counts per-size inventory, indexed like PLATE_UNITS
 * @param shown_lb_out receives the exact loadable total weight
 * @param out receives the per-side plate counts for that weight
 */
void plate_math_calc_row(const int counts[PLATE_N], int max_lb, int pct,
                         int *shown_lb_out, int out[PLATE_N]);
