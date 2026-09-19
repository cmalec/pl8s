---
name: pebble-design
description: |
  Pebble design and interaction guidelines: the UI design and interaction rules (font sizes, colour meaning, button conventions), when to choose each common design style (cards, MenuLayer lists, ActionBarLayer, action menus, forms, status bar, modal dialogs), haptic feedback conventions, handling lost phone/internet connections, round-display design for Chalk and Gabbro (two-pixel bezel margin, centre-focused menus, pagination, ContentIndicator), one-click action apps with AppExitReason and AppGlance, and a catalogue of ready-to-adapt UI pattern implementations. Use when designing or reviewing a watchapp or watchface UI, choosing between MenuLayer, ActionBarLayer, StatusBarLayer or a dialog, laying out content for a round display, deciding on font sizes or colours, adding vibration feedback, or building a single-purpose one-click app.
---

# Pebble design and interaction

Offline copy of the `developer.repebble.com` *Design and Interaction* guides —
six pages, with their screenshots embedded inline and the code samples intact.
Anything added from the SDK or from this repo is marked `[repo]` or `[SDK]`.

These guides are conceptual: they are about layout, navigation and interaction
choices, not resource plumbing. For the resource side — bitmaps, fonts, PDC,
APNG, platform tagging — see the `pebble-app-resources` skill.

## The guides

| Reference | Page |
|-----------|------|
| [recommended.md](references/recommended.md)           | `…/design-and-interaction/recommended/` |
| [in-the-round.md](references/in-the-round.md)         | `…/design-and-interaction/in-the-round/` |
| [one-click-actions.md](references/one-click-actions.md) | `…/design-and-interaction/one-click-actions/` |
| [implementation.md](references/implementation.md)     | `…/design-and-interaction/implementation/` |
| [core-experience.md](references/core-experience.md)   | `…/design-and-interaction/core-experience/` |
| [benefits.md](references/benefits.md)                 | `…/design-and-interaction/benefits/` |

Base URL: `https://developer.repebble.com/guides/design-and-interaction/<name>/`.

## Routing

| Task | Read |
|------|------|
| Pick a layout or navigation style | [recommended.md](references/recommended.md#common-design-styles) |
| Laying out for a round display | [in-the-round.md](references/in-the-round.md) |
| Build a single-purpose launch-and-go app | [one-click-actions.md](references/one-click-actions.md) |
| Want ready-made code for a pattern | [implementation.md](references/implementation.md) |
| See how the system apps solve a UI problem | [core-experience.md](references/core-experience.md) |
| Why the guidelines exist at all | [benefits.md](references/benefits.md) |

## The rules worth knowing without opening a reference

**Layout and type**

- Keep layouts simple: only as much information as is **immediately required**.
- Font sizes: consider **28** for larger items, **minimum 18** for smaller ones.
  (System-font sizes available: see `pebble-app-resources`.)
- Use colour to convey information **without text** only where the association is
  already established (green = complete). Avoid colours in places where they have
  a **pre-conceived meaning that does not apply** — red text when there are no
  errors.
- Use animations to draw the eye to changing data, and to give the layout
  character.

**Interaction**

- Don't repurpose the Pebble buttons for actions not already associated with them
  unless the UI clearly marks it with an `ActionBarLayer`. Up/Down as
  previous/next needs no visual cue.
- Prefer **single-press** over **long-press** — long-press is hard to discover
  without an extra prompt in the UI.
- Give **direct feedback** to every input, or users will think the button did
  nothing.
- **Never override Back.** It exits the current `Window` as a platform-wide
  pattern; overriding it confuses users.
- Prefer **one `Window` showing a whole data set** paged with Up/Down (the card
  pattern) over menus-within-menus.
- **Preserve state** across launches (Persistent Storage) for apps used
  repetitively, so users don't re-navigate the same menus every time.

**Choosing a style**

| Need | Use |
|------|-----|
| Show many items, pick one | `MenuLayer` |
| Whole data set on one screen, paged | card pattern |
| A few actions tied to buttons | `ActionBarLayer` |
| More actions than the action bar holds | action menu (breadcrumb dots show level) |
| Collect several values from the user | form, one field per element, Select advances |
| Show the time / persistent app data | `StatusBarLayer` |
| Important message or a decision | full-screen modal dialog |
| Attention for an event / error | `Vibes`: **short** pulse = long task done, **long** pulse = failure needing attention |

## Round displays (Chalk, Gabbro)

- The round display has a **two-pixel border on each edge** for the bezel. Stretch
  background colour to the edges, and keep readable content out of that margin.
- **Avoid thin rings** near the edge — manufacturing variation makes them look
  off-centre. Use thick rings or inset them significantly.
- Menus are **centre-focused on the highlighted item**; rows have varying width by
  vertical position. Put extra detail in the highlighted cell, reduced previews in
  the others. `menu_layer_set_center_focused()` /
  `menu_layer_is_index_selected()` for custom cell drawing.
- Text **cannot** be scrolled per-pixel — it would reflow constantly. **Paginate**:
  scroll in discrete pages. `ScrollLayer` does this on Chalk.
- Use **`ContentIndicator`** to show there is more content (arrows top and bottom,
  like the system UI).
- Be willing to **design a different UI** for round rather than squeezing the
  rect one.

## `[repo]` pl8s

`pl8s` targets all seven platforms, including the round **chalk** and **gabbro**,
so the round-display section applies. Its UI is a `Window` plus `TextLayer`s and
raw `Layer` drawing driven by a click config provider — no `MenuLayer`,
`ActionBarLayer`, `StatusBarLayer` or `ScrollLayer` yet. If the wizard ever grows
a list of plates or a settings list, the round rules (centre-focus, pagination,
the two-pixel margin) are the ones to apply first. It is a multi-screen wizard
rather than a single-purpose one-click app, so the one-click reference matters
mainly for its **launch-reason** guidance: gate any launch-triggered action on
`APP_LAUNCH_USER` / `APP_LAUNCH_QUICK_LAUNCH`.