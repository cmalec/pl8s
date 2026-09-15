#include "plate_math.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

const int PLATE_UNITS[PLATE_N] = {22, 18, 14, 10, 6, 4, 2, 1};
const char *const PLATE_LB_STRS[PLATE_N] = {"55", "45", "35", "25",
                                            "15", "10", "5",  "2.5"};

// Set of reachable per-side loads: bit s is set when load s (2.5 lb units) is
// buildable from the plate sizes considered so far. 3 words is 192 bits,
// enough for the 0..MAX_SIDE_UNITS range.
typedef uint64_t bitset_t[3];

static bool bs_test(const bitset_t set, int bit) {
  return (set[bit >> 6] >> (bit & 63)) & 1;
}

static void bs_set(bitset_t set, int bit) {
  set[bit >> 6] |= (uint64_t)1 << (bit & 63);
}

/** set <<= bits, in place; loads past the window fall off the end. */
static void bs_shift(bitset_t set, int bits) {
  int words = bits >> 6;
  int shift = bits & 63;
  uint64_t out[3] = {0, 0, 0};
  for (int i = 2; i >= 0; i--) {
    int from = i - words;
    if (from < 0) {
      continue;
    }
    out[i] = set[from] << shift;
    if (shift != 0 && from > 0) {
      out[i] |= set[from - 1] >> (64 - shift);
    }
  }
  memcpy(set, out, sizeof out);
}

/**
 * How many plates of size i may be used: the inventory, capped at what fits
 * under the target. Unlimited inventory only binds through what fits.
 */
static int plate_limit(const int counts[PLATE_N], int i, int units) {
  int fit = units / PLATE_UNITS[i];
  int have = counts[i];
  if (have >= COUNT_UNLIMITED) {
    return fit;
  }
  return have < fit ? have : fit;
}

int plate_math_best_load(const int counts[PLATE_N], int units,
                         int out[PLATE_N]) {
  if (units > MAX_SIDE_UNITS) {
    units = MAX_SIDE_UNITS;
  }
  if (units < 0) {
    units = 0;
  }

  // reach[i] holds the loads buildable from sizes i..PLATE_N-1 alone. Built
  // lightest size first, so the plates behind any load can afterwards be read
  // back heaviest first.
  static bitset_t reach[PLATE_N + 1];
  memset(reach[PLATE_N], 0, sizeof(bitset_t));
  bs_set(reach[PLATE_N], 0);  // no plates left to place, no load left to cover

  for (int i = PLATE_N - 1; i >= 0; i--) {
    memcpy(reach[i], reach[i + 1], sizeof(bitset_t));
    bitset_t with;  // the same loads, plus k plates of this size
    memcpy(with, reach[i + 1], sizeof(bitset_t));
    int limit = plate_limit(counts, i, units);
    for (int k = 0; k < limit; k++) {
      bs_shift(with, PLATE_UNITS[i]);
      for (int w = 0; w < 3; w++) {
        reach[i][w] |= with[w];
      }
    }
  }

  int load = units;
  while (load > 0 && !bs_test(reach[0], load)) {
    load--;
  }

  // The plates for it, heaviest size first: take as many as leave a remainder
  // the lighter sizes can still build on their own. With a complete inventory
  // this is the familiar heaviest-first fill; with plates missing it is the
  // heaviest mix that still adds up exactly.
  int left = load;
  for (int i = 0; i < PLATE_N; i++) {
    int u = PLATE_UNITS[i];
    int k = plate_limit(counts, i, left);
    while (k > 0 && !bs_test(reach[i + 1], left - k * u)) {
      k--;
    }
    out[i] = k;
    left -= k * u;
  }
  return load;
}

void plate_math_calc_row(const int counts[PLATE_N], int max_lb, int pct,
                         int *shown_lb_out, int out[PLATE_N]) {
  // Target in half-pounds (exact for .5 totals): max_lb * pct * 2 / 100.
  int target_half = max_lb * pct * 2 / 100;
  // Per-side target in 2.5 lb units (10 half-pounds each), rounded nearest.
  int target_units = (target_half - BAR_LB * 2 + 5) / 10;
  if (target_units > MAX_SIDE_UNITS) {
    target_units = MAX_SIDE_UNITS;
  }
  if (target_units < 0) {
    target_units = 0;
  }

  int load = plate_math_best_load(counts, target_units, out);
  *shown_lb_out = BAR_LB + load * 5;
}
