/**
 * Host unit test for src/c/plate_math.c -- the same source the watchapp
 * builds, so what is tested here is the code that ships. Run with `npm test`.
 *
 * The oracle is deliberately a different algorithm from the app's: an
 * exhaustive, memoized choice of how many plates of each size to use, rather
 * than the app's reachable-load bitset. Two implementations agreeing is
 * evidence; a copy of the app's own logic would only be a restatement.
 *
 * For every percentage row of every inventory tried, it checks that the
 * shown weight is
 *   - the heaviest loadable weight <= the row's rounded target (nothing
 *     heavier that the inventory can load is passed over),
 *   - built from exactly the plates it reports, and
 *   - buildable within the per-size inventory counts.
 */
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "plate_math.h"

static int checks;
static int failures;

// Both step modes (10%: 90..50, 5%: 90..60).
static const int PERCENTS[] = {90, 85, 80, 75, 70, 65, 60, 50};
#define N_PERCENTS ((int)(sizeof(PERCENTS) / sizeof(PERCENTS[0])))

// Maxima used by the wide inventory sweeps; the curated inventories sweep
// every max instead.
static const int SAMPLE_MAX[] = {45, 120, 225, 315, 405, 495, 705, 995};
#define N_SAMPLE_MAX ((int)(sizeof(SAMPLE_MAX) / sizeof(SAMPLE_MAX[0])))

static void show_inventory(const int counts[PLATE_N], char *buf, size_t size) {
  size_t at = 0;
  for (int i = 0; i < PLATE_N && at < size; i++) {
    int wrote = snprintf(buf + at, size - at, "%s%s:%d", i ? " " : "",
                         PLATE_LB_STRS[i], counts[i]);
    if (wrote < 0) {
      break;
    }
    at += (size_t)wrote;
  }
}

// ---------------------------------------------------------------------------
// Oracle: exhaustive, memoized search over the plate multiset.
// ---------------------------------------------------------------------------

static short oracle_memo[PLATE_N + 1][MAX_SIDE_UNITS + 1];

/** Heaviest load <= room using sizes >= size, choosing every copy by hand. */
static int oracle_from(const int counts[PLATE_N], int size, int room) {
  if (size == PLATE_N) {
    return 0;
  }
  short *slot = &oracle_memo[size][room];
  if (*slot >= 0) {
    return *slot;
  }
  int u = PLATE_UNITS[size];
  int limit = room / u;
  if (counts[size] < COUNT_UNLIMITED && counts[size] < limit) {
    limit = counts[size];
  }
  int best = 0;
  for (int k = 0; k <= limit; k++) {
    int load = k * u + oracle_from(counts, size + 1, room - k * u);
    if (load > best) {
      best = load;
    }
  }
  *slot = (short)best;
  return best;
}

/** Cached per inventory (the memo keys on room, not on counts). */
static void oracle_reset(void) { memset(oracle_memo, -1, sizeof oracle_memo); }

static int oracle_best(const int counts[PLATE_N], int units) {
  if (units > MAX_SIDE_UNITS) {
    units = MAX_SIDE_UNITS;
  }
  return units < 0 ? 0 : oracle_from(counts, 0, units);
}

/**
 * Test-side heaviest-first fill: the plate choice the app promises (largest
 * plate that fits, worked down through the sizes).
 */
static bool canonical_fill(const int counts[PLATE_N], int units,
                           int out[PLATE_N]) {
  int remaining = units;
  for (int i = 0; i < PLATE_N; i++) {
    int n = remaining / PLATE_UNITS[i];
    int have = counts[i] >= COUNT_UNLIMITED ? n : counts[i];
    if (n > have) {
      n = have;
    }
    out[i] = n;
    remaining -= n * PLATE_UNITS[i];
  }
  return remaining == 0;
}

/**
 * The documented target for one row: max * pct, per side, rounded to the
 * nearest 2.5 lb step, capped at the per-side load limit.
 */
