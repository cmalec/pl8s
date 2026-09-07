# Plate Calc

A minimalist barbell percentage + plate calculator for Pebble, written in C.

Enter your training max for an exercise and Plate Calc shows five descending percentages (90 / 80 / 70 / 60 / 50%) with the exact weight to load and a side-view glyph of the plates needed **per side**, color-coded by size class.

![results screen](shots/s_default.png)

## Features

- **Five percentage rows** with the loadable weight and per-side plate glyph
- **Max entry**: UP/DN nudge the max in 5 lb steps (hold to repeat), SELECT opens a NumberWindow for direct entry (45-995 lb)
- **Plate inventory setting** (hold SELECT): step through each plate size (55/45/35/25/15/10/5/2.5 lb) and set how many the gym has, 0-10 per side. 10 means unlimited - the default
- **Always loadable weights**: percentages are snapped to the nearest weight the configured inventory can build exactly. With unlimited plates every 2.5 lb step is reachable; with a limited inventory the weight snaps DOWN to the heaviest buildable weight, never showing a load you can't make
- **Persistent**: max and plate counts survive app exit
- 45 lb Olympic bar assumed

## Screenshots

|                                   |                                               |
|-----------------------------------|-----------------------------------------------|
| ![max entry](shots/s_maxedit.png) | ![plate inventory wizard](shots/s_wizard.png) |
| Max entry (NumberWindow)          | Plate inventory wizard (hold SELECT)          |

## Building & running

```sh
pebble build                          # build for all targetPlatforms
pebble install --emulator emery       # install on the emery emulator
pebble install --phone <ip>           # install to a paired phone
```

## Target platforms

Built for **emery** (Pebble Time 2) first, with layout adaptations for the smaller rect displays (basalt/diorite/flint/aplite) and round displays (chalk/gabbro).

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
