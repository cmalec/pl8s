---
name: pebble-app-resources
description: |
  Pebble app resource reference: bitmap/PNG resources and their memory and storage optimization, custom TTF fonts with characterRegex, the system FONT_KEY catalog, the supported emoji codepoints, the binary Pebble Draw Command (PDC / PDCI / PDCS) format, launcher menu icons, the official 64-color palettes, and the ready-made icon asset packs. Use when adding or optimizing an entry in package.json's resources.media, choosing or limiting a font, drawing an emoji on a TextLayer, reading or writing a .pdc file, setting up a watchapp menu icon, or wondering why an image is too big for a 1-bit platform.
---

# Pebble app resources

Offline copy of the five `developer.rebble.com` app-resource guides, with the
tables and code samples intact. Everything here is source-of-truth from those
guides; anything added from the SDK or from this repo is marked `[repo]` or
`[SDK]`.

Source pages (canonical, re-check if a claim looks stale):

| Reference                 | Upstream URL                                                  |
|---------------------------|---------------------------------------------------------------|
| [images.md](references/images.md)               | `https://developer.rebble.com/guides/app-resources/images/`               |
| [fonts.md](references/fonts.md)                 | `https://developer.rebble.com/guides/app-resources/fonts/`                |
| [system-fonts.md](references/system-fonts.md)   | `https://developer.rebble.com/guides/app-resources/system-fonts/`         |
| [pdc-format.md](references/pdc-format.md)       | `https://developer.rebble.com/guides/app-resources/pdc-format/`           |
| [app-assets.md](references/app-assets.md)       | `https://developer.rebble.com/guides/app-resources/app-assets/`           |

Pages in the same guide section that are **not** mirrored here: *Animated
Images*, *Converting SVG to PDC*, *Platform-specific Resources*, *Raw Data
Files*.

## Routing

| Task                                                | Read                                       |
|-----------------------------------------------------|--------------------------------------------|
| Add a bitmap, shrink an image, set a menu icon      | [images.md](references/images.md)          |
| Add a custom font, trim its glyphs, system font use | [fonts.md](references/fonts.md)            |
| Pick a `FONT_KEY_*`, or print an emoji              | [system-fonts.md](references/system-fonts.md) |
| Parse or emit a `.pdc` binary                        | [pdc-format.md](references/pdc-format.md)  |
| Grab ready-made icons or a color palette            | [app-assets.md](references/app-assets.md)  |

## The resource entry

Every resource is an object in `package.json` →
`pebble.resources.media[]`. The `name` becomes `RESOURCE_ID_<NAME>` in C.

```json
{ "type": "bitmap", "name": "EXAMPLE_IMAGE", "file": "background.png" }
```

Keys that matter and are easy to get wrong:

| Key                | Applies to | Values |
|--------------------|------------|--------|
| `type`             | all        | `bitmap`, `font`, plus legacy `png` / `pbi` / `pbi8` |
| `memoryFormat`     | bitmap     | `Smallest` (default), `SmallestPalette`, `1Bit`, `8Bit`, `1BitPalette`, `2BitPalette`, `4BitPalette` |
| `storageFormat`    | bitmap     | `pbi`, `png` |
| `spaceOptimization`| bitmap     | `storage`, `memory` — `memory` is the Aplite default, `storage` everywhere else |
| `characterRegex`   | font       | Python regex of the glyph set to bake in |
| `compatibility`    | font       | `"2.7"` reverts pre-SDK-2.8 rendering |
| `menuIcon`         | bitmap     | `true` marks the 25x25 launcher icon |
| `targetPlatforms`  | all        | restrict an entry to some platforms |

`file` is relative to the project's `resources/` directory.

- A `bitmap` name **need not** encode a size; a `font` name **must end in the
  point size** (`EXAMPLE_FONT_20`), and the guide caps the recommended size at
  48.
- Unsupported combinations fail the build rather than falling back — a `1Bit`
  *unpalettized* PNG is the example the guide gives. Palettized 1-bit PNGs are
  fine.

## Hard truths worth knowing before you touch resources

- **The legacy types are shorthands, not alternatives.** `png` ≡ `bitmap` with
  `storageFormat: png`; `pbi` ≡ `bitmap` + `memoryFormat: 1Bit`; `pbi8` ≡
  `bitmap` + `memoryFormat: 8Bit` + `storageFormat: pbi`. The guide explicitly
  says continuing to use `png` is **not encouraged** because on Aplite it is
  unoptimized against the smallest memory budget.
- **Aplite has 2 colors; Basalt, Chalk and Emery have 64; Diorite and Flint have
  2.** Colors are not intent-preserving on 1-bit: they are mapped by luminance.
- **Several system fonts are number-only.** Roboto 49 Bold Subset and Bitham
  34/42 Medium Numbers carry digits and a colon; Bitham 18/34 Light Subset is
  not suitable for general text; the LECO number sets are number-only. Check
  [system-fonts.md](references/system-fonts.md) before rendering a string in one.
- **A custom font must be loaded *and unloaded* by you.** System fonts must not
  be destroyed.
- **Only Gothic 18 / 18 Bold / 24 / 24 Bold carry emoji, and not the full
  range.** Aplite dropped several that the other platforms keep.

## [repo] Current state of pl8s

As of 2026-09-19: all 17 entries in `package.json` → `resources.media` use the
legacy `"type": "png"`, including `IMAGE_MENU_ICON`
(`resources/images/menu_icon.png`, 25x25 RGBA, `menuIcon: true`). `pl8s` targets
`aplite`, where the guide discourages that type and where the menu icon's
inversion mode expects a 1-bit black-and-white icon. Worth converting to
`bitmap` entries with explicit `memoryFormat` / `spaceOptimization` when
resource size or Aplite memory becomes a problem.