static int target_units(int max_lb, int pct) {
  int half = max_lb * pct * 2 / 100;
  int units = (half - BAR_LB * 2 + 5) / 10;
  if (units > MAX_SIDE_UNITS) {
    units = MAX_SIDE_UNITS;
  }
  return units < 0 ? 0 : units;
}

static void check_row(const int counts[PLATE_N], int max_lb, int pct) {
  int used[PLATE_N];
  int shown = 0;
  plate_math_calc_row(counts, max_lb, pct, &shown, used);
  checks++;

  int want = BAR_LB + oracle_best(counts, target_units(max_lb, pct)) * 5;
  int loaded = 0;
  bool over = false;
  for (int i = 0; i < PLATE_N; i++) {
    loaded += used[i] * PLATE_UNITS[i];
    if (used[i] < 0 || (counts[i] < COUNT_UNLIMITED && used[i] > counts[i])) {
      over = true;
    }
  }

  const char *why = NULL;
  if (shown != want) {
    why = "not the heaviest loadable weight";
  } else if (BAR_LB + loaded * 5 != shown) {
    why = "shown weight does not match the plates reported";
  } else if (over) {
    why = "uses more plates than the inventory holds";
  } else {
    int fill[PLATE_N];
    if (canonical_fill(counts, (shown - BAR_LB) / 5, fill) &&
        memcmp(fill, used, sizeof fill) != 0) {
      why = "plates are not the heaviest-first fill";
    }
  }
  if (why == NULL) {
    return;
  }

  failures++;
  if (failures <= 20) {
    char inv[128];
    show_inventory(counts, inv, sizeof inv);
    printf("FAIL %s | max=%d %d%% | shown=%d want=%d | %s\n", inv, max_lb, pct,
           shown, want, why);
  }
}

static void sweep(const char *name, const int counts[PLATE_N], int max_from,
                  int max_to, int max_step) {
  int before = failures;
  oracle_reset();
  for (int max_lb = max_from; max_lb <= max_to; max_lb += max_step) {
    for (int p = 0; p < N_PERCENTS; p++) {
      check_row(counts, max_lb, PERCENTS[p]);
    }
  }
  printf("%-42s %s\n", name, failures == before ? "ok" : "FAILED");
}

// ---------------------------------------------------------------------------
// Cases
// ---------------------------------------------------------------------------

// Default inventory, and the reference for the app's original rounding: with
// every size unlimited each row is the nearest 2.5 lb step below its target.
static const int UNLIMITED[PLATE_N] = {COUNT_UNLIMITED, COUNT_UNLIMITED,
                                       COUNT_UNLIMITED, COUNT_UNLIMITED,
                                       COUNT_UNLIMITED, COUNT_UNLIMITED,
                                       COUNT_UNLIMITED, COUNT_UNLIMITED};
static const int ONE_EACH[PLATE_N] = {1, 1, 1, 1, 1, 1, 1, 1};
static const int TYPICAL_GYM[PLATE_N] = {0, 2, 2, 2, 0, 2, 2, 6};
static const int EMPTY[PLATE_N] = {0, 0, 0, 0, 0, 0, 0, 0};

// Rows the greedy first-fit this replaced got wrong: it took a big plate it
// could not finish the load with, then snapped below a weight the inventory
// can load. Hand-computed expectations, independent of either implementation.
static const struct {
  int counts[PLATE_N];
  int max_lb, pct, shown;
} REGRESSIONS[] = {
    // target 13 units (110 lb): first-fit took the single 25 lb plate and
    // showed 105; 15 + 15 + 2.5 = 32.5 lb/side loads 110.
    {{3, 3, 0, 1, 3, 1, 0, 2}, 120, 90, 110},
    // 5% mode, target 60 units (345 lb): first-fit took 55 + 45 + 35 + 10 and
    // showed 335; 55 + 45 + 25 + 25 loads 345.
    {{1, 1, 10, 2, 0, 2, 0, 0}, 460, 75, 345},
};
#define N_REGRESSIONS ((int)(sizeof(REGRESSIONS) / sizeof(REGRESSIONS[0])))

