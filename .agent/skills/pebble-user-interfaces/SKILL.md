---
name: pebble-user-interfaces
description: |
  Pebble User Interfaces reference: the Layer system and every built-in layer subclass, app configuration pages (Clay, messageKeys, the webview round trip), App Exit Reason, the three AppGlance APIs (C, REST and PebbleKit JS), Content Size for user text-size preferences, Round App UI in code, and the Unobstructed Area API for the timeline overlay. Use when building or debugging a Pebble UI, choosing or wiring a layer type, adding a settings/configuration page, making an app exit to a particular place, updating an app glance from C, a backend or JavaScript, adapting a layout to the user's text size, implementing a circular layout, or handling the timeline Quick View overlaying a watchface.
---

# Pebble user interfaces

Offline copy of the `developer.repebble.com` *User Interfaces* guides — nine
pages. Anything added from the SDK, from the newer upstream revision, or from
this repo is marked `[SDK]`, `[upstream]` or `[repo]`.

| Reference | Page |
|-----------|------|
| [layers.md](references/layers.md)                                   | `…/user-interfaces/layers/` |
| [app-configuration.md](references/app-configuration.md)             | `…/user-interfaces/app-configuration/` |
| [round-app-ui.md](references/round-app-ui.md)                       | `…/user-interfaces/round-app-ui/` |
| [unobstructed-area.md](references/unobstructed-area.md)             | `…/user-interfaces/unobstructed-area/` |
| [app-exit-reason.md](references/app-exit-reason.md)                 | `…/user-interfaces/app-exit-reason/` |
| [appglance-c.md](references/appglance-c.md)                         | `…/user-interfaces/appglance-c/` |
| [appglance-rest.md](references/appglance-rest.md)                   | `…/user-interfaces/appglance-rest/` |
| [appglance-pebblekit-js.md](references/appglance-pebblekit-js.md)   | `…/user-interfaces/appglance-pebblekit-js/` |
| [content-size.md](references/content-size.md)                       | `…/user-interfaces/content-size/` |

Base URL: `https://developer.rebble.com/guides/user-interfaces/<name>/`.

Not mirrored: *App Configuration (Static)*, the manual setup page linked from
app-configuration — <https://developer.rebble.com/guides/user-interfaces/app-configuration-static/>.

## Routing

| Task | Read |
|------|------|
| Understand layers, or pick a built-in layer type | [layers.md](references/layers.md) |
| Add a settings page to a watchapp/watchface | [app-configuration.md](references/app-configuration.md) |
| Lay out for a round display in code | [round-app-ui.md](references/round-app-ui.md) |
| Handle the timeline Quick View covering the screen | [unobstructed-area.md](references/unobstructed-area.md) |
| Control where the app returns the user on exit | [app-exit-reason.md](references/app-exit-reason.md) |
| Update an app glance from C / a backend / JS | [appglance-c.md](references/appglance-c.md) · [appglance-rest.md](references/appglance-rest.md) · [appglance-pebblekit-js.md](references/appglance-pebblekit-js.md) |
| Respect the user's text-size preference | [content-size.md](references/content-size.md) |

## The facts worth knowing without opening a reference

**Layers** — `layers.md` is the foundation page. A layer has a hierarchy, an
update procedure, and a frame; you create it, add it to a parent, and destroy it.
Composition is by parenting, not by inheritance of behaviour.

**Unobstructed area** (SDK 4.0+)

- `layer_get_unobstructed_bounds()` is the size **excluding system obstructions**;
  `layer_get_bounds()` is the full screen.
- Use unobstructed bounds for content, plain bounds for a **fullscreen background**
  that should stay fullscreen — combine both in the same `window_load`.
- The obstruction can appear and disappear while the app is running, so you are
  responsible for handling it live via `unobstructed_area_service_subscribe()`.
- The Timeline **Quick View** is currently the only system overlay.

**App exit reason** (SDK 4.0+)

- `exit_reason_set(...)`, with `APP_EXIT_NOT_SPECIFIED` (default — returns the user
  to their previous location) and `APP_EXIT_ACTION_PERFORMED_SUCCESSFULLY`
  (returns to the default watchface; for one-click apps).
- `[SDK]` On Aplite it is a **no-op macro** (`#define exit_reason_set(...) do {}
  while(0)`), so it compiles everywhere but only does anything on Basalt and newer.
- `[SDK]` Some other pages spell this `app_exit_reason_set()` — **no such function
  exists**.

**AppGlance** — watchapps only, not watchfaces.

- C: `app_glance_reload()` → your callback → `app_glance_add_slice()`. The system
  caps slices at **8 per watchapp**, but read the `limit` argument rather than
  assuming. Slices show in the order added and persist until they expire.
- REST: `PUT /v1/user/glance` with an `X-User-Token` header and a JSON body.
  `[SDK]` The page documents `timeline-api.getpebble.com`, which is **defunct** —
  use `timeline-api.rebble.io`.
- PebbleKit JS: `Pebble.appGlanceReload()`.

**Content size** — `preferred_content_size()` returns a `PreferredContentSize`.
It **never changes during runtime**, so call it once in `init()`. `[upstream]` The
API is SDK 4.2 and beta.

**Platform conditionals** — `PBL_IF_ROUND_ELSE()` / `PBL_IF_RECT_ELSE()` and
`PBL_COLOR` for shape and colour branching; see `pebble-best-practices` for the
full define table.

## Related skills

- Drawing, animation and framebuffer: `pebble-graphics`
- Choosing a layout or navigation pattern: `pebble-design`
- `PBL_*` defines, battery, project structure: `pebble-best-practices`
- Bitmaps, fonts, PDC: `pebble-app-resources`

## `[repo]` pl8s

pl8s's configuration flow already matches
[app-configuration.md](references/app-configuration.md): a `showConfiguration`
listener, a `webviewclosed` listener, an `localStorage`-backed settings cache, and
a `pebblejs://close#<json>` return from `config.html`. The page's Clay-based flow
is the alternative to that hand-rolled one.

It uses only `TextLayer` (plus raw `Layer` drawing) — no `MenuLayer`, `ScrollLayer`,
`StatusBarLayer`, `ActionBarLayer` or `BitmapLayer`. It uses no `AppGlance`,
`AppExitReason`, `ContentSize` or UnobstructedArea API.

**The one live gap:** pl8s never calls `layer_get_unobstructed_bounds()`, so its
layout does not react when the timeline Quick View covers the bottom of the
screen. See [unobstructed-area.md](references/unobstructed-area.md).