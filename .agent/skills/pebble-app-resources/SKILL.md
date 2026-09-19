---
name: pebble-app-resources
description: |
  Pebble app resource reference: bitmap/PNG resources and their memory and storage optimization, custom TTF fonts with characterRegex, the system FONT_KEY catalog with preview-image links, the supported emoji codepoints with their small/large/jumbomoji renderings, the binary Pebble Draw Command (PDC / PDCI / PDCS) format, APNG animated images, raw data resources, platform-specific resource tagging, launcher menu icons, the official 64-color palettes, and the ready-made icon asset packs. Use when adding or optimizing an entry in package.json's resources.media, choosing or limiting a font, drawing an emoji on a TextLayer, reading or writing a .pdc file, animating an image, restricting a resource to some platforms, setting up a watchapp menu icon, or wondering why an image is too big for a 1-bit platform.
---

# Pebble app resources

Offline copy of the `developer.repebble.com` app-resource guides, with the tables
and code samples intact. Everything here is source-of-truth from those guides;
anything added from the SDK or from this repo is marked `[repo]` or `[SDK]`.

**Target hardware is Emery (Pebble Time 2).** Where the guides distinguish per
platform, the Emery/Basalt/Chalk rendering is the default answer here; check the
1-bit column when a change also has to survive Aplite/Diorite/Flint, which
`pl8s` still ships to.

## The guides

| Reference | Page |
|-----------|------|
| [images.md](references/images.md)                             | `…/app-resources/images/` |
| [fonts.md](references/fonts.md)                               | `…/app-resources/fonts/` |
| [system-fonts.md](references/system-fonts.md)                 | `…/app-resources/system-fonts/` |
| [pdc-format.md](references/pdc-format.md)                     | `…/app-resources/pdc-format/` |
| [app-assets.md](references/app-assets.md)                     | `…/app-resources/app-assets/` |
| [animated-images.md](references/animated-images.md)           | `…/app-resources/animated-images/` |
| [platform-specific.md](references/platform-specific.md)       | `…/app-resources/platform-specific/` |
| [raw-data-files.md](references/raw-data-files.md)             | `…/app-resources/raw-data-files/` |
| *not mirrored*                                                | `…/app-resources/converting-svg-to-pdc/` — <https://developer.repebble.com/guides/app-resources/converting-svg-to-pdc/> |

Base URL for every page above is
`https://developer.repebble.com/guides/app-resources/<name>/`.

*Converting SVG to PDC* is deliberately not mirrored: it is mostly a walkthrough
of third-party converters, so read it at the URL above.

## Routing

| Task                                                | Read |
|-----------------------------------------------------|------|
| Add a bitmap, shrink an image, set a menu icon      | [images.md](references/images.md) |
| Add a custom font, trim its glyphs, system font use | [fonts.md](references/fonts.md) |
| Pick a `FONT_KEY_*`, see a preview, print an emoji  | [system-fonts.md](references/system-fonts.md) |
| Parse or emit a `.pdc` binary                       | [pdc-format.md](references/pdc-format.md) |
| Grab ready-made icons or a color palette            | [app-assets.md](references/app-assets.md) |
| Ship an animated image                              | [animated-images.md](references/animated-images.md) |
| Vary a file per platform, or include one only there | [platform-specific.md](references/platform-specific.md) |
| Embed a data blob (PDC/APNG input, CSV, strings)    | [raw-data-files.md](references/raw-data-files.md) |
| Convert an SVG to PDC                               | the un-mirrored guide, linked above |

## Seeing what a font or emoji looks like

[system-fonts.md](references/system-fonts.md) embeds a preview image for every
one of the 25 `FONT_KEY`s — an **Emery** rendering and a **1-bit** rendering —
and a link to the **small**, **large** and **jumbomoji** artwork for each of the
118 supported emoji codepoints. The previews render inline in any markdown
viewer; the URLs are the site's own asset paths, so they need a live connection
to `developer.repebble.com`.

## The resource entry

Every resource is an object in `package.json` → `pebble.resources.media[]`. The
`name` becomes `RESOURCE_ID_<NAME>` in C.

```json
{ "type": "bitmap", "name": "EXAMPLE_IMAGE", "file": "background.png" }
```

Keys that matter and are easy to get wrong:

| Key                | Applies to | Values |
|--------------------|------------|--------|
| `type`             | all        | `bitmap`, `font`, `raw`, plus legacy `png` / `pbi` / `pbi8` |
| `memoryFormat`     | bitmap     | `Smallest` (default), `SmallestPalette`, `1Bit`, `8Bit`, `1BitPalette`, `2BitPalette`, `4BitPalette` |
| `storageFormat`    | bitmap     | `pbi`, `png` |
| `spaceOptimization`| bitmap     | `storage`, `memory` — `memory` is the Aplite default, `storage` everywhere else |
| `characterRegex`   | font       | Python regex of the glyph set to bake in |
| `compatibility`    | font       | `"2.7"` reverts pre-SDK-2.8 rendering |
| `menuIcon`         | bitmap     | `true` marks the 25x25 launcher icon |
| `targetPlatforms`  | all        | restrict an entry to some platforms |

`file` is relative to the project's `resources/` directory. A filename carrying
`~tag~tag` variants is declared **without** the tags — the build resolves them
(see [platform-specific.md](references/platform-specific.md)).

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
- **An animated image costs an 8-bit frame buffer** (`GBitmapFormat8Bit`) for as
  long as the sequence lives.

## [repo] Current state of pl8s

As of 2026-09-19: all 17 entries in `package.json` → `resources.media` use the
legacy `"type": "png"`, including `IMAGE_MENU_ICON`
(`resources/images/menu_icon.png`, 25x25 RGBA, `menuIcon: true`). `pl8s` targets
`aplite`, where the guide discourages that type and where the menu icon's
inversion mode expects a 1-bit black-and-white icon. Worth converting to
`bitmap` entries with explicit `memoryFormat` / `spaceOptimization` when
resource size or Aplite memory becomes a problem.