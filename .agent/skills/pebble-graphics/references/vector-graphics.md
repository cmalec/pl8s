# Vector graphics

Source: <https://developer.repebble.com/guides/graphics-and-animations/vector-graphics/>

An overview of drawing vector images using Pebble Draw Command files. See the
[Vector Animations tutorial](https://developer.repebble.com/tutorials/advanced/vector-animations)
for more.

## Vector graphics on Pebble

Where a bitmap contains data for every pixel to be drawn, a vector file contains
only instructions about points contained in the image and how to draw lines
connecting them, plus fill color, stroke color, and stroke width.

Vector images on Pebble use the `Draw Commands` API, which loads and displays PDC
(Pebble Draw Command) images and sequences. An example is the weather icon used in
weather timeline pins — the benefit being that the image stretches in the familiar
manner as it moves between the timeline view and the pin detail view:

![weather icon](https://developer.repebble.com/assets/images/tutorials/advanced/weather.png)

**Benefits over bitmaps** for simple images and icons:

- **Smaller resource size** — instructions for joining points are less memory
  expensive than per-pixel bitmap data.
- **Flexible rendering** — vector images can be rendered as intended, or
  manipulated at runtime to move individual points around. This lets icons appear
  more organic and life-like than static PNGs. Scaling and distortion are also
  possible.

**Drawbacks:**

- Vector files require more specialized tools to create than bitmaps, and so are
  harder to produce.
- Complicated vector files may take more time to render than if they were simply
  drawn per-pixel as a bitmap, depending on the drawing implementation.

## Creating compatible files

The on-Pebble vector format is **PDC**, created from compatible SVG files. Read
*Converting SVG to PDC* at
<https://developer.repebble.com/guides/app-resources/converting-svg-to-pdc/>
for how to produce them.

**Pebble Draw Command files can only be used from app resources, and cannot be
created at runtime.**

The byte layout of the format is documented in
[PDC file format](../../pebble-app-resources/references/pdc-format.md).

## Drawing vector graphics

Add the PDC file to project resources with the `type` field set to `raw`:

```json
"media": [
  {
    "type": "raw",
    "name": "EXAMPLE_IMAGE",
    "file": "example_image.pdc"
  }
]
```

Declare a `GDrawCommandImage` pointer at the top of the file:

```c
static GDrawCommandImage *s_command_image;
```

Create and assign it in `init()`, before calling `window_stack_push()`:

```c
// Create the object from resource file
s_command_image = gdraw_command_image_create_with_resource(RESOURCE_ID_EXAMPLE_IMAGE);
```

Define the `LayerUpdateProc` that draws it:

```c
static void update_proc(Layer *layer, GContext *ctx) {
  // Set the origin offset from the context for drawing the image
  GPoint origin = GPoint(10, 20);

  // Draw the GDrawCommandImage to the GContext
  gdraw_command_image_draw(ctx, s_command_image, origin);
}
```

Create a `Layer` to display the image:

```c
static Layer *s_canvas_layer;
```

Assign the `LayerUpdateProc` and add it to the desired `Window` during
`window_load()`:

```c
// Create the canvas Layer
s_canvas_layer = layer_create(GRect(30, 30, bounds.size.w, bounds.size.h));

// Set the LayerUpdateProc
layer_set_update_proc(s_canvas_layer, update_proc);

// Add to parent Window
layer_add_child(window_layer, s_canvas_layer);
```

Free the sub-components in `main_window_unload()`:

```c
// Destroy the canvas Layer
layer_destroy(s_canvas_layer);

// Destroy the PDC image
gdraw_command_image_destroy(s_command_image);
```

The image is loaded and rendered in the `LayerUpdateProc`. Optionally change the
`Window` background color after `window_create()` for contrast:

```c
window_set_background_color(s_main_window, GColorBlueMoon);
```

![rendered vector image](https://developer.repebble.com/assets/images/tutorials/advanced/weather-image.png)

## Related SDK docs and examples

- [Draw Commands](https://developer.repebble.com/docs/c/Graphics/Draw_Commands/)
- [PDC Image example](https://github.com/pebble-examples/pdc-image)
- [PDC Sequence example](https://github.com/pebble-examples/pdc-sequence)