---
name: pebble-best-practices
description: |
  Pebble app best practices: writing one app that works on every platform (PBL_COLOR, PBL_ROUND, PBL_MICROPHONE and the full define/macro table, PBL_API_EXISTS, WatchInfo model and colour, PebbleKit JS feature detection), avoiding hardcoded layout values with unobstructed window bounds, and the battery rules (tick units, sensor batching, Bluetooth sniff interval, backlight, vibration), plus how to break a growing app into modules and per-window files. Use when making an app work across all Pebble models, deciding between PBL_PLATFORM and feature defines, laying out against screen size, porting code that assumed 144x168, reducing power drain, or splitting up a large main.c.
---

# Pebble best practices

Offline copy of the `developer.repebble.com` *Best Practices* guides — three
pages. Anything added from the SDK or from this repo is marked `[repo]` or
`[SDK]`.

| Reference | Page |
|-----------|------|
| [building-for-every-pebble.md](references/building-for-every-pebble.md) | `…/best-practices/building-for-every-pebble/` |
| [conserving-battery-life.md](references/conserving-battery-life.md)   | `…/best-practices/conserving-battery-life/` |
| [modular-app-architecture.md](references/modular-app-architecture.md) | `…/best-practices/modular-app-architecture/` |

Base URL: `https://developer.repebble.com/guides/best-practices/<name>/`.

## Routing

| Task | Read |
|------|------|
| Make one app work on all platforms | [building-for-every-pebble.md](references/building-for-every-pebble.md) |
| Choose a conditional-compilation define or macro | [building-for-every-pebble.md#available-defines-and-macros](references/building-for-every-pebble.md#available-defines-and-macros) |
| Fix layout that breaks on other screen sizes | [building-for-every-pebble.md#avoid-hardcoded-layout-values](references/building-for-every-pebble.md#avoid-hardcoded-layout-values) |
| Cut power consumption | [conserving-battery-life.md](references/conserving-battery-life.md) |
| Split up a large `main.c` | [modular-app-architecture.md](references/modular-app-architecture.md) |

## The rules worth knowing without opening a reference

**Compatibility**

- **Prefer feature defines over `PBL_PLATFORM_*` defines.** Features are the
  stable axis; platforms come and go. The guide says this explicitly.
- The most future-proof check is **API detection**:
  `#if PBL_API_EXISTS(some_api_function)`.
- Use the **defines** to select a whole block of code (`PBL_COLOR`, `PBL_BW`,
  `PBL_ROUND`, `PBL_RECT`, `PBL_MICROPHONE`, `PBL_HEALTH`, ...), and the matching
  **macros** for a single value inside an expression —
  `PBL_IF_COLOR_ELSE(GColorJaegerGreen, GColorBlack)`.
- **Never hardcode `GRect(0, 0, 144, 168)`.** It fills Aplite/Basalt/Diorite/Flint
  but not Chalk or Emery. Use
  `layer_get_unobstructed_bounds(window_get_root_layer(window))`.
- Screen-size conditionals (`PBL_DISPLAY_HEIGHT == 228`) beat platform detection,
  because several platforms share a resolution — but **dynamic bounds beat both**.
- PebbleKit JS APIs added in SDK 3.x (`Pebble.getActiveWatchInfo()`,
  `Pebble.timelineSubscribe()`) **crash** on older mobile app versions. Guard with
  `if (Pebble.getActiveWatchInfo) { ... }`.

**Battery — the single principle is "let the watch sleep"**

- Anything that keeps the watch awake costs real battery.
- Tick at `MINUTE_UNIT`, not `SECOND_UNIT`, unless you display seconds.
  `HOUR_UNIT` for minimal watchfaces. Critical on **Chalk**, where capacity is
  reduced and per-second animation can drop life to a day or less.
- **Batch sensor samples** (`accel_service_set_sampling_rate` +
  `accel_data_service_subscribe(num_samples, ...)`) and filter compass headings.
- Leave Bluetooth in `SNIFF_INTERVAL_NORMAL`; if you drop to reduced sniff for a
  bulk transfer, restore it with `app_comm_set_sniff_interval(...)` immediately
  after.
- The backlight is a big consumer. Minimize button presses (an `ActionBarLayer`
  beats a long scrolling `MenuLayer`), and if you call `light_enable(true)`,
  **always** `light_enable(false)` as soon as possible.
- Prefer animation triggered by user intent (wrist flick, tap) over animation on a
  timer.
- Cache over-the-wire data in `Storage` rather than re-fetching.

**Structure**

- A single `.c` is fine until it reaches **several hundred lines** with many
  sub-components sharing globals — then modularize.
- Layout: `src/modules/<name>.{h,c}` for logic, `src/windows/<name>.{h,c}` for
  windows, and a thin `src/main.c` that only calls `*_init()` / `*_push()` and
  deinits.
- Keep module state `static` in the `.c` and expose only getters/setters —
  callers then can't depend on the implementation.
- Give each window module a single `*_push()` entry point.

## `[repo]` pl8s

pl8s already follows most of this:

- Layout is derived from the display, not hardcoded — `main.c` keys off
  `PBL_DISPLAY_HEIGHT` for its `LAYOUT_LARGE` path, with a comment saying so
  explicitly. It uses `PBL_ROUND` and `PBL_COLOR` conditionals throughout and
  defines no `144`/`168` magic screen sizes.
- Backlight handling is correct: `light_enable(true)` on the results window load,
  `light_enable(false)` on unload, which is exactly what the battery guide asks
  for.
- No `tick_timer_service_subscribe`, accelerometer, compass, `Vibes`,
  `Animation` or `AppTimer`, so it has no obvious time-awake drain. It uses
  `persist_*` for storage and opens `AppMessage` once.

Two gaps worth knowing about:

1. **It uses `layer_get_bounds()`, never `layer_get_unobstructed_bounds()`.** The
   timeline peek can push over the bottom of the display; the plain bounds value
   does not account for it. This is the one concrete compatibility item from these
   guides that applies to pl8s today.
2. **`src/c/main.c` is 961 lines**, which is past the guide's "several hundred"
   threshold. It already has a module-shaped precedent in `plate_math.{c,h}` —
   following the guide's layout would mean `src/modules/` and `src/windows/` with
   a thin `main.c`.