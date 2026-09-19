# App assets

Source: <https://developer.repebble.com/guides/app-resources/app-assets/>

Curated resources Pebble publishes for developers, to improve consistency and for convenience. Most `ActionBarLayer` implementations need at least one of the icons below.

> **Link status.** Two of the three download links on this page are dead as of 2026-09-19 — the `getpebble.com` hosts are gone. Verified replacements are given below.

| Asset | Link in the guide | Status |
|-------|-------------------|--------|
| Timeline pin icons (PDC) | `https://developer-assets.getpebble.com/assets/other/pebble-timeline-icons-pdc.zip` | **dead** — host is NXDOMAIN |
| Action bar icons | `https://s3.amazonaws.com/developer.getpebble.com/assets/other/actionbar-icons.zip` | **dead** — 301 with no usable target; the bucket redirects to `ap-southeast-1` and that endpoint returns 403 |
| Example PDC icon SVGs | `https://github.com/pebble-examples/pdc-sequence/tree/master/resources` | live |

## Live replacement: `pebble-dev/iconography`

<https://github.com/pebble-dev/iconography> holds the PebbleOS icon set with its original filenames, so it covers what the dead zips held:

- **`pebbleos/`** (586 files) — the system icons, including `action_bar_icon_check.png`, `action_bar_icon_up.png`, `action_bar_icon_down.png`, `action_bar_icon_snooze.png`, and the full `music_icon_*` family (`ellipsis`, `play`, `pause`, `skip_forward`, `skip_backward`, `volume_up`, `volume_down`). Those are **11 of the 14** filenames in the guide's table, byte-for-byte the same names.
- **`icons/`** (80 SVGs) — the app/UI set, e.g. `25px_Checkmark.svg`, `25px_Close_X.svg`, `25px_Delete_trash.svg`, `25px_Caret_up.svg`.
- Many files ship as **SVG** with `~bw` / `~color` variants (e.g. `arrow_down~bw.png`, `Pebble-PRF_launch_app~bw.svg`), which is both a better source than the rasters in the zip and a worked example of the tilde-tag convention described in [platform-specific.md](platform-specific.md).

Three of the guide's names have no exact match in that repo: `action_bar_icon_dismiss`, `action_bar_icon_edit`, `action_bar_icon_delete`. Closest equivalents there are `icons/25px_Close_X.svg` and `icons/25px_Delete_trash.svg`; for the pencil, the guide's preview image under `assets/images/guides/design-and-interaction/icons/action_bar_icon_edit.png` is the only copy.

## Example PDC icon SVG files

Many of the system PDC animations are available for use in watchfaces and watchapps as part of the `pdc-sequence` example project:

```
https://github.com/pebble-examples/pdc-sequence/tree/master/resources
```

## Example action bar icons

The guide's suggested usage for each name, so you can match a name to an intent and find the equivalent in `iconography`:

| File                            | Suggested usage |
|---------------------------------|-----------------|
| `action_bar_icon_check.png`     | Check mark for confirmation actions. |
| `action_bar_icon_dismiss.png`   | Cross mark for dismiss, cancel, or decline actions. |
| `action_bar_icon_up.png`        | Up arrow for navigating or scrolling upwards. |
| `action_bar_icon_down.png`      | Down arrow for navigating or scrolling downwards. |
| `action_bar_icon_edit.png`      | Pencil icon for edit actions. |
| `action_bar_icon_delete.png`    | Trash can icon for delete actions. |
| `action_bar_icon_snooze.png`    | Stylized 'zzz' for snooze actions. |
| `music_icon_ellipsis.png`       | Ellipsis to suggest further information or actions are available. |
| `music_icon_play.png`           | Common icon for play actions. |
| `music_icon_pause.png`          | Common icon for pause actions. |
| `music_icon_skip_forward.png`   | Common icon for skip forward actions. |
| `music_icon_skip_backward.png`  | Common icon for skip backward actions. |
| `music_icon_volume_up.png`      | Common icon for raising volume. |
| `music_icon_volume_down.png`    | Common icon for lowering volume. |

## Color palettes

See [images.md](images.md#color-palettes) for the six 64-color palette files (Photoshop, Aseprite, Illustrator, GIMP, ImageMagick) — all of those links are live.