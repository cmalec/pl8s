/**
 * Plate Calc - a minimalist barbell percentage + plate calculator for Pebble.
 *
 * Enter your max for an exercise, and the app shows five descending
 * percentages (90/80/70/60/50) with the weight to load and a side-view
 * glyph of the plates needed per side.
 *
 * Assumptions: 45 lb Olympic bar; plates 55/45/35/25/15/10/5/2.5 lb.
 * Percentages are snapped down to the nearest weight buildable with the
 * configured plate inventory (per-size count, default unlimited), so every
 * weight shown can be loaded exactly.
 */

#include <pebble.h>

// ---------------------------------------------------------------------------
// Configuration
// ---------------------------------------------------------------------------

#define BAR_LB 45

// Plate weights in 2.5 lb units (55 lb = 22 units), heaviest first.
#define PLATE_N 8
static const int PLATE_UNITS[PLATE_N] = {22, 18, 14, 10, 6, 4, 2, 1};
// Human-readable plate sizes, same order as PLATE_UNITS.
static const char *const PLATE_LB_STRS[] = {"55", "45", "35", "25",
                                            "15", "10", "5",  "2.5"};

#define NUM_ROWS 5
static const int PERCENTS[NUM_ROWS] = {90, 80, 70, 60, 50};

#define PERSIST_KEY_MAX 1
#define PERSIST_KEY_SETTINGS 2
#define DEFAULT_MAX_LB 225
// UI cap for the exercise max. With unlimited plates every 2.5 lb step up to
// this max is loadable; with a limited inventory the math snaps down to the
// heaviest achievable weight below each target.
#define MAX_LB_LIMIT 995
// Per-side load cap in 2.5 lb units for the exact-fit search: (995 * 0.9
// - 45) / 2 / 2.5 = 170 units at the heaviest row, with margin.
#define MAX_SIDE_UNITS 180
#define NUDGE_LB 5

// Plate inventory: per-size count 0..COUNT_UNLIMITED. COUNT_UNLIMITED means
// "unlimited" (the default; glyph overflow is indicated by a clip marker).
#define COUNT_UNLIMITED 10
#define COUNT_DEFAULT COUNT_UNLIMITED

// ---------------------------------------------------------------------------
// Layout (derived from display size, not hardcoded 144x168)
// ---------------------------------------------------------------------------

#if defined(PBL_DISPLAY_HEIGHT) && PBL_DISPLAY_HEIGHT >= 220
#define LAYOUT_LARGE 1
#endif

#if LAYOUT_LARGE
#define HEADER_H 36
#define ROW_H 32
#define TOP_Y 44
#define PCT_W 56
#define WEIGHT_X 62
#define WEIGHT_W 78
#define GLYPH_W 58
#define FONT_HEADER FONT_KEY_GOTHIC_24_BOLD
#define FONT_PCT FONT_KEY_GOTHIC_18_BOLD
#define FONT_WEIGHT FONT_KEY_GOTHIC_24_BOLD
#define FONT_FOOTER FONT_KEY_GOTHIC_14
#define PLATE_H_BIG 20
#define PLATE_H_MID 13
#define PLATE_H_SMALL 8
#else
#define HEADER_H 28
#define ROW_H 24
#define TOP_Y 32
#define PCT_W 44
#define WEIGHT_X 48
#define WEIGHT_W 58
#define GLYPH_W 40
#define FONT_HEADER FONT_KEY_GOTHIC_18_BOLD
#define FONT_PCT FONT_KEY_GOTHIC_14_BOLD
#define FONT_WEIGHT FONT_KEY_GOTHIC_18_BOLD
#define FONT_FOOTER FONT_KEY_GOTHIC_14
#define PLATE_H_BIG 14
#define PLATE_H_MID 9
#define PLATE_H_SMALL 6
#endif

// Per-plate glyph metrics: width in px, height class.
static const int PLATE_PX_W[] = {7, 6, 5, 4, 3, 3, 2, 2};

