# Animated images (APNG)

Source: <https://developer.repebble.com/guides/app-resources/animated-images/>

Animated images are played with the `GBitmapSequence` API, which takes
[APNG](https://en.wikipedia.org/wiki/APNG) files as input. APNG is similar to
`.gif`, which is not supported directly but can be converted.

The same effect is possible with multiple image resources, a `BitmapLayer` and
an `AppTimer` — but that needs a lot more code. `GBitmapSequence` handles
reading, decompression, and frame duration/count automatically.

## Converting GIF to APNG

Use [gif2apng](http://gif2apng.sourceforge.net/) with the `-z0` flag:

```
./gif2apng -z0 animation.gif
```

> **Note:** the file extension must be `.png`, **not** `.apng`.

## Adding an APNG

Include it as a **`raw`** resource (see [raw-data-files.md](raw-data-files.md)):

```json
"resources": {
  "media": [
    {
      "type":"raw",
      "name":"ANIMATION",
      "file":"images/animation.png"
    }
  ]
}
```

## Displaying APNG frames

`GBitmapSequence` uses a `GBitmap` as a container and updates its contents each
time a new frame is read from the APNG. So the first step is to create a blank
`GBitmap` to be that container.

```c
static GBitmapSequence *s_sequence;
static GBitmap *s_bitmap;
```

Load the APNG from resources and use the frame size to create the blank
container bitmap:

```c
// Create sequence
s_sequence = gbitmap_sequence_create_with_resource(RESOURCE_ID_ANIMATION);

// Create blank GBitmap using APNG frame size
GSize frame_size = gbitmap_sequence_get_bitmap_size(s_sequence);
s_bitmap = gbitmap_create_blank(frame_size, GBitmapFormat8Bit);
```

Then advance each frame with an `AppTimer` until the end of the sequence.
Loading the next frame and writing it into the container `GBitmap` is handled
for you.

```c
static BitmapLayer *s_bitmap_layer;
```

Set that layer up as described in
[Displaying an image](images.md#displaying-an-image).

The callback re-arms itself and stops when
`gbitmap_sequence_update_bitmap_next_frame()` returns `false`:

```c
static void timer_handler(void *context) {
  uint32_t next_delay;

  // Advance to the next APNG frame, and get the delay for this frame
  if(gbitmap_sequence_update_bitmap_next_frame(s_sequence, s_bitmap, &next_delay)) {
    // Set the new frame into the BitmapLayer
    bitmap_layer_set_bitmap(s_bitmap_layer, s_bitmap);
    layer_mark_dirty(bitmap_layer_get_layer(s_bitmap_layer));

    // Timer for that frame's delay
    app_timer_register(next_delay, timer_handler, NULL);
  }
}
```

Kick it off with the first frame advance:

```c
uint32_t first_delay_ms = 10;

// Schedule a timer to advance the first frame
app_timer_register(first_delay_ms, timer_handler, NULL);
```

Destroy both when the resource is no longer required:

```c
gbitmap_sequence_destroy(s_sequence);
gbitmap_destroy(s_bitmap);
```

Note the container is `GBitmapFormat8Bit`, so an animated image costs a full
8-bit frame buffer for as long as the sequence is alive — relevant on Aplite,
which has the smallest memory budget.