# Drawing primitives, images and text

Source: <https://developer.repebble.com/guides/graphics-and-animations/drawing-primitives-images-and-text/>

While Layer types such as `TextLayer` and `BitmapLayer` allow easy rendering of
text and bitmaps, more precise drawing is possible through the Graphics Context
APIs. Custom drawing of primitive shapes such as lines, rectangles and circles is
also supported. Clever use of these functions can **remove the need to
pre-prepare bitmap images** for many UI elements and icons.

## Obtaining a drawing context

All custom drawing requires a `GContext` instance. These **cannot be created** —
they are only available inside a `LayerUpdateProc`. That update procedure is a
function called when a `Layer` is to be rendered; a `BitmapLayer` is simply a
`Layer` with a `LayerUpdateProc` abstracted away by the SDK.

```c
static Layer *s_canvas_layer;
```

Allocate the Layer during `Window` creation:

```c
GRect bounds = layer_get_bounds(window_get_root_layer(window));

// Create canvas layer
s_canvas_layer = layer_create(bounds);
```

Define the `LayerUpdateProc`:

```c
static void canvas_update_proc(Layer *layer, GContext *ctx) {
  // Custom drawing happens here!

}
```

Assign it and add the layer to the `Window`:

```c
// Assign the custom drawing procedure
layer_set_update_proc(s_canvas_layer, canvas_update_proc);

// Add to Window
layer_add_child(window_get_root_layer(window), s_canvas_layer);
```

From then on, every time the Layer needs redrawing (for example, if other layer
geometry changes) the `LayerUpdateProc` is called. It can also be marked for
redrawing explicitly:

```c
// Redraw this as soon as possible
layer_mark_dirty(s_canvas_layer);
```

## Drawing primitive shapes

Lines, rectangles, circles and arbitrary paths can all be drawn and filled. Set
the output colors with:

```c
// Set the line color
graphics_context_set_stroke_color(ctx, GColorRed);

// Set the fill color
graphics_context_set_fill_color(ctx, GColorBlue);
```

Stroke width and antialiasing can also be changed:

```c
// Set the stroke width (must be an odd integer value)
graphics_context_set_stroke_width(ctx, 5);

// Disable antialiasing (enabled by default where available)
graphics_context_set_antialiased(ctx, false);
```

### Lines

Start and end positions as `GPoint` values:

```c
GPoint start = GPoint(10, 10);
GPoint end = GPoint(40, 60);

// Draw a line
graphics_draw_line(ctx, start, end);
```

### Rectangles

A bounding `GRect`, plus fill parameters where applicable:

```c
GRect rect_bounds = GRect(10, 10, 40, 60);

// Draw a rectangle
graphics_draw_rect(ctx, rect_bounds);

// Fill a rectangle with rounded corners
int corner_radius = 10;
graphics_fill_rect(ctx, rect_bounds, corner_radius, GCornersAll);
```

A rounded **unfilled** rectangle:

```c
// Draw outline of a rounded rectangle
graphics_draw_round_rect(ctx, rect_bounds, corner_radius);
```

### Circles

A center `GPoint` and radius:

```c
GPoint center = GPoint(25, 25);
uint16_t radius = 50;

// Draw the outline of a circle
graphics_draw_circle(ctx, center, radius);

// Fill a circle
graphics_fill_circle(ctx, center, radius);
```

Arcs can be drawn and filled too. Here `GOvalScaleMode` determines how the shape
is adjusted to fill the rectangle, and cartesian angles are transformed to
preserve accuracy:

```c
int32_t angle_start = DEG_TO_TRIGANGLE(0);
int32_t angle_end = DEG_TO_TRIGANGLE(45);

// Draw an arc
graphics_draw_arc(ctx, rect_bounds, GOvalScaleModeFitCircle, angle_start,
                                                                    angle_end);
```

A filled circle with a sector removed, where `inset_thickness` is the inner inset
removed from the full circle:

```c
uint16_t inset_thickness = 10;

// Fill a radial section of a circle
graphics_fill_radial(ctx, rect_bounds, GOvalScaleModeFitCircle, inset_thickness,
                                                        angle_start, angle_end);
```

For more on round elements, see the 2015 Developer Retreat talk on
[developing for Pebble Time Round](https://www.youtube.com/watch?v=3a1V4n9HDvY).

## Bitmaps

Manually drawing a `GBitmap` with the Graphics Context API is similar to using a
`BitmapLayer`, which provides extra convenience.

Load the image data from resources (see
[Images](../../pebble-app-resources/references/images.md) for how to include images
in a project):

```c
static GBitmap *s_bitmap;
```

```c
// Load the image data
s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_EXAMPLE_IMAGE);
```

Inside the `LayerUpdateProc`, draw it into the desired rectangle:

> **Note:** unlike `BitmapLayer`, the image is drawn relative to the **Layer's
> origin**, and not centered.

```c
// Get the bounds of the image
GRect bitmap_bounds = gbitmap_get_bounds(s_bitmap);

// Set the compositing mode (GCompOpSet is required for transparency)
graphics_context_set_compositing_mode(ctx, GCompOpSet);

// Draw the image
graphics_draw_bitmap_in_rect(ctx, s_bitmap, bitmap_bounds);
```

Free the data when it is no longer needed (i.e. the app is exiting):

```c
// Destroy the image data
gbitmap_destroy(s_bitmap);
```

## Drawing text

Like the `TextLayer`, a `LayerUpdateProc` can draw text. The advantages include
drawing in multiple fonts with only one `Layer`, and combining text with other
drawing operations.

First, get or load the font and set the text color:

```c
// Load the font
GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
// Set the color
graphics_context_set_text_color(ctx, GColorBlack);
```

Then determine the bounds that guide position and overflow behavior. This can be
the Layer's size, or more precise bounds. It is useful for drawing multiple text
items one after another with automatic spacing.

```c
char *text = "Example test string for the Developer Website guide!";

// Determine a reduced bounding box
GRect layer_bounds = layer_get_bounds(layer);
GRect bounds = GRect(layer_bounds.origin.x, layer_bounds.origin.y,
                     layer_bounds.size.w / 2, layer_bounds.size.h);

// Calculate the size of the text to be drawn, with restricted space
GSize text_size = graphics_text_layout_get_content_size(text, font, bounds,
                              GTextOverflowModeWordWrap, GTextAlignmentCenter);
```

Finally draw the text into the bounding rectangle:

```c
// Draw the text
graphics_draw_text(ctx, text, font, bounds, GTextOverflowModeWordWrap,
                                            GTextAlignmentCenter, NULL);
```

For the fonts available and their sizes, see
[system fonts](../../pebble-app-resources/references/system-fonts.md).