// Muted greys vanish or dither poorly on 1-bit displays; use white there.
#if defined(PBL_COLOR)
#define COLOR_PCT GColorLightGray
#define COLOR_FOOTER GColorDarkGray
#else
#define COLOR_PCT GColorWhite
#define COLOR_FOOTER GColorWhite
#endif

// ---------------------------------------------------------------------------
// State
// ---------------------------------------------------------------------------

static Window *s_results_window;
static Layer *s_glyph_layer;
static TextLayer *s_title_layer;
static TextLayer *s_pct_layers[NUM_ROWS];
static TextLayer *s_weight_layers[NUM_ROWS];
static TextLayer *s_footer_layer;

static NumberWindow *s_number_window;

static int s_max_lb = DEFAULT_MAX_LB;

// Per-side plate inventory, indexed like PLATE_UNITS. COUNT_UNLIMITED means
// unlimited plates of that size.
static int8_t s_plate_counts[PLATE_N] = {COUNT_DEFAULT, COUNT_DEFAULT,
                                         COUNT_DEFAULT, COUNT_DEFAULT,
                                         COUNT_DEFAULT, COUNT_DEFAULT,
                                         COUNT_DEFAULT, COUNT_DEFAULT};

// ---------------------------------------------------------------------------
// Plate-count settings window (number entry)
// ---------------------------------------------------------------------------

static NumberWindow *s_count_window;
static int s_count_index;  // which plate size is being edited

// ---------------------------------------------------------------------------
// Plate math
// ---------------------------------------------------------------------------

/**
 * Greedy-fill one per-side target (2.5 lb units) from the inventory.
 * @return true if the target was hit exactly, false if plates ran out
 */
static bool try_load(int units, int *counts_out) {
  int remaining = units;
  for (int i = 0; i < PLATE_N; i++) {
    int limit = (s_plate_counts[i] >= COUNT_UNLIMITED)
                    ? remaining / PLATE_UNITS[i] + 1  // enough to not bind
                    : s_plate_counts[i];
    int n = remaining / PLATE_UNITS[i];
    if (n > limit) {
      n = limit;
    }
    counts_out[i] = n;
    remaining -= n * PLATE_UNITS[i];
  }
  return remaining == 0;
}

/**
 * Compute the displayed load for one percentage row.
 *
 * The per-side target (max_lb * pct - bar) / 2 is rounded to the nearest
 * 2.5 lb step; with a complete inventory every step is loadable. If the
 * inventory cannot build it exactly, the weight snaps DOWN to the heaviest
 * buildable weight, so every displayed weight is always loadable.
 *
 * @param max_lb the exercise max
 * @param pct the percentage (e.g. 90)
 * @param shown_lb_out receives the exact loadable total weight
 * @param counts_out receives per-side plate counts indexed like PLATE_UNITS
 */
static void calc_row(int max_lb, int pct, int *shown_lb_out, int *counts_out) {
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

  if (!try_load(target_units, counts_out)) {
    // Snap down one 2.5 lb step at a time until buildable (try_load(0)
    // always succeeds, so this terminates).
    while (target_units > 0 && !try_load(target_units, counts_out)) {
      target_units--;
    }
  }

  *shown_lb_out = BAR_LB + target_units * 5;
}

// ---------------------------------------------------------------------------
// Drawing
// ---------------------------------------------------------------------------

static GColor plate_color(int plate_index) {
#if defined(PBL_COLOR)
  // Color-code by weight class: heavy = orange, mid = white, light = grey.
  if (PLATE_UNITS[plate_index] >= 14) {
    return GColorOrange;
  } else if (PLATE_UNITS[plate_index] >= 4) {
    return GColorWhite;
  }
  return GColorLightGray;
#else
  return GColorWhite;
#endif
}

