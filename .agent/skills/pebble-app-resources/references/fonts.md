# Fonts

Source: <https://developer.rebble.com/guides/app-resources/fonts/>

Text can be drawn with a built-in system font or a custom font added as a
project resource.

## Custom font resources

Custom fonts **must be `.ttf`** (TrueType). At build time the SDK processes the
file according to the `compatibility` and `characterRegex` fields; the latter is
a standard Python regex describing the character set of the resulting font.

## System fonts

All built-in system fonts are available through `fonts_get_system_font()`. The
full catalog is in [system-fonts.md](system-fonts.md).

### Limitations

Owing to the memory available on Pebble, the Bitham, Roboto, Droid and LECO
fonts only contain a subset of the default character set:

- **Roboto 49 Bold Subset** — contains digits and a colon.
- **Bitham 34/42 Medium Numbers** — contain digits and a colon.
- **Bitham 18/34 Light Subset** — only contains a few characters and is **not
  suitable for displaying general text**.
- **LECO Number sets** — suitable for number-only usage.

## Using a system font

The easiest choice for simple text; a system font can be obtained at any time
and **the developer is not responsible for destroying it**. Two modes:

```c
// Use a system font in a TextLayer
text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
```

```c
// Use a system font when drawing text manually
graphics_draw_text(ctx, text, fonts_get_system_font(FONT_KEY_GOTHIC_24), bounds,
                     GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
```

## Adding a custom font

Put the `.ttf` in the project's `resources` directory, then add a `font` type
item to the `media` array in `package.json`. The `name` field's contents become
available at compile time with `RESOURCE_ID_` prefixed, and **must end with the
desired font size**.

```json
"resources": {
  "media": [
    {
      "type": "font",
      "name": "EXAMPLE_FONT_20",
      "file": "example_font.ttf"
    }
  ]
}
```

> **Important:** The maximum recommended font size is 48.

## Using a custom font

Unlike a system font, a custom font **must be loaded and unloaded by the
developer**.

```c
// Declare a file-scope variable
static GFont s_font;

// Load the custom font
s_font = fonts_load_custom_font(
                          resource_get_handle(RESOURCE_ID_EXAMPLE_FONT_20));
```

Then it is used the same way as a system font:

```c
// Use a custom font in a TextLayer
text_layer_set_font(s_text_layer, s_font);
```

```c
// Use a custom font when drawing text manually
graphics_draw_text(ctx, text, s_font, bounds, GTextOverflowModeWordWrap,
                                                  GTextAlignmentCenter, NULL);
```

`[SDK]` The guide states the unload requirement but only demonstrates the load.
The matching call is `fonts_unload_custom_font(s_font)`; it is not shown in the
guide, so verify against the SDK headers before relying on it.

## Font compatibility

Font rendering was improved in SDK 2.8, which in some cases slightly changed how
custom fonts appear. To revert to the old rendering, add `"compatibility": "2.7"`
to the font's object in the `media` array:

```json
"media": [
  {
    "characterRegex": "[:0-9]",
    "type": "font",
    "name": "EXAMPLE_FONT",
    "file": "example_font.ttf"
  }
]
```

## Choosing font characters

By default the **maximum** number of supported characters is generated for a
font resource. That is usually far more than needed and bloats the app.
`characterRegex` limits the baked-in set to only what you require.

| Expression       | Result                                                             |
|------------------|--------------------------------------------------------------------|
| `[ -~]`          | ASCII characters only.                                             |
| `[0-9]`          | Numbers only.                                                      |
| `[0-9 ]`         | Numbers and spaces only.                                           |
| `[a-zA-Z]`       | Letters only.                                                      |
| `[a-zA-Z ]`      | Letters and spaces only.                                           |
| `[0-9:APM ]`     | Time strings only (e.g. "12:45 AM").                               |
| `[0-9:A-Za-z ]`  | Time and date strings (e.g. "12:43 AM Wednesday 3rd March 2015").  |
| `[0-9:A-Za-z° ]` | Time, date, and degree symbol for temperature gauges.              |
| `[0-9°CF ]`      | Numbers and degree symbol with 'C' and 'F' for temperature gauges. |

Note the shape of the idiom: a character class listing `:` and the other
punctuation *alongside* the letter and digit ranges, with a literal space inside
the class where spaces must render. A character you forget here is not rendered
at all.

Add `characterRegex` to any font object in `package.json`'s `media` array.
Reference for the regex dialect: <http://www.regular-expressions.info/tutorial.html>