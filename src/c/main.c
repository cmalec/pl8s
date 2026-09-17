/**
 * pl8s - "plates", a minimalist barbell percentage + plate calculator for
 * Pebble.
 *
 * Enter your max for an exercise, and the app shows descending
 * percentages (90/80/70/60/50 or 90..60 in 5% steps) with the weight to
 * load and a glyph of the plates needed per side. Large color displays
 * (emery/gabbro) draw each plate as a thick disc with its weight printed
 * horizontally on the face; the backlight stays on while the app is
 * foregrounded. Settings can be changed on the watch (hold SEL) or from
 * the phone app's config page (src/pkjs/index.js + config.html).
 *
 * Assumptions: 45 lb Olympic bar; plates 55/45/35/25/15/10/5/2.5 lb.
 * Percentages are snapped down to the nearest weight buildable with the
 * configured plate inventory (per-size count, default unlimited), so every
 * weight shown can be loaded exactly.
 */

#include <pebble.h>

#include "plate_math.h"

// ---------------------------------------------------------------------------
// Configuration
// ---------------------------------------------------------------------------

// Percentage rows: 10% step shows 90..50 (5 rows), 5% step 90..60 (7 rows).
// The step is user-selectable in the settings wizard and persisted.
#define MAX_ROWS 7
#define PCT_STEP_FINE 5
#define PCT_STEP_DEFAULT 10

#define PERSIST_KEY_MAX 1
#define PERSIST_KEY_SETTINGS 2
#define PERSIST_KEY_STEP 3
#define DEFAULT_MAX_LB 225
#define NUDGE_LB 5

#define COUNT_DEFAULT COUNT_UNLIMITED

// ---------------------------------------------------------------------------
// Layout (derived from display size, not hardcoded 144x168)
// ---------------------------------------------------------------------------

#if defined(PBL_DISPLAY_HEIGHT) && PBL_DISPLAY_HEIGHT >= 220
#define LAYOUT_LARGE 1
#endif

#if LAYOUT_LARGE
// Large rect displays (emery 200x228, gabbro 260x260): each row gets a tall
// band with big labeled plate discs. ROW heights are computed at window load
// from the actual display height (see results_window_load).
#define HEADER_H 48
#define TOP_Y 52
#define PCT_W 28
#define WEIGHT_X 34
#define WEIGHT_W 55
#define HINT_Y 30
#define FONT_HEADER FONT_KEY_GOTHIC_24_BOLD
#define FONT_HINT FONT_KEY_GOTHIC_14
#define FONT_PCT FONT_KEY_GOTHIC_14_BOLD
#define FONT_WEIGHT FONT_KEY_GOTHIC_24_BOLD
// Plate disc classes (width x height), indexed by weight via plate_w()/plate_h().
// Width is the side-view "thickness"; faces are wide enough for the
// horizontal weight labels.
#define PLATE_W_BIG 26
#define PLATE_H_BIG 22
#define PLATE_W_MID 20
#define PLATE_H_MID 18
#define PLATE_W_SMALL 17
#define PLATE_H_SMALL 15
// Compact set for 5% step (7 rows): tighter plates so all rows fit.
#define PLATE_W_BIG_C 21
#define PLATE_H_BIG_C 18
#define PLATE_W_MID_C 17
#define PLATE_H_MID_C 15
#define PLATE_W_SMALL_C 15
#define PLATE_H_SMALL_C 13
#define PLATE_GAP 2
#define PLATE_RADIUS 3
#else
#define HEADER_H 28
#define ROW_H 24
#define TOP_Y 32
#define PCT_W 44
#define WEIGHT_X 48
#define WEIGHT_W 58
#define FONT_HEADER FONT_KEY_GOTHIC_18_BOLD
#define FONT_PCT FONT_KEY_GOTHIC_14_BOLD
#define FONT_WEIGHT FONT_KEY_GOTHIC_18_BOLD
#define FONT_FOOTER FONT_KEY_GOTHIC_14
#define PLATE_H_BIG 14
#define PLATE_H_MID 9
#define PLATE_H_SMALL 6
#define PLATE_GAP 1
#define PLATE_RADIUS 1
#endif

// Plate disc size by weight class. Large layouts draw fat labeled discs;
// compact layouts keep the thin side-view bars (PLATE_PX_W).
// ---------------------------------------------------------------------------
// Runtime layout state (step selector, row count, results window geometry)
// ---------------------------------------------------------------------------