static void draw_plate(GContext *ctx, GRect zone, int plate_index, int16_t *x,
                       bool *clipped) {
  int w = PLATE_PX_W[plate_index];
  int units = PLATE_UNITS[plate_index];
  int h;
  if (units >= 10) {
    h = PLATE_H_BIG;
  } else if (units >= 4) {
    h = PLATE_H_MID;
  } else {
    h = PLATE_H_SMALL;
  }

  if (*x - w < zone.origin.x) {
    *clipped = true;
    return;
  }

  GRect r = GRect(*x - w, zone.origin.y + (zone.size.h - h) / 2, w, h);
  GColor color = plate_color(plate_index);
  graphics_context_set_fill_color(ctx, color);
  graphics_context_set_stroke_color(ctx, color);

  if (units >= 10) {
    // Solid plate with an outline to suggest thickness.
    graphics_fill_rect(ctx, r, 2, GCornersAll);
  } else {
    // Small plates are hollow so they stay readable at tiny sizes.
    graphics_draw_round_rect(ctx, r, 1);
  }
  *x -= (w + 1);
}

static void glyph_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  int row_h = bounds.size.h / NUM_ROWS;

  for (int row = 0; row < NUM_ROWS; row++) {
    GRect zone = GRect(0, row * row_h, bounds.size.w, row_h);

    int counts[PLATE_N];
    int shown_lb;
    calc_row(s_max_lb, PERCENTS[row], &shown_lb, counts);

    // Plates are drawn right to left from the collar.
    int16_t x = zone.origin.x + zone.size.w - 3;
    bool clipped = false;
    for (int i = 0; i < PLATE_N; i++) {
      for (int n = 0; n < counts[i]; n++) {
        draw_plate(ctx, zone, i, &x, &clipped);
        if (clipped) {
          break;
        }
      }
      if (clipped) {
        break;
      }
    }
    if (clipped) {
      // Too many plates to render: indicate more off-screen to the left.
      graphics_context_set_stroke_color(ctx, GColorLightGray);
      graphics_draw_line(ctx, GPoint(zone.origin.x + 1, zone.origin.y + zone.size.h / 2),
                         GPoint(zone.origin.x + 5, zone.origin.y + zone.size.h / 2));
    }

    // Collar: vertical line at the right edge of the glyph zone.
    graphics_context_set_stroke_color(ctx, GColorDarkGray);
    graphics_draw_line(ctx,
                       GPoint(zone.origin.x + zone.size.w - 1, zone.origin.y + 2),
                       GPoint(zone.origin.x + zone.size.w - 1,
                              zone.origin.y + zone.size.h - 3));
  }
}

// ---------------------------------------------------------------------------
// UI updates
// ---------------------------------------------------------------------------

static void update_ui(void);  // forward decl (used by the count window below)

// ---------------------------------------------------------------------------
// Plate-count settings window (number entry)
// ---------------------------------------------------------------------------

static void count_configure(NumberWindow *nw, int index) {
  static char label[24];
  snprintf(label, sizeof(label), "# of %s lb plates",
           PLATE_LB_STRS[index]);
  number_window_set_label(nw, label);
  // Stored 0..10 maps to NumberWindow 1..11 so "10" (the sentinel for
  // unlimited) is directly selectable.
  number_window_set_min(nw, 1);
  number_window_set_max(nw, COUNT_UNLIMITED + 1);
  number_window_set_value(nw, s_plate_counts[index] + 1);
}

static void count_selected_handler(struct NumberWindow *nw, void *context) {
  s_plate_counts[s_count_index] =
      (int8_t)number_window_get_value(nw) - 1;  // 1..11 -> 0..10
  persist_write_data(PERSIST_KEY_SETTINGS, s_plate_counts,
                     sizeof(s_plate_counts));
  APP_LOG(APP_LOG_LEVEL_INFO, "plate[%s]=%d",
          PLATE_LB_STRS[s_count_index], s_plate_counts[s_count_index]);
  s_count_index++;
  if (s_count_index < PLATE_N) {
    count_configure(nw, s_count_index);  // next plate size, in place
  } else {
    s_count_index = 0;
    window_stack_pop(true);  // wizard finished
  }
  update_ui();
}

static void open_count_window(void) {
  s_count_index = 0;
  count_configure(s_count_window, s_count_index);
  window_stack_push(number_window_get_window(s_count_window), true);
}

// ---------------------------------------------------------------------------
// UI updates
// ---------------------------------------------------------------------------

