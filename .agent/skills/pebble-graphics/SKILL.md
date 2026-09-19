---
name: pebble-graphics
description: |
  Pebble graphics and animation reference: custom drawing in a LayerUpdateProc with the Graphics Context (lines, rectangles, circles, arcs, radial fills, bitmaps, text), Animation and PropertyAnimation with easing curves and handlers, AppTimer and psleep, composite sequence and spawn animations, direct framebuffer access with GBitmapDataRowInfo and per-platform pixel formats, and Pebble Draw Command vector rendering. Use when drawing shapes or text manually instead of using a Layer type, animating a layer or a custom value, scheduling or cancelling a timer, doing effects or fast drawing through the framebuffer, getting or setting individual pixels, or rendering a .pdc vector image.
---

# Pebble graphics and animations

Offline copy of the `developer.repebble.com` *Graphics and Animations* guides —
four pages, with the code samples intact. Anything added from the SDK or from
this repo is marked `[repo]` or `[SDK]`.

This is the *how to draw* skill. For *what to draw and when* — layout, navigation,
component choice — see `pebble-design`. For the resource side of images, fonts and
PDC files, see `pebble-app-resources`.

## The guides

| Reference | Page |
|-----------|------|
| [drawing-primitives.md](references/drawing-primitives.md)     | `…/graphics-and-animations/drawing-primitives-images-and-text/` |
| [animations.md](references/animations.md)                     | `…/graphics-and-animations/animations/` |
| [framebuffer-graphics.md](references/framebuffer-graphics.md) | `…/graphics-and-animations/framebuffer-graphics/` |
| [vector-graphics.md](references/vector-graphics.md)           | `…/graphics-and-animations/vector-graphics/` |

Base URL: `https://developer.repebble.com/guides/graphics-and-animations/<name>/`.

Not mirrored, read online:
[Vector Animations tutorial](https://developer.repebble.com/tutorials/advanced/vector-animations),
[Converting SVG to PDC](https://developer.repebble.com/guides/app-resources/converting-svg-to-pdc/).

## Routing

| Task | Read |
|------|------|
| Draw shapes, text or a bitmap by hand | [drawing-primitives.md](references/drawing-primitives.md) |
| Animate a layer or a custom value, or use a timer | [animations.md](references/animations.md) |
| Touch individual pixels, or need speed | [framebuffer-graphics.md](references/framebuffer-graphics.md) |
| Render a `.pdc` vector image | [vector-graphics.md](references/vector-graphics.md) |

## The facts worth knowing without opening a reference

**Drawing context**

- You **cannot create a `GContext`** — it only exists inside a `LayerUpdateProc`.
  A `BitmapLayer` is just a `Layer` with one abstracted away.
- Call `layer_mark_dirty(layer)` to force a redraw at the next opportunity.
- Stroke width **must be an odd integer**. Antialiasing is **on by default** where
  available.
- `graphics_draw_bitmap_in_rect()` draws **relative to the Layer's origin**, not
  centered — unlike `BitmapLayer`, which centers. It needs
  `graphics_context_set_compositing_mode(ctx, GCompOpSet)` for transparency.
- Prefer primitives over pre-baked bitmaps where you can: drawing a shape can
  remove the need for a resource entirely.

**Framebuffer**

- Capture only inside a `LayerUpdateProc`, and **always release** it
  (`graphics_release_frame_buffer`) or drawing stops.
- Formats differ per platform: 1-bit on Aplite/Diorite/Flint, 8-bit on
  Basalt/Emery/Gabbro, and **`GBitmapFormat8BitCircular` on Chalk only**.
- **Gabbro is round but its framebuffer is plain rectangular 8-bit** — don't detect
  round by checking the circular format.
- Use `gbitmap_get_data_row_info()` and `min_x`/`max_x`, not `gbitmap_get_data()`,
  because Chalk rows differ in width. Call it **once per row**; per-pixel calls are
  a significant speed penalty.
- Pixel access splits on `#if defined(PBL_COLOR)` (one byte per pixel) versus
  `PBL_BW` (one bit per byte, with the `byte_get_bit` / `byte_set_bit` helpers).

**Animation and time**

- `property_animation_create_layer_frame()` animates a Layer's frame; use
  `animation_clone()` to derive a second animation rather than reusing one.
- Composite animations **own** their components: a component becomes
  **immutable** and **single-use** once added, and cannot appear twice in a list.
  Sequence = series, spawn = parallel; both compose recursively.
- An `AppTimer` callback fires **once** — re-register inside it to repeat.
- `psleep()` is a thread-blocking pause, **not** for loops that update UI, nor for
  scheduling `AppMessage` (both need the event loop). Use `Wakeup` for events that
  must outlive the app.

**Vector**

- PDC files are a `raw` resource type, produced at **build time only** — they
  cannot be created at runtime. Draw with one call:
  `gdraw_command_image_draw(ctx, image, origin)`.

## `[repo]` pl8s

pl8s already does custom drawing: `layer_set_update_proc`, `layer_mark_dirty`,
and `graphics_context_*` / `graphics_draw_*` / `graphics_fill_*` calls. That is
exactly the [drawing-primitives](references/drawing-primitives.md) surface, so the
stroke-width, antialiasing and `graphics_draw_bitmap_in_rect` origin notes apply
to it directly.

It uses **none** of: framebuffer capture, PDC/`GDrawCommand`, `Animation`, or
`AppTimer`. Those three references are therefore forward-looking — reach for
[framebuffer-graphics](references/framebuffer-graphics.md) if a per-pixel effect is
ever needed, [vector-graphics](references/vector-graphics.md) if an asset should
become a `.pdc`, and [animations](references/animations.md) if a transition is
wanted.