static int s_pct_step = PCT_STEP_DEFAULT;  // 5 or 10
static int s_num_rows = 5;                 // 90..50 (10%) or 90..60 (5%)
static int s_row_h;                        // computed at window load
static Layer *s_root;                      // results root layer

// Are we in the compact 5%-step layout (7 rows)? Large displays shrink the
// discs so the taller row stack fits; small layouts keep the thin bars.
#if LAYOUT_LARGE
static bool compact_plates(void) { return s_num_rows > 5; }
#endif

static int plate_w(int i) {
#if LAYOUT_LARGE
  int u = PLATE_UNITS[i];
  if (u >= 10) {
    return compact_plates() ? PLATE_W_BIG_C : PLATE_W_BIG;
  } else if (u >= 4) {
    return compact_plates() ? PLATE_W_MID_C : PLATE_W_MID;
  }
  return compact_plates() ? PLATE_W_SMALL_C : PLATE_W_SMALL;
#else
  static const int PLATE_PX_W[] = {7, 6, 5, 4, 3, 3, 2, 2};
  return PLATE_PX_W[i];
#endif
}

static int plate_h(int i) {
  int u = PLATE_UNITS[i];
#if LAYOUT_LARGE
  if (u >= 10) {
    return compact_plates() ? PLATE_H_BIG_C : PLATE_H_BIG;
  } else if (u >= 4) {
    return compact_plates() ? PLATE_H_MID_C : PLATE_H_MID;
  }
  return compact_plates() ? PLATE_H_SMALL_C : PLATE_H_SMALL;
#else
  if (u >= 10) {
    return PLATE_H_BIG;
  } else if (u >= 4) {
    return PLATE_H_MID;
  }
  return PLATE_H_SMALL;
#endif
}

// Round displays cut the corners off: row content is inset from the screen
// edge so it stays inside the circular mask. The mask narrows toward the top
// and bottom of the glass, so the inset is per row; rect displays keep the
// small EDGE_INSET everywhere.
#if defined(PBL_ROUND)
#define EDGE_INSET 10
#else
#define EDGE_INSET 0
#endif

// Compact round displays are at their narrowest right at the top of the
// glass: the title sits a little lower, where the mask's chord is wide enough
// for it, "(5%)" suffix included.
#if defined(PBL_ROUND) && !LAYOUT_LARGE
#define HEADER_Y 14
#else
#define HEADER_Y 0
#endif

// Rows stop short of the bottom of the glass by this much: at the very bottom
// the mask's chord is too narrow to hold a row.
#define BOTTOM_MARGIN (EDGE_INSET ? 40 : 4)

#if defined(PBL_ROUND)
/** Integer square root (Newton's method), for the mask geometry below. */
static int isqrt(int n) {
  if (n <= 0) {
    return 0;
  }
  int x = n;
  for (int i = 0; i < 16; i++) {
    int next = (x + n / x) / 2;
    if (next >= x) {
      break;
    }
    x = next;
  }
  return x;
}
#endif

/**
 * How far a row's content stays from the screen edge to clear the mask. The
 * narrowest point of the row's band is the edge further from the middle, so
 * that edge is what the inset is computed from.
 */
static int row_inset(int w, int h, int row_y, int row_h) {
#if defined(PBL_ROUND)
  int r = w / 2;
  int bottom = row_y + row_h - h / 2;
  int top = h / 2 - row_y;
  int far = bottom > top ? bottom : top;
  if (far < 0) {
    far = -far;
  }
  return far >= r ? r : r - isqrt(r * r - far * far) + 4;
#else
  (void)w;
  (void)h;
  (void)row_y;
  (void)row_h;
  return EDGE_INSET;
#endif
}

// Muted greys vanish or dither poorly on 1-bit displays; use white there.
#if defined(PBL_COLOR)
#define COLOR_PCT GColorLightGray
#define COLOR_FOOTER GColorDarkGray
#define COLOR_TITLE GColorOrange
#else
#define COLOR_PCT GColorWhite
#define COLOR_FOOTER GColorWhite
#define COLOR_TITLE GColorWhite  // orange is black on 1-bit displays
#endif

// ---------------------------------------------------------------------------
// State
// ---------------------------------------------------------------------------

static Window *s_results_window;
static Layer *s_glyph_layer;
static TextLayer *s_title_layer;
#if LAYOUT_LARGE
static TextLayer *s_hint_layer;  // header hint line
#endif
static TextLayer *s_pct_layers[MAX_ROWS];
static TextLayer *s_weight_layers[MAX_ROWS];
#if !LAYOUT_LARGE
static TextLayer *s_footer_layer;
#endif

