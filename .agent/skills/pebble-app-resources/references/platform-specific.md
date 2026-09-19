# Platform-specific resources

Source: <https://developer.rebble.com/guides/app-resources/platform-specific/>

> **Provenance:** this page body was mirrored from the older
> `developer.rebble.io` mirror on 2026-09-19 because the `rebble.com` zone was
> returning SERVFAIL from every resolver. The mirror **trails** the live site:
> its tag table has no `gabbro` column, while the live site lists `gabbro`
> among the target platforms. Re-check against the `.com` URL when it is back.

Different versions of a resource can be used on different platforms by "tagging"
resource files with the attributes that make them relevant to a given platform.

## The tags

| Aplite  | Basalt     | Chalk      | Diorite | Emery      | Flint   |
|---------|------------|------------|---------|------------|---------|
| rect    | rect       | round      | rect    | rect       | rect    |
| bw      | color      | color      | bw      | color      | bw      |
| aplite  | basalt     | chalk      | diorite | emery      | flint   |
| 144w    | 144w       | 180w       | 144w    | 200w       | 144w    |
| 168h    | 168h       | 180h       | 168h    | 228h       | 168h    |
| compass | compass    | compass    |         | compass    | compass |
|         | mic        | mic        | mic     | mic        | mic     |
|         | strap      | strap      | strap   | strap      |         |
|         | strappower | strappower |         | strappower |         |
|         | health     | health     | health  | health     | health  |

`gabbro` (Pebble Time Round 2) is absent from this mirror's table; the live site
lists it. Its geometry is 260w x 260h and it is a `round` / `color` platform.

## Tagging a file

Tags go after the filename, separated by tildes:

- `example-image~color.png` — used on color platforms only.
- `example-image~color~round.png` — used only on platforms with round, color
  displays.

**All tags must match** for the file to be used. If no file matches for a
platform, **a compilation error occurs**.

## Ambiguity rules

If the correct file for a platform is ambiguous, an error occurs at compile
time. You cannot have both `example~color.png` and `example~round.png`, because
it is unclear which to use when building for Chalk. Instead, use
`example~color~rect.png` and `example~round.png`.

**If multiple images could match, the one with the most tags wins.**

The platform tags are **not special**: `example~basalt.png` and
`example~rect.png` both match Basalt, so having both is ambiguous and causes a
compilation error.

## Prefer descriptive tags

Avoid the platform-specific tags (`aplite`, `basalt`, ...). When new platforms
are released you would have to create new files for them; with the descriptive
tags the build chooses them automatically.

## Example layout

```
my-project/
  resources/
    images/
      example-image~bw.png
      example-image~color~rect.png
      example-image~color~round.png
  src/
    main.c
  package.json
  wscript
```

The resource is declared once, **without** the tilde tags — the build resolves
the variants:

```json
"resources": {
  "media": [
    {
      "type": "bitmap",
      "name": "EXAMPLE_IMAGE",
      "file": "images/example-image.png"
    }
  ]
}
```

## Single-platform resources

To include a resource on only a **specific** platform, add a `targetPlatforms`
field to the resource's entry in the `media` array. This example is included
only in the Basalt build:

```json
"resources": {
  "media": [
    {
      "type": "bitmap",
      "name": "BACKGROUND_IMAGE",
      "file": "images/background.png",
      "targetPlatforms": [
        "basalt"
      ]
    }
  ]
}
```

`targetPlatforms` and the tilde tags are complementary: the tags pick *which
file* for a platform, `targetPlatforms` decides *whether the resource exists at
all* for a build. The [images doc](images.md#menu-icon-in-the-launcher) uses
`targetPlatforms` to split the launcher icon between 1-bit and color platforms.