// The glyph draws these counts, so the plates for a row are observable too:
// the heaviest-first fill, never a heap of small plates that happens to add
// up (a variant of this code filled a row with thirty-two 2.5 lb plates).
static const struct {
  int counts[PLATE_N];
  int max_lb, pct, shown;
  int plates[PLATE_N];
} PLATE_CHOICE[] = {
    // 225 max, 90% -> 205 lb = 45 bar + 55 + 25 per side.
    {{COUNT_UNLIMITED, COUNT_UNLIMITED, COUNT_UNLIMITED, COUNT_UNLIMITED,
      COUNT_UNLIMITED, COUNT_UNLIMITED, COUNT_UNLIMITED, COUNT_UNLIMITED},
     225, 90, 205, {1, 0, 0, 1, 0, 0, 0, 0}},
    // 315 max, 80% -> 250 lb = 45 bar + 55 + 45 + 2.5 per side.
    {{COUNT_UNLIMITED, COUNT_UNLIMITED, COUNT_UNLIMITED, COUNT_UNLIMITED,
      COUNT_UNLIMITED, COUNT_UNLIMITED, COUNT_UNLIMITED, COUNT_UNLIMITED},
     315, 80, 250, {1, 1, 0, 0, 0, 0, 0, 1}},
};
#define N_PLATE_CHOICE ((int)(sizeof(PLATE_CHOICE) / sizeof(PLATE_CHOICE[0])))

// Deterministic LCG: same sweep everywhere, no seed or OS differences.
static unsigned rnd_state = 12345;
static int rnd(int n) {
  rnd_state = rnd_state * 1103515245u + 12345u;
  return (int)((rnd_state >> 16) % (unsigned)n);
}

static void check_regressions(void) {
  int before = failures;
  for (int r = 0; r < N_REGRESSIONS; r++) {
    int used[PLATE_N];
    int shown = 0;
    plate_math_calc_row(REGRESSIONS[r].counts, REGRESSIONS[r].max_lb,
                        REGRESSIONS[r].pct, &shown, used);
    checks++;
    if (shown == REGRESSIONS[r].shown) {
      continue;
    }
    failures++;
    char inv[128];
    show_inventory(REGRESSIONS[r].counts, inv, sizeof inv);
    printf("FAIL regression: %s max=%d %d%% shown=%d want=%d\n", inv,
           REGRESSIONS[r].max_lb, REGRESSIONS[r].pct, shown,
           REGRESSIONS[r].shown);
  }
  printf("%-42s %s\n", "regressions", failures == before ? "ok" : "FAILED");
}

// The plates for a row are user-visible (the glyph draws them), so pin the
// exact fill as well as the weight.
static void check_plate_choice(void) {
  int before = failures;
  for (int c = 0; c < N_PLATE_CHOICE; c++) {
    int used[PLATE_N];
    int shown = 0;
    plate_math_calc_row(PLATE_CHOICE[c].counts, PLATE_CHOICE[c].max_lb,
                        PLATE_CHOICE[c].pct, &shown, used);
    checks++;
    if (shown == PLATE_CHOICE[c].shown &&
        memcmp(used, PLATE_CHOICE[c].plates, sizeof used) == 0) {
      continue;
    }
    failures++;
    printf("FAIL plate choice: max=%d %d%% shown=%d want=%d plates=",
           PLATE_CHOICE[c].max_lb, PLATE_CHOICE[c].pct, shown,
           PLATE_CHOICE[c].shown);
    for (int i = 0; i < PLATE_N; i++) {
      printf("%s%s%s", i ? "+" : "", used[i] ? "x" : "",
             used[i] ? PLATE_LB_STRS[i] : "");
    }
    printf("\n");
  }
  printf("%-42s %s\n", "plate choice", failures == before ? "ok" : "FAILED");
}