// Rotated weight labels, one per plate size, pre-rendered resources
// (large color layouts only; NULL elsewhere).
#if defined(PBL_COLOR) && LAYOUT_LARGE
static GBitmap *s_label_bmps[PLATE_N];
#endif

static NumberWindow *s_number_window;

static int s_max_lb = DEFAULT_MAX_LB;

// Per-side plate inventory, indexed like PLATE_UNITS. COUNT_UNLIMITED means
// unlimited plates of that size. Persisted as a raw blob: changing the
// element type needs the read-back size check in init().
static int s_plate_counts[PLATE_N] = {COUNT_DEFAULT, COUNT_DEFAULT,
                                      COUNT_DEFAULT, COUNT_DEFAULT,
                                      COUNT_DEFAULT, COUNT_DEFAULT,
                                      COUNT_DEFAULT, COUNT_DEFAULT};

// A count is either a literal 0..COUNT_MAX_LITERAL or the sentinel.
static bool count_valid(int v) {
  return v >= 0 && (v <= COUNT_MAX_LITERAL || v == COUNT_UNLIMITED);
}

// Snap any other value into the valid set: below none stays none, the gap
// between the literal counts and the sentinel means "more than the glyph can
// draw" and becomes the largest literal count, past the sentinel is the
// sentinel.
static int count_clamp(int v) {
  if (v < 0) {
    return 0;
  }
  if (v > COUNT_MAX_LITERAL && v < COUNT_UNLIMITED) {
    return COUNT_MAX_LITERAL;
  }
  return v > COUNT_UNLIMITED ? COUNT_UNLIMITED : v;
}

// ---------------------------------------------------------------------------
// Plate-count settings window (number entry)
// ---------------------------------------------------------------------------

static NumberWindow *s_count_window;
static int s_count_index;  // which plate size is being edited

// ---------------------------------------------------------------------------
// Drawing
// ---------------------------------------------------------------------------

static GColor plate_color(int plate_index) {
#if defined(PBL_COLOR)
  // Gym-standard color per plate size: red/blue/yellow/green/pink/white/cyan.
  switch (PLATE_UNITS[plate_index]) {
    case 22: return GColorRed;        // 55
    case 18: return GColorBlue;       // 45
    case 14: return GColorYellow;     // 35
    case 10: return GColorGreen;      // 25
    case 6:  return GColorShockingPink;  // 15
    case 4:  return GColorWhite;      // 10
    case 2:  return GColorCyan;       // 5
    default: return GColorLightGray;  // 2.5
  }
#else
  return GColorWhite;
#endif
}

static void draw_plate_rect(GContext *ctx, GRect zone, int plate_index,
                            int16_t *x, bool *clipped) {
  int w = plate_w(plate_index);
  int h = plate_h(plate_index);

  if (*x - w < zone.origin.x) {
    *clipped = true;
    return;
  }

  GRect r = GRect(*x - w, zone.origin.y + (zone.size.h - h) / 2, w, h);
  GColor color = plate_color(plate_index);
  graphics_context_set_fill_color(ctx, color);
  graphics_context_set_stroke_color(ctx, color);

#if LAYOUT_LARGE
  // Large layouts: solid discs in every class so the labels stay readable.
  graphics_fill_rect(ctx, r, PLATE_RADIUS, GCornersAll);
#else
  if (PLATE_UNITS[plate_index] >= 10) {
    // Solid plate with an outline to suggest thickness.
    graphics_fill_rect(ctx, r, 2, GCornersAll);
  } else {
    // Small plates are hollow so they stay readable at tiny sizes.
    graphics_draw_round_rect(ctx, r, 1);
  }
#endif
  *x -= (w + PLATE_GAP);
}

// Composite the horizontal weight label over a drawn plate. Large color only.
#if defined(PBL_COLOR) && LAYOUT_LARGE
static void draw_plate_label(GContext *ctx, GRect zone, int plate_index,
                             int16_t *x, bool *clipped) {
  int w = plate_w(plate_index);
  int h = plate_h(plate_index);
  if (*x - w < zone.origin.x) {
    *clipped = true;
    return;
  }
  GRect r = GRect(*x - w, zone.origin.y + (zone.size.h - h) / 2, w, h);
  if (s_label_bmps[plate_index] != NULL) {
    GRect lb = gbitmap_get_bounds(s_label_bmps[plate_index]);
    GRect lr = GRect(r.origin.x + (r.size.w - lb.size.w) / 2,
                     r.origin.y + (r.size.h - lb.size.h) / 2,
                     lb.size.w, lb.size.h);
    graphics_context_set_compositing_mode(ctx, GCompOpSet);
    graphics_draw_bitmap_in_rect(ctx, s_label_bmps[plate_index], lr);
    graphics_context_set_compositing_mode(ctx, GCompOpAssign);
  }
  *x -= (w + PLATE_GAP);
}
#endif  // PBL_COLOR && LAYOUT_LARGE