static void update_ui(void) {
  static char title_buf[32];
  static char weight_bufs[NUM_ROWS][8];

  snprintf(title_buf, sizeof(title_buf), "Max %d lb", s_max_lb);
  text_layer_set_text(s_title_layer, title_buf);

  char dbg[80];
  int off = snprintf(dbg, sizeof(dbg), "UI max=%d w=", s_max_lb);
  for (int row = 0; row < NUM_ROWS; row++) {
    int counts[PLATE_N];
    int shown_lb;
    calc_row(s_max_lb, PERCENTS[row], &shown_lb, counts);
    snprintf(weight_bufs[row], sizeof(weight_bufs[row]), "%d", shown_lb);
    text_layer_set_text(s_weight_layers[row], weight_bufs[row]);
    if (off > 0 && off < (int)sizeof(dbg)) {
      off += snprintf(dbg + off, sizeof(dbg) - off, "%s%d",
                      row ? "," : "", shown_lb);
    }
  }
  APP_LOG(APP_LOG_LEVEL_INFO, "%s", dbg);
  layer_mark_dirty(s_glyph_layer);
}

// ---------------------------------------------------------------------------
// Max entry (NumberWindow)
// ---------------------------------------------------------------------------

static void number_selected_handler(struct NumberWindow *nw, void *context) {
  s_max_lb = number_window_get_value(nw);
  if (s_max_lb > MAX_LB_LIMIT) {
    s_max_lb = MAX_LB_LIMIT;
    number_window_set_value(nw, s_max_lb);
  }
  persist_write_int(PERSIST_KEY_MAX, s_max_lb);
  update_ui();
}

static void open_number_window(void) {
  number_window_set_value(s_number_window, s_max_lb);
  window_stack_push(number_window_get_window(s_number_window), true);
}

static void results_select_handler(ClickRecognizerRef ref, void *context) {
  open_number_window();
}

static void results_select_long_handler(ClickRecognizerRef ref,
                                        void *context) {
  open_count_window();
}

static void results_nudge(int delta) {
  int next = s_max_lb + delta;
  if (next >= BAR_LB && next <= MAX_LB_LIMIT) {
    s_max_lb = next;
    persist_write_int(PERSIST_KEY_MAX, s_max_lb);
    update_ui();
  }
}

static void results_up_handler(ClickRecognizerRef ref, void *context) {
  results_nudge(NUDGE_LB);
}

static void results_down_handler(ClickRecognizerRef ref, void *context) {
  results_nudge(-NUDGE_LB);
}

static void results_click_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, results_select_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT, 500,
                              results_select_long_handler, NULL);
  window_single_repeating_click_subscribe(BUTTON_ID_UP, 350,
                                          results_up_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 350,
                                          results_down_handler);
}

// ---------------------------------------------------------------------------
// Results window setup
// ---------------------------------------------------------------------------

static TextLayer *make_text_layer(GRect frame, GFont font, GColor color,
                                  GTextAlignment align) {
  TextLayer *layer = text_layer_create(frame);
  text_layer_set_font(layer, font);
  text_layer_set_text_color(layer, color);
  text_layer_set_background_color(layer, GColorClear);
  text_layer_set_text_alignment(layer, align);
  return layer;
}

