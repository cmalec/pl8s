# Images

Source: <https://developer.rebble.com/guides/app-resources/images/>

Images are stored in memory as a `GBitmap` while the app runs, and displayed
either in a `BitmapLayer` or with `graphics_draw_bitmap_in_rect()`.

## Creating an image

Save as PNG, ideally **palettized**, with the platform's color count:

| Platform | Number of colors    |
|----------|---------------------|
| Aplite   | 2 (black and white) |
| Basalt   | 64 colors           |
| Chalk    | 64 colors           |
| Diorite  | 2 (black and white) |
| Emery    | 64 colors           |
| Flint    | 2 (black and white) |

The guide's table omits `gabbro`; it is a color platform (see
*Platform-specific Resources*).

### Color palettes

Palette files containing the 64 supported colors, for popular graphics
packages. Base URL `https://developer.rebble.com`:

| Package                                        | File |
|------------------------------------------------|------|
| Photoshop `.act`                               | `/assets/other/pebble_colors_64.act` |
| Aseprite, raw colors for watch displays        | `/assets/other/pebble_colors_uncorrected.aseprite` |
| Aseprite, Sunlight, color-corrected for HD     | `/assets/other/pebble_colors_sunlight.aseprite` |
| Illustrator `.ai`                              | `/assets/other/pebble_colors_64.ai` |
| GIMP `.pal`                                    | `/assets/other/pebble_colors_64.pal` |
| ImageMagick `.gif`                             | `/assets/other/pebble_colors_64.gif` |

## Import the image

Place the file under the project's `resources/` directory, then add an entry to
`resources.media` in `package.json`. `file` is relative to `resources/`.

```json
"resources": {
  "media": [
    {
      "type": "bitmap",
      "name": "EXAMPLE_IMAGE",
      "file": "background.png"
    }
  ]
},
```

## Specifying an image resource

`bitmap` resources accept three optimization attributes:

| Attribute           | Description                                                                                | Values |
|---------------------|--------------------------------------------------------------------------------------------|--------|
| `memoryFormat`      | Optional. Determines the bitmap type. Reflects values in the `GBitmapFormat` `enum`.        | `Smallest`, `SmallestPalette`, `1Bit`, `8Bit`, `1BitPalette`, `2BitPalette`, `4BitPalette` |
| `storageFormat`     | Optional. Determines the file format used for storage. Using `spaceOptimization` instead is preferred. | `pbi` or `png` |
| `spaceOptimization` | Optional. Optimizes the output resource for low runtime memory or low resource space.       | `storage` or `memory` |

```json
{
  "type": "bitmap",
  "name": "IMAGE_EXAMPLE",
  "file": "images/example_image.png",
  "memoryFormat": "Smallest",
  "spaceOptimization": "memory"
}
```

**Defaults.** `memoryFormat` defaults to `Smallest` on every platform.
`spaceOptimization` defaults to `memory` on Aplite and `storage` on all other
platforms.

> If you specify a combination of attributes that is not supported, such as a
> `1Bit` unpalettized PNG, the build will fail. Palettized 1-bit PNGs are
> supported.

### Relationship to the older resource types

- `png` is equivalent to `bitmap` with no additional specifiers.
- `pbi` is equivalent to `bitmap` with `"memoryFormat": "1Bit"`.
- `pbi8` is equivalent to `bitmap` with `"memoryFormat": "8Bit"` and
  `"storageFormat": "pbi"`.

Continuing to use the `png` resource type results in a `bitmap` resource with
`"storageFormat": "png"`, which is **not optimized for memory usage on the
Aplite platform** due to less memory available in total, and is **not
encouraged**.

### Per-platform resources

Resources can be included only for specific platforms, and selected by platform
and display shape, to save resource space — e.g. Aplite needs only black and
white versions of images, which are significantly smaller. See
*Platform-specific Resources* (not mirrored in this skill) and the
`targetPlatforms` key.

## Displaying an image

```c
// Declare the GBitmap the image data is stored in while the app runs
static GBitmap *s_bitmap;

// Create it, specifying the name chosen earlier, prefixed with RESOURCE_ID_
s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_EXAMPLE_IMAGE);

static BitmapLayer *s_bitmap_layer;

// Supply the image's real width and height, and use GCompOpSet so color
// transparency is applied correctly
s_bitmap_layer = bitmap_layer_create(GRect(5, 5, 48, 48));
bitmap_layer_set_compositing_mode(s_bitmap_layer, GCompOpSet);
bitmap_layer_set_bitmap(s_bitmap_layer, s_bitmap);

layer_add_child(window_get_root_layer(window),
                                      bitmap_layer_get_layer(s_bitmap_layer));

// Destroy both when the app exits
gbitmap_destroy(s_bitmap);
bitmap_layer_destroy(s_bitmap_layer);
```

## Menu icon in the launcher

Firmware 4.0+ lets a watchapp or watchface supply a custom icon for the
launcher list. Add a **25x25 `png`** to `resources.media` and set
`"menuIcon": true`. **Larger icons are rejected by the SDK.**

```json
"resources": {
  "media": [
    {
      "type": "bitmap",
      "name": "MENU_ICON",
      "file": "images/icon.png",
      "menuIcon": true
    }
  ]
}
```

### The four display modes

Chosen automatically from the platform, the app type, and how the icon resource
is specified:

1. **Inversion** — black & white platforms, both apps and watchfaces. The mode
   all system icons use: the icon is inverted depending on the background it
   sits on. **Aplite** requires the resource to specify 1Bit memory format and
   the icon to contain only black and white; **transparency and color are not
   handled**. **Diorite and Flint** don't require 1Bit memory format for apps
   (but do for watchfaces), treat any transparency in the file as white, and
   automatically convert any color to black & white.
2. **Non-inverting transparent black & white** — optional, **Diorite & Flint
   watchfaces only**. The icon is never inverted and transparency works. A color
   icon is converted to black & white by luminance. Enabled when the memory
   format is **not** 1Bit.
3. **Non-inverting transparent greyscale** — the only mode for **apps on color
   platforms**. A color icon is converted to greyscale by luminance.
4. **Non-inverting transparent color** — the only mode for **watchfaces on color
   platforms**. (The icon doesn't have to contain color.)

### Supporting every platform with two files

Provide `icon~bw.png` and `icon~color.png` (see *Platform-specific Resources*)
and split the entry by `targetPlatforms` in `platform.json`:

```json
"resources": {
  "media": [
    {
      "file": "icon.png",
      "name": "APP_ICON",
      "type": "bitmap",
      "memoryFormat": "1Bit",
      "menuIcon": true,
      "targetPlatforms": ["aplite", "diorite", "flint"]
    },
    {
      "file": "icon.png",
      "name": "APP_ICON",
      "type": "bitmap",
      "targetPlatforms": ["basalt", "chalk", "emery", "gabbro"]
    }
  ]
}
```

Two things about that snippet: the guide writes `"file": "icon.png"` in both
entries while telling you to create `icon~bw.png` / `icon~color.png` — the
`~<variant>` suffix is what the platform-specific mechanism resolves, so the
placeholder filename is a guide inconsistency, not a second convention. The
guide's version also has a trailing comma after the last array element, which
is not valid JSON.

To get a non-inverting transparent icon on Diorite and Flint (watchfaces only),
point those two platforms at a different, non-1Bit icon — or, if the automatic
black & white conversion is good enough, at the same resource the color
platforms use.

**The app icons submitted to the appstore are independent of this resource.**
The store icons are used in the mobile app and the appstore; the `menuIcon`
resource is what always appears in the watch's launcher list.