static void glyph_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GRect glass = layer_get_bounds(s_root);
  int row_h = bounds.size.h / s_num_rows;

  for (int row = 0; row < s_num_rows; row++) {
    GRect zone = GRect(0, row * row_h, bounds.size.w, row_h);

    int counts[PLATE_N];
    int shown_lb;
    plate_math_calc_row(s_plate_counts, s_max_lb, 90 - row * s_pct_step,
                        &shown_lb, counts);

    // Plates hang from the collar at the right edge, heaviest innermost. On
    // round displays the collar follows the mask, so the plates stay on the
    // glass instead of being cut by it.
    int inset = row_inset(glass.size.w, glass.size.h, TOP_Y + row * row_h,
                          row_h);
    int16_t collar_x = zone.origin.x + zone.size.w - 3 - inset;
    int16_t x = collar_x - PLATE_GAP;
    bool clipped = false;
    for (int i = 0; i < PLATE_N; i++) {
      for (int n = 0; n < counts[i]; n++) {
        draw_plate_rect(ctx, zone, i, &x, &clipped);
        if (clipped) {
          break;
        }
      }
      if (clipped) {
        break;
      }
    }
#if defined(PBL_COLOR) && LAYOUT_LARGE
    // Labels walk the same geometry, over the rects.
    x = collar_x - PLATE_GAP;
    for (int i = 0; i < PLATE_N; i++) {
      for (int n = 0; n < counts[i]; n++) {
        draw_plate_label(ctx, zone, i, &x, &clipped);
        if (clipped) {
          break;
        }
      }
      if (clipped) {
        break;
      }
    }
#endif
    if (clipped) {
      // Too many plates to render: indicate more off-screen to the left.
      graphics_context_set_stroke_color(ctx, GColorLightGray);
      graphics_draw_line(ctx, GPoint(zone.origin.x + 1, zone.origin.y + zone.size.h / 2),
                         GPoint(zone.origin.x + 5, zone.origin.y + zone.size.h / 2));
    }

    // Collar: vertical line at the right edge of the glyph zone.
    graphics_context_set_stroke_color(ctx, GColorDarkGray);
    graphics_draw_line(ctx,
                       GPoint(collar_x, zone.origin.y + 2),
                       GPoint(collar_x, zone.origin.y + zone.size.h - 3));
  }
}

// ---------------------------------------------------------------------------
// UI updates
// ---------------------------------------------------------------------------

static void update_ui(void);  // forward decl (used by the count window below)
static void rebuild_row_layers(void);  // forward decl (wizard step change)
static void send_settings_to_phone(void);  // fwd decl (wizard uses it too)

// ---------------------------------------------------------------------------
// Plate-count settings window (number entry)
// ---------------------------------------------------------------------------

static void count_configure(NumberWindow *nw, int index) {
  static char label[40];
  if (index == -1) {
    // First wizard screen: percent step size (5 or 10).
    number_window_set_label(nw, "Pct step");
    number_window_set_min(nw, PCT_STEP_FINE);
    number_window_set_max(nw, PCT_STEP_DEFAULT);
    number_window_set_step_size(nw, 5);
    number_window_set_value(nw, s_pct_step);
    return;
  }
  snprintf(label, sizeof(label), "# of %s lb plates",
           PLATE_LB_STRS[index]);
  number_window_set_label(nw, label);
  // The widget walks 0..COUNT_UNLIMITED, but only 0..COUNT_MAX_LITERAL and the
  // sentinel are real counts: the callbacks below snap the values in between
  // onto the sentinel, so UP from the largest literal count shows 99 and DOWN
  // from 99 shows it right back.
  number_window_set_min(nw, 0);
  number_window_set_max(nw, COUNT_UNLIMITED);
  number_window_set_step_size(nw, 1);
  number_window_set_value(nw, s_plate_counts[index]);
}

// Skip the widget's 11..98 on the way past the literal counts. The click
// handlers set the new value and mark the layer dirty rather than drawing, so
// the snapped value is the one that reaches the screen.
static void count_incremented(NumberWindow *nw, void *context) {
  if (s_count_index >= 0 && number_window_get_value(nw) > COUNT_MAX_LITERAL) {
    number_window_set_value(nw, COUNT_UNLIMITED);
  }
}

