---
name: pebble-tools
description: |
  Pebble tooling and project reference: the full package.json app-metadata spec (uuid, displayName, targetPlatforms, capabilities, messageKeys, hiddenApp, publishedMedia), the pebble command-line tool (build, install, logs, emulator control, wipe, clean, package, GDB debugging), the Pebble Developer Connection, per-platform hardware capabilities and size limits, the 64-colour GColor table with hex values and Sunlight correction, and app internationalization. Use when editing package.json's pebble block, running or scripting pebble CLI commands, enabling the developer connection, checking a platform's screen/memory/app-size limits, looking up a GColor constant or its hex value, or localizing an app into other languages.
---

# Pebble tools and resources

Offline copy of the `developer.repebble.com` *Tools and Resources* guides — seven
pages. Anything added from the SDK or from this repo is marked `[SDK]` or `[repo]`.

| Reference | Page |
|-----------|------|
| [pebble-tool.md](references/pebble-tool.md)                       | `…/tools-and-resources/pebble-tool/` |
| [app-metadata.md](references/app-metadata.md)                     | `…/tools-and-resources/app-metadata/` |
| [hardware-information.md](references/hardware-information.md)     | `…/tools-and-resources/hardware-information/` |
| [color-picker.md](references/color-picker.md)                     | `…/tools-and-resources/color-picker/` |
| [developer-connection.md](references/developer-connection.md)     | `…/tools-and-resources/developer-connection/` |
| [internationalization.md](references/internationalization.md)     | `…/tools-and-resources/internationalization/` |
| [getting-started-time-round.md](references/getting-started-time-round.md) | `…/getting-started-pebble-time-round/` — **note the different path** |

Base URL: `https://developer.repebble.com/guides/tools-and-resources/<name>/`,
except the Time Round page which lives at
`https://developer.repebble.com/guides/getting-started-pebble-time-round/`.

## Routing

| Task | Read |
|------|------|
| Edit `package.json`'s `pebble` block | [app-metadata.md](references/app-metadata.md) |
| Run or script the `pebble` CLI | [pebble-tool.md](references/pebble-tool.md) |
| Install/debug over Wi-Fi from a computer | [developer-connection.md](references/developer-connection.md) |
| Check a platform's screen, memory or size limits | [hardware-information.md](references/hardware-information.md) |
| Look up a `GColor` constant, hex or Sunlight value | [color-picker.md](references/color-picker.md) |
| Localize an app | [internationalization.md](references/internationalization.md) |
| Chalk / Time Round specifics | [getting-started-time-round.md](references/getting-started-time-round.md) |

## The facts worth knowing without opening a reference

**App metadata** — `package.json`'s `pebble` block is the app manifest. Among the
properties: `uuid`, `displayName`, `shortName`, `version`, `sdkVersion`,
`targetPlatforms`, `watchapp` / `watchface`, `capabilities`, `messageKeys`,
`resources`, `enableMultiJS`.

- `hiddenApp` and `onlyShownOnCommunication` are **mutually exclusive**;
  `hiddenApp` always takes preference.
- Hiding an app from the launcher is what timeline-only apps should do — see
  `pebble-design`'s recommended-patterns page.

**The `pebble` CLI** — the operations guide is
[pebble-tool.md](references/pebble-tool.md): building, installing to emulator or
phone, streaming logs, driving the emulator, wiping, cleaning, converting an old
project, packaging and GDB debugging. `[repo]` pl8s's `package.json` scripts
already shell out to `pebble build` for `npm run test:emulator`-style flows.

**Hardware limits** — [hardware-information.md](references/hardware-information.md)
is one wide table across all seven platforms (Aplite, Basalt, Chalk, Diorite,
Flint, Emery, Gabbro): SOC, CPU, **max resource size**, **max app size**, display
shape/size/resolution/PPI/colours/manufacturer, touch, backlight, heart-rate
monitor, microphone, speaker, sensors, buttons, vibration, case and lens
material, charging port, battery life and water resistance. The two size columns
are the ones to check before shipping a resource-heavy app.

**Colours** — [color-picker.md](references/color-picker.md) has the complete
64-colour table: name, hex, and SDK constant, plus the **Sunlight** (uncorrected →
corrected) mapping used for the colour displays. `[SDK]` The picker does **not**
include `GColorClear`, which the SDK does define; the file records that gap
explicitly. Values were cross-checked against the SDK's own `gcolor_definitions.h`
name↔hex pairs and match exactly.

**Developer connection** — enabling it differs per phone OS; the page has separate
Android and iOS instructions.

**Internationalization** — locale detection, the `~<lang>` resource naming that
plugs into the platform-specific resource mechanism, and the PebbleKit JS locale
path.

## Related skills

- `PBL_*` platform defines and the compatibility matrix: `pebble-best-practices`
- Resource declaration and per-platform tagging: `pebble-app-resources`
- Layers, AppGlance, configuration pages: `pebble-user-interfaces`
- Appstore submission and asset sizes: `pebble-appstore`

## `[repo]` pl8s

- `package.json` uses ten `pebble` keys: `capabilities`, `displayName`,
  `enableMultiJS`, `messageKeys`, `resources`, `sdkVersion`, `shortName`,
  `targetPlatforms`, `uuid`, `watchapp`. It does **not** set `companyName`,
  `hiddenApp` or `publishedMedia`.
- It targets all seven platforms, so the **max app size / max resource size**
  columns in [hardware-information.md](references/hardware-information.md) are the
  binding constraint — Aplite is the smallest budget by a wide margin.
- It ships 17 PNG resources and colours them with `GColor` constants, so
  [color-picker.md](references/color-picker.md) is the lookup table for picking
  values that survive the 1-bit platforms (see `pebble-app-resources` on luminance
  mapping).
- English only — nothing in the repo uses the internationalization APIs.