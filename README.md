# Plate Calc

A minimalist barbell percentage + plate calculator for Pebble, written in C.

Enter your training max for an exercise and Plate Calc shows descending percentages with the exact weight to load and a glyph of the plates needed **per side** (default 90..50 in 10% steps, or 90..60 in 5% steps — selectable in settings). Large color displays (Pebble Time 2 / agate) draw each plate as a big disc with its weight printed vertically on the face and a gym-standard color per size (55 red, 45 blue, 35 yellow, 25 green, 15 pink, 10 white, 5 cyan, 2.5 grey); compact displays keep the thin side-view bars.

![results screen](shots/s_default.png)

## Features

- **Five percentage rows** with the loadable weight and per-side plate glyphs (rotated weight labels on large color displays). Default 10% step shows 90..50; switch to **5% steps** in settings to see 90, 85, 80, 75, 70, 65, 60 (7 rows)
- **Max entry**: UP/DN nudge the max in 5 lb steps (hold to repeat), SELECT opens a NumberWindow for direct entry (45-995 lb)
- **Settings wizard** (hold SELECT): first set the percent step (5% or 10%), then step through each plate size (55/45/35/25/15/10/5/2.5 lb) and set how many the gym has, 0-10 per side. 10 means unlimited - the default
- **Always loadable weights**: percentages are snapped to the nearest weight the configured inventory can build exactly. With unlimited plates every 2.5 lb step is reachable; with a limited inventory the weight snaps DOWN to the heaviest buildable weight, never showing a load you can't make
- **Backlight stays on** while the app is in the foreground (returns to automatic control on exit); the user-facing "backlight timeout" watch setting (`Settings > Display > Backlight`) is a separate system control
- **Persistent**: max, percent step, and plate counts survive app exit
- 45 lb Olympic bar assumed

## Screenshots

|                                   |                                               |
|-----------------------------------|-----------------------------------------------|
| ![max entry](shots/s_maxedit.png) | ![settings wizard](shots/s_wizard.png)        |
| Max entry (NumberWindow)          | Settings wizard (hold SEL): percent step      |

## Building & running

```sh
pebble build                          # build for all targetPlatforms
pebble install --emulator emery       # install on the emery emulator
pebble install --phone <ip>           # install to a paired phone
```

## Target platforms

Built for **emery** (Pebble Time 2) first, with layout adaptations for the smaller rect displays (basalt/diorite/flint/aplite) and round displays (chalk/gabbro). Round large displays (gabbro) may crop the outer corners of the bottom row to the circular mask.

## Project layout

```
src/c/           C source for the watchapp
tools/           Math verification + emulator test scripts (Python)
resources/       Images, fonts, and other bundled resources
package.json     Project metadata (UUID, platforms, resources, message keys)
wscript          Build rules - usually no need to edit
```

## Verifying the math

`tools/verify_math.py` mirrors the C plate math and exhaustively checks that every displayed weight is exactly loadable across inventories and maxima:

```sh
python3 tools/verify_math.py
```

## Documentation

Full SDK docs, tutorials, and API reference: <https://developer.repebble.com>