static void count_decremented(NumberWindow *nw, void *context) {
  if (s_count_index >= 0 && number_window_get_value(nw) > COUNT_MAX_LITERAL) {
    number_window_set_value(nw, COUNT_MAX_LITERAL);
  }
}

static void count_selected_handler(struct NumberWindow *nw, void *context) {
  if (s_count_index == -1) {
    // Step size screen: 5 or 10, directly selectable.
    int v = (int)number_window_get_value(nw);
    s_pct_step = (v == PCT_STEP_FINE) ? PCT_STEP_FINE : PCT_STEP_DEFAULT;
    persist_write_int(PERSIST_KEY_STEP, s_pct_step);
    APP_LOG(APP_LOG_LEVEL_INFO, "pct step = %d", s_pct_step);
    s_count_index = 0;
    count_configure(nw, 0);
    rebuild_row_layers();  // row count / plate size changed underneath
  } else {
    s_plate_counts[s_count_index] =
        count_clamp((int)number_window_get_value(nw));
    persist_write_data(PERSIST_KEY_SETTINGS, s_plate_counts,
                       sizeof(s_plate_counts));
    APP_LOG(APP_LOG_LEVEL_INFO, "plate[%s]=%d",
            PLATE_LB_STRS[s_count_index], s_plate_counts[s_count_index]);
    s_count_index++;
    if (s_count_index < PLATE_N) {
      count_configure(nw, s_count_index);  // next plate size, in place
    } else {
      s_count_index = -1;
      window_stack_pop(true);  // wizard finished
      send_settings_to_phone();  // keep the phone config page prefilled
    }
  }
  update_ui();
}

static void open_count_window(void) {
  s_count_index = -1;  // start on the step-size screen
  count_configure(s_count_window, s_count_index);
  window_stack_push(number_window_get_window(s_count_window), true);
}

// ---------------------------------------------------------------------------
// UI updates
// ---------------------------------------------------------------------------