static void check_clamping(void) {
  int before = failures;
  checks += 3;
  int used[PLATE_N];

  if (plate_math_best_load(UNLIMITED, MAX_SIDE_UNITS + 40, used) !=
      MAX_SIDE_UNITS) {
    failures++;
    printf("FAIL target above the per-side cap is not clamped\n");
  }
  if (plate_math_best_load(UNLIMITED, -5, used) != 0) {
    failures++;
    printf("FAIL negative target is not clamped to zero\n");
  }
  int empty_ok = true;
  for (int u = 0; u <= MAX_SIDE_UNITS; u += 7) {
    if (plate_math_best_load(EMPTY, u, used) != 0) {
      empty_ok = false;
    }
  }
  if (!empty_ok) {
    failures++;
    printf("FAIL empty inventory loads something\n");
  }
  printf("%-42s %s\n", "target clamping", failures == before ? "ok" : "FAILED");
}

static void check_exhaustive_small(void) {
  int before = failures;
  int counts[PLATE_N] = {0};
  long inventories = 0;
  // Every inventory with 0..2 of each size (3^8 = 6561).
  for (;;) {
    oracle_reset();
    for (int m = 0; m < N_SAMPLE_MAX; m++) {
      for (int p = 0; p < N_PERCENTS; p++) {
        check_row(counts, SAMPLE_MAX[m], PERCENTS[p]);
      }
    }
    inventories++;

    int i = 0;
    while (i < PLATE_N && ++counts[i] > 2) {
      counts[i] = 0;
      i++;
    }
    if (i == PLATE_N) {
      break;
    }
  }
  printf("%-42s %s (%ld inventories)\n", "all 0..2 inventories",
         failures == before ? "ok" : "FAILED", inventories);
}

static void check_random_inventories(void) {
  int before = failures;
  for (int trial = 0; trial < 400; trial++) {
    int counts[PLATE_N];
    for (int i = 0; i < PLATE_N; i++) {
      int v = rnd(12);  // 0..9 counts, 10+ means unlimited
      counts[i] = v > COUNT_UNLIMITED ? COUNT_UNLIMITED : v;
    }
    oracle_reset();
    for (int m = 0; m < N_SAMPLE_MAX; m++) {
      for (int p = 0; p < N_PERCENTS; p++) {
        check_row(counts, SAMPLE_MAX[m], PERCENTS[p]);
      }
    }
  }
  printf("%-42s %s (400 inventories)\n", "random inventories 0..9",
         failures == before ? "ok" : "FAILED");
}

int main(void) {
  printf("plate math: rows are the heaviest loadable weight <= target\n\n");

  sweep("unlimited inventory (all maxima)", UNLIMITED, BAR_LB, MAX_LB_LIMIT, 1);
  sweep("one of each size (all maxima)", ONE_EACH, BAR_LB, MAX_LB_LIMIT, 1);
  sweep("typical gym (all maxima)", TYPICAL_GYM, BAR_LB, MAX_LB_LIMIT, 1);

  for (int n = 0; n <= PLATE_N; n++) {
    int counts[PLATE_N] = {0};
    char name[64];
    if (n < PLATE_N) {
      counts[n] = 4;
      snprintf(name, sizeof name, "%s lb only x4 (all maxima)",
               PLATE_LB_STRS[n]);
    } else {
      snprintf(name, sizeof name, "empty inventory (all maxima)");
    }
    sweep(name, counts, BAR_LB, MAX_LB_LIMIT, 1);
  }

  check_exhaustive_small();
  check_random_inventories();
  check_clamping();
  check_plate_choice();
  check_regressions();

  printf("\n%d rows checked, %d failures\n", checks, failures);
  return failures == 0 ? 0 : 1;
}
