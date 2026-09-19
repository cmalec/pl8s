# Framebuffer graphics

Source: <https://developer.repebble.com/guides/graphics-and-animations/framebuffer-graphics/>

In a Pebble app the framebuffer is the data region storing the contents of what is
shown on the display. The Graphics Context API lets you draw primitive shapes and
text, but at a slower speed and with a restricted set of drawing patterns. Getting
**direct access to the framebuffer** allows arbitrary transforms, special effects
and other modifications, and lets you draw at a **much greater speed** than the
standard SDK APIs.

## Accessing the framebuffer

Access can only be obtained during a `LayerUpdateProc`, when redrawing is taking
place. The framebuffer is obtained as a `GBitmap`:

```c
static void layer_update_proc(Layer *layer, GContext *ctx) {
  // Get the framebuffer
  GBitmap *fb = graphics_capture_frame_buffer(ctx);

  // Manipulate the image data...

  // Finally, release the framebuffer
  graphics_release_frame_buffer(ctx, fb);
}
```

> Once obtained, the framebuffer **must** be released back to the app so that it
> may continue drawing.

The data format varies by platform, as does the representation of a single pixel:

| Platform | Framebuffer Bitmap Format | Pixel Format |
|----------|---------------------------|--------------|
| Aplite   | `GBitmapFormat1Bit`         | One bit (black or white) |
| Basalt   | `GBitmapFormat8Bit`         | One byte (two bits per color) |
| Chalk    | `GBitmapFormat8BitCircular` | One byte (two bits per color) |
| Diorite  | `GBitmapFormat1Bit`         | One bit (black or white) |
| Flint    | `GBitmapFormat1Bit`         | One bit (black or white) |
| Emery    | `GBitmapFormat8Bit`         | One byte (two bits per color) |
| Gabbro   | `GBitmapFormat8Bit`         | One byte (two bits per color) |

Although Gabbro has a round display, its framebuffer is a regular **rectangular**
`GBitmapFormat8Bit` rather than the packed `GBitmapFormat8BitCircular` used on
Chalk. Code that special-cases round by checking for the circular format will
therefore not catch Gabbro.

## Modifying the framebuffer data

Rows or individual pixels can be manipulated. The data region is available via
`gbitmap_get_data()`, but the recommended approach is `gbitmap_get_data_row_info()`
objects, which cater for framebuffer formats (such as `GBitmapFormat8BitCircular`
on Chalk) where **not every row is of the same width**. The `GBitmapDataRowInfo`
object provides a `min_x` and `max_x` for each `y` used to build it. Using it on
platforms with a regular rectangular framebuffer is still safe — `min_x` and
`max_x` simply span the full row width.

Two nested loops iterate all rows and columns, safely avoiding rows with irregular
start and end indices. The implementation of `set_pixel_color()` is in
[Getting and setting pixels](#getting-and-setting-pixels).

> It is only necessary to call `gbitmap_get_data_row_info()` **once per row**.
> Calling it more often (such as for every pixel) incurs a significant speed
> penalty.

```c
GRect bounds = layer_get_bounds(layer);

// Iterate over all rows
for(int y = 0; y < bounds.size.h; y++) {
  // Get this row's range and data
  GBitmapDataRowInfo info = gbitmap_get_data_row_info(fb, y);

  // Iterate over all visible columns
  for(int x = info.min_x; x <= info.max_x; x++) {
    // Manipulate the pixel at x,y...
    const GColor random_color = (GColor){ .argb = rand() % 255 };

    // ...to be a random color
    set_pixel_color(info, GPoint(x, y), random_color);
  }
}
```

## Getting and setting pixels

To modify a pixel, set a new value at the appropriate position in the `data` field
of that row's `GBitmapDataRowInfo`. This modifies the underlying data and updates
the display once the framebuffer is released.

The process differs by `GBitmapFormat`. On a color platform each pixel is a single
byte; on black and white platforms it is **one bit per byte**, so reading or
modifying the correct pixel takes more logic:

```c
static GColor get_pixel_color(GBitmapDataRowInfo info, GPoint point) {
#if defined(PBL_COLOR)
  // Read the single byte color pixel
  return (GColor){ .argb = info.data[point.x] };
#elif defined(PBL_BW)
  // Read the single bit of the correct byte
  uint8_t byte = point.x / 8;
  uint8_t bit = point.x % 8;
  return byte_get_bit(&info.data[byte], bit) ? GColorWhite : GColorBlack;
#endif
}
```

Setting a value works the same way, with per-format logic:

```c
static void set_pixel_color(GBitmapDataRowInfo info, GPoint point,
                                                                GColor color) {
#if defined(PBL_COLOR)
  // Write the pixel's byte color
  memset(&info.data[point.x], color.argb, 1);
#elif defined(PBL_BW)
  // Find the correct byte, then set the appropriate bit
  uint8_t byte = point.x / 8;
  uint8_t bit = point.x % 8;
  byte_set_bit(&info.data[byte], bit, gcolor_equal(color, GColorWhite) ? 1 : 0);
#endif
}
```

The bit helpers, shown for convenience:

```c
static bool byte_get_bit(uint8_t *byte, uint8_t bit) {
  return ((*byte) >> bit) & 1;
}

static void byte_set_bit(uint8_t *byte, uint8_t bit, uint8_t value) {
  *byte ^= (-value ^ *byte) & (1 << bit);
}
```

The `#if defined(PBL_COLOR)` / `PBL_BW` split is the same platform-differentiation
mechanism used elsewhere in the SDK — the same conditional that decides whether a
`GColor` is representable on the target.

## Learn more

For an example of what direct framebuffer access can achieve, see the
[talk given at the 2014 Developer Retreat](https://www.youtube.com/watch?v=lYoHh19RNy4).

Related SDK docs:
[Graphics Context](https://developer.repebble.com/docs/c/Graphics/Graphics_Context/),
[GBitmap](https://developer.repebble.com/docs/c/Graphics/Graphics_Types/#GBitmap),
[GBitmapDataRowInfo](https://developer.repebble.com/docs/c/Graphics/Graphics_Types/#GBitmapDataRowInfo).