static void update_ui(void) {
  static char title_buf[32];
  static char weight_bufs[MAX_ROWS][8];

  if (s_pct_step == PCT_STEP_FINE) {
    snprintf(title_buf, sizeof(title_buf), "Max %d lb (%d%%)",
             s_max_lb, s_pct_step);
  } else {
    snprintf(title_buf, sizeof(title_buf), "Max %d lb", s_max_lb);
  }
  text_layer_set_text(s_title_layer, title_buf);

  for (int row = 0; row < s_num_rows; row++) {
    int counts[PLATE_N];
    int shown_lb;
    plate_math_calc_row(s_plate_counts, s_max_lb, 90 - row * s_pct_step,
                        &shown_lb, counts);
    snprintf(weight_bufs[row], sizeof(weight_bufs[row]), "%d", shown_lb);
    text_layer_set_text(s_weight_layers[row], weight_bufs[row]);
  }
  APP_LOG(APP_LOG_LEVEL_INFO, "UI max=%d step=%d", s_max_lb, s_pct_step);
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
  window_stack_pop(true);  // back to the percentage screen
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

#if defined(PBL_COLOR) && LAYOUT_LARGE
// Load the pre-rendered horizontal weight labels, compact set when in 5% mode.
static void load_labels(void) {
  static const uint32_t ids[2][PLATE_N] = {
    {RESOURCE_ID_LABEL_55, RESOURCE_ID_LABEL_45, RESOURCE_ID_LABEL_35,
     RESOURCE_ID_LABEL_25, RESOURCE_ID_LABEL_15, RESOURCE_ID_LABEL_10,
     RESOURCE_ID_LABEL_5,  RESOURCE_ID_LABEL_2P5},
    {RESOURCE_ID_LABEL_55C, RESOURCE_ID_LABEL_45C, RESOURCE_ID_LABEL_35C,
     RESOURCE_ID_LABEL_25C, RESOURCE_ID_LABEL_15C, RESOURCE_ID_LABEL_10C,
     RESOURCE_ID_LABEL_5C,  RESOURCE_ID_LABEL_2P5C},
  };
  for (int i = 0; i < PLATE_N; i++) {
    s_label_bmps[i] =
        gbitmap_create_with_resource(ids[s_num_rows > 5 ? 1 : 0][i]);
  }
}
#endif  // PBL_COLOR && LAYOUT_LARGE

// Create the glyph + per-row text layers for the current s_num_rows layout.
static void create_row_layers(Layer *root, int w, int h) {
  (void)root;
  (void)h;
  s_glyph_layer = layer_create(GRect(0, TOP_Y, w, s_row_h * s_num_rows));
  layer_set_update_proc(s_glyph_layer, glyph_update_proc);

  static char pct_bufs[MAX_ROWS][16];
  for (int row = 0; row < s_num_rows; row++) {
    GRect row_frame = GRect(0, TOP_Y + row * s_row_h, w, s_row_h);
    // On round displays the text steps inward with the glyph, so a row's
    // columns stay inside that row's slice of the mask.
    int inset = row_inset(w, h, TOP_Y + row * s_row_h, s_row_h);
#if LAYOUT_LARGE
    GRect pct_frame = GRect(8 + inset,
                            row_frame.origin.y + (s_row_h - 16) / 2,
                            PCT_W, 16);
#else
    GRect pct_frame = GRect(8 + inset, row_frame.origin.y, PCT_W,
                            s_row_h);
#endif
    GRect weight_frame = GRect(WEIGHT_X + inset, row_frame.origin.y,
                               WEIGHT_W, s_row_h);

    // Percentages are always 2 digits (90..50).
    snprintf(pct_bufs[row], sizeof(pct_bufs[row]), "%2d%%",
             90 - row * s_pct_step);

    s_pct_layers[row] = make_text_layer(
        pct_frame, fonts_get_system_font(FONT_PCT), COLOR_PCT,
        GTextAlignmentLeft);
    text_layer_set_text(s_pct_layers[row], pct_bufs[row]);

#if LAYOUT_LARGE
    s_weight_layers[row] = make_text_layer(
        weight_frame, fonts_get_system_font(FONT_WEIGHT), GColorWhite,
        GTextAlignmentLeft);
#else
    // 7 rows on a 168px display leaves ~16px rows: shrink the weight font.
    GFont weight_font = fonts_get_system_font(
        s_num_rows > 5 ? FONT_KEY_GOTHIC_14_BOLD : FONT_WEIGHT);
    s_weight_layers[row] = make_text_layer(
        weight_frame, weight_font, GColorWhite, GTextAlignmentLeft);
#endif
  }
}

static void destroy_row_layers(void) {
  for (int row = 0; row < s_num_rows; row++) {
    text_layer_destroy(s_pct_layers[row]);
    text_layer_destroy(s_weight_layers[row]);
  }
  layer_destroy(s_glyph_layer);
  s_glyph_layer = NULL;
}

// Re-add every layer to the root in paint order. Used on load and after a
// step change (the row layers are recreated; base layers persist).
static void add_all_layers(Layer *root) {
  layer_add_child(root, text_layer_get_layer(s_title_layer));
#if LAYOUT_LARGE
  layer_add_child(root, text_layer_get_layer(s_hint_layer));
#endif
  layer_add_child(root, s_glyph_layer);
  for (int row = 0; row < s_num_rows; row++) {
    layer_add_child(root, text_layer_get_layer(s_pct_layers[row]));
    layer_add_child(root, text_layer_get_layer(s_weight_layers[row]));
  }
#if !LAYOUT_LARGE
  layer_add_child(root, text_layer_get_layer(s_footer_layer));
#endif
}

// Rebuild the row layers after the step size changed (called while the
// settings wizard is stacked above the results window). Also swaps the
// label set (regular <-> compact) to match the new row count.
static void rebuild_row_layers(void) {
  s_num_rows = (s_pct_step == PCT_STEP_FINE) ? 7 : 5;
#if defined(PBL_COLOR) && LAYOUT_LARGE
  for (int i = 0; i < PLATE_N; i++) {
    if (s_label_bmps[i] != NULL) {
      gbitmap_destroy(s_label_bmps[i]);
      s_label_bmps[i] = NULL;
    }
  }
  load_labels();
#endif
  GRect b = layer_get_bounds(s_root);
  layer_remove_child_layers(s_root);
  destroy_row_layers();
  int avail = b.size.h - TOP_Y;
#if LAYOUT_LARGE
  avail -= BOTTOM_MARGIN;
#else
  avail -= EDGE_INSET ? 36 : 20;  // footer, and the mask on round displays
#endif
  s_row_h = avail / s_num_rows;
  create_row_layers(s_root, b.size.w, b.size.h);
  add_all_layers(s_root);
}

static void results_window_load(Window *window) {
  APP_LOG(APP_LOG_LEVEL_INFO, "results window load");
  Layer *root = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(root);
  int w = bounds.size.w;
  int h = bounds.size.h;
  s_root = root;

  window_set_background_color(window, GColorBlack);

  // Keep the backlight on while the results screen is foregrounded. The
  // light snaps back to automatic control on unload/app exit.
  light_enable(true);

  s_num_rows = (s_pct_step == PCT_STEP_FINE) ? 7 : 5;

#if LAYOUT_LARGE
  // Rows fill everything below a two-line header. Round displays need a
  // bottom margin so the last row stays inside the circular mask.
  s_row_h = (h - TOP_Y - (EDGE_INSET ? 28 : 4)) / s_num_rows;
  GRect header = GRect(0, 2, w, HEADER_H);
  s_title_layer = make_text_layer(header, fonts_get_system_font(FONT_HEADER),
                                  COLOR_TITLE, GTextAlignmentCenter);
  GRect hint = GRect(0, HINT_Y, w, 18);
  s_hint_layer = make_text_layer(hint, fonts_get_system_font(FONT_HINT),
                                 COLOR_PCT, GTextAlignmentCenter);
  text_layer_set_text(s_hint_layer, "per side  |  SEL: step/plates");
#else
  // Rows stop above the footer, and on round displays above the mask as well.
  s_row_h = (h - TOP_Y - (EDGE_INSET ? 36 : 20)) / s_num_rows;
  GRect header = GRect(0, HEADER_Y, w, HEADER_H);
  s_title_layer = make_text_layer(header, fonts_get_system_font(FONT_HEADER),
                                  COLOR_TITLE, GTextAlignmentCenter);
#endif

#if !LAYOUT_LARGE
  // Round displays: the footer keeps clear of the narrow bottom of the mask.
  GRect footer = GRect(0, h - (EDGE_INSET ? 34 : 22), w, 20);
  s_footer_layer = make_text_layer(footer, fonts_get_system_font(FONT_FOOTER),
                                   COLOR_FOOTER, GTextAlignmentCenter);
  text_layer_set_text(s_footer_layer, "SEL: step/plates");
#endif

#if defined(PBL_COLOR) && LAYOUT_LARGE
  load_labels();
#endif

  create_row_layers(root, w, h);
  add_all_layers(root);
  update_ui();
}

static void results_window_unload(Window *window) {
  APP_LOG(APP_LOG_LEVEL_INFO, "results window unload");
  light_enable(false);  // return backlight to automatic control
#if defined(PBL_COLOR) && LAYOUT_LARGE
  for (int i = 0; i < PLATE_N; i++) {
    if (s_label_bmps[i] != NULL) {
      gbitmap_destroy(s_label_bmps[i]);
      s_label_bmps[i] = NULL;
    }
  }
#endif
  destroy_row_layers();
  text_layer_destroy(s_title_layer);
#if LAYOUT_LARGE
  text_layer_destroy(s_hint_layer);
#else
  text_layer_destroy(s_footer_layer);
#endif
}

// ---------------------------------------------------------------------------
// Phone configuration (AppMessage)
// ---------------------------------------------------------------------------

// Plate message keys, filled once at startup (MESSAGE_KEY_* are link-time
// externs, so they cannot appear in static initializers).
static uint32_t s_plate_keys[PLATE_N];

// Push the current settings to the phone so the config page can prefill.
static void send_settings_to_phone(void) {
  DictionaryIterator *iter;
  if (app_message_outbox_begin(&iter) != APP_MSG_OK) {
    return;
  }
  dict_write_int32(iter, MESSAGE_KEY_STEP, s_pct_step);
  dict_write_int32(iter, MESSAGE_KEY_MAX_LB, s_max_lb);
  for (int i = 0; i < PLATE_N; i++) {
    dict_write_int32(iter, s_plate_keys[i], s_plate_counts[i]);
  }
  AppMessageResult r = app_message_outbox_send();
  if (r != APP_MSG_OK) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "outbox send failed: %d", r);
  }
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  (void)context;
  bool step_changed = false;
  bool state_changed = false;

  Tuple *t = dict_find(iter, MESSAGE_KEY_SYNC);
  if (t != NULL) {
    send_settings_to_phone();  // phone (re)connected / config page opening
    return;
  }

  t = dict_find(iter, MESSAGE_KEY_STEP);
  if (t != NULL && t->type == TUPLE_INT) {
    int v = (int)t->value->int32;
    if (v == PCT_STEP_FINE || v == PCT_STEP_DEFAULT) {
      if (v != s_pct_step) {
        s_pct_step = v;
        persist_write_int(PERSIST_KEY_STEP, s_pct_step);
        step_changed = true;
        state_changed = true;
      }
    }
    APP_LOG(APP_LOG_LEVEL_INFO, "msg: step=%d", v);
  }

  t = dict_find(iter, MESSAGE_KEY_MAX_LB);
  if (t != NULL && t->type == TUPLE_INT) {
    int v = (int)t->value->int32;
    if (v >= BAR_LB && v <= MAX_LB_LIMIT) {
      s_max_lb = v;
      persist_write_int(PERSIST_KEY_MAX, s_max_lb);
      state_changed = true;
    }
    APP_LOG(APP_LOG_LEVEL_INFO, "msg: max=%d", v);
  }

  for (int i = 0; i < PLATE_N; i++) {
    t = dict_find(iter, s_plate_keys[i]);
    if (t != NULL && t->type == TUPLE_INT) {
      int v = count_clamp((int)t->value->int32);
      if (v != s_plate_counts[i]) {
        s_plate_counts[i] = v;
        state_changed = true;
      }
      APP_LOG(APP_LOG_LEVEL_INFO, "msg: plate[%d]=%d", i, v);
    }
  }

  if (state_changed) {
    persist_write_data(PERSIST_KEY_SETTINGS, s_plate_counts,
                       sizeof(s_plate_counts));
    if (step_changed) {
      rebuild_row_layers();
    }
    update_ui();
  }
}