static void results_window_load(Window *window) {
  APP_LOG(APP_LOG_LEVEL_INFO, "results window load");
  Layer *root = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(root);
  int w = bounds.size.w;

  window_set_background_color(window, GColorBlack);

#if LAYOUT_LARGE
  GRect header = GRect(0, 2, w, HEADER_H);
#else
  GRect header = GRect(0, 0, w, HEADER_H);
#endif
  s_title_layer = make_text_layer(header, fonts_get_system_font(FONT_HEADER),
                                  GColorOrange, GTextAlignmentCenter);

  // Divider under the header.
  s_glyph_layer = layer_create(GRect(0, TOP_Y, w, ROW_H * NUM_ROWS));
  layer_set_update_proc(s_glyph_layer, glyph_update_proc);

  for (int row = 0; row < NUM_ROWS; row++) {
    GRect row_frame = GRect(0, TOP_Y + row * ROW_H, w, ROW_H);
    GRect pct_frame = GRect(8, row_frame.origin.y, PCT_W, ROW_H);
    GRect weight_frame = GRect(WEIGHT_X, row_frame.origin.y, WEIGHT_W, ROW_H);

    static char pct_bufs[NUM_ROWS][6];
    snprintf(pct_bufs[row], sizeof(pct_bufs[row]), "%d%%", PERCENTS[row]);

    s_pct_layers[row] = make_text_layer(
        pct_frame, fonts_get_system_font(FONT_PCT), COLOR_PCT,
        GTextAlignmentLeft);
    text_layer_set_text(s_pct_layers[row], pct_bufs[row]);

    s_weight_layers[row] = make_text_layer(
        weight_frame, fonts_get_system_font(FONT_WEIGHT), GColorWhite,
        GTextAlignmentLeft);
  }

  GRect footer = GRect(0, bounds.size.h - 22, w, 20);
  s_footer_layer = make_text_layer(footer, fonts_get_system_font(FONT_FOOTER),
                                   COLOR_FOOTER, GTextAlignmentCenter);
  text_layer_set_text(s_footer_layer,
                      "UP/DN max  SEL menu  hold SEL plates");

  // Add all layers before applying initial state (see pebble skill note).
  layer_add_child(root, text_layer_get_layer(s_title_layer));
  layer_add_child(root, s_glyph_layer);
  for (int row = 0; row < NUM_ROWS; row++) {
    layer_add_child(root, text_layer_get_layer(s_pct_layers[row]));
    layer_add_child(root, text_layer_get_layer(s_weight_layers[row]));
  }
  layer_add_child(root, text_layer_get_layer(s_footer_layer));

  update_ui();
}

static void results_window_unload(Window *window) {
  APP_LOG(APP_LOG_LEVEL_INFO, "results window unload");
  text_layer_destroy(s_title_layer);
  text_layer_destroy(s_footer_layer);
  for (int row = 0; row < NUM_ROWS; row++) {
    text_layer_destroy(s_pct_layers[row]);
    text_layer_destroy(s_weight_layers[row]);
  }
  layer_destroy(s_glyph_layer);
}

// ---------------------------------------------------------------------------
// App lifecycle
// ---------------------------------------------------------------------------

static void init(void) {
  APP_LOG(APP_LOG_LEVEL_INFO, "init begin");
  if (persist_exists(PERSIST_KEY_MAX)) {
    int stored = persist_read_int(PERSIST_KEY_MAX);
    if (stored >= BAR_LB && stored <= MAX_LB_LIMIT) {
      s_max_lb = stored;
    }
  }
  if (persist_exists(PERSIST_KEY_SETTINGS)) {
    persist_read_data(PERSIST_KEY_SETTINGS, s_plate_counts,
                      sizeof(s_plate_counts));
    for (int i = 0; i < PLATE_N; i++) {
      if (s_plate_counts[i] < 0 || s_plate_counts[i] > COUNT_UNLIMITED) {
        s_plate_counts[i] = COUNT_DEFAULT;  // corrupt/foreign blob: reset
      }
    }
  }

  s_number_window = number_window_create("Max (lb)",
      (NumberWindowCallbacks){.selected = number_selected_handler}, NULL);
  number_window_set_min(s_number_window, BAR_LB);
  number_window_set_max(s_number_window, MAX_LB_LIMIT);
  number_window_set_value(s_number_window, s_max_lb);

  s_count_window = number_window_create("Plates",
      (NumberWindowCallbacks){.selected = count_selected_handler}, NULL);

  s_results_window = window_create();
  window_set_click_config_provider(s_results_window, results_click_provider);
  window_set_window_handlers(s_results_window, (WindowHandlers){
      .load = results_window_load,
      .unload = results_window_unload,
  });
  window_stack_push(s_results_window, true);
  APP_LOG(APP_LOG_LEVEL_INFO, "init complete");
}

static void deinit(void) {
  APP_LOG(APP_LOG_LEVEL_INFO, "deinit");
  window_destroy(s_results_window);
  number_window_destroy(s_number_window);
  number_window_destroy(s_count_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