// ---------------------------------------------------------------------------
// App lifecycle
// ---------------------------------------------------------------------------

static void init(void) {
  APP_LOG(APP_LOG_LEVEL_INFO, "init begin");
  if (persist_exists(PERSIST_KEY_STEP)) {
    int step = persist_read_int(PERSIST_KEY_STEP);
    if (step == PCT_STEP_FINE || step == PCT_STEP_DEFAULT) {
      s_pct_step = step;
    }
  }
  if (persist_exists(PERSIST_KEY_MAX)) {
    int stored = persist_read_int(PERSIST_KEY_MAX);
    if (stored >= BAR_LB && stored <= MAX_LB_LIMIT) {
      s_max_lb = stored;
    }
  }
  if (persist_exists(PERSIST_KEY_SETTINGS)) {
    int bytes = persist_read_data(PERSIST_KEY_SETTINGS, s_plate_counts,
                                  sizeof(s_plate_counts));
    for (int i = 0; i < PLATE_N; i++) {
      // Stale blob from a build with a different inventory type, or a
      // corrupt/foreign count: reset that size to the default.
      if (bytes != (int)sizeof(s_plate_counts) ||
          !count_valid(s_plate_counts[i])) {
        s_plate_counts[i] = COUNT_DEFAULT;
      }
    }
  }

  s_number_window = number_window_create("Max (lb)",
      (NumberWindowCallbacks){.selected = number_selected_handler}, NULL);
  number_window_set_min(s_number_window, BAR_LB);
  number_window_set_max(s_number_window, MAX_LB_LIMIT);
  number_window_set_value(s_number_window, s_max_lb);

  s_count_window = number_window_create("Plates",
      (NumberWindowCallbacks){.incremented = count_incremented,
                              .decremented = count_decremented,
                              .selected = count_selected_handler}, NULL);

  s_results_window = window_create();
  window_set_click_config_provider(s_results_window, results_click_provider);
  window_set_window_handlers(s_results_window, (WindowHandlers){
      .load = results_window_load,
      .unload = results_window_unload,
  });
  window_stack_push(s_results_window, true);

  // Phone configuration (settings gear in the phone app): the JS side
  // requests the current settings with SYNC, pushes changes with STEP /
  // MAX_LB / PLATE_*. All MESSAGE_KEY_* values are link-time externs; copy
  // the plate ones into the runtime table once.
  s_plate_keys[0] = MESSAGE_KEY_PLATE_55;
  s_plate_keys[1] = MESSAGE_KEY_PLATE_45;
  s_plate_keys[2] = MESSAGE_KEY_PLATE_35;
  s_plate_keys[3] = MESSAGE_KEY_PLATE_25;
  s_plate_keys[4] = MESSAGE_KEY_PLATE_15;
  s_plate_keys[5] = MESSAGE_KEY_PLATE_10;
  s_plate_keys[6] = MESSAGE_KEY_PLATE_5;
  s_plate_keys[7] = MESSAGE_KEY_PLATE_2P5;
  app_message_register_inbox_received(inbox_received_handler);
  app_message_open(app_message_inbox_size_maximum(),
                   app_message_outbox_size_maximum());
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
