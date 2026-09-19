# pl8s appstore submission

Everything the [Rebble developer portal](https://dev-portal.rebble.io/) asks for, in the order the portal asks for it. **Nothing here has been submitted**:
the listing is created by hand, this directory is the material for it.

The portal wants one *asset collection* per supported platform, so the description and the assets below are given per platform.

## Basic info

| Field                | Value                                                        |
|----------------------|--------------------------------------------------------------|
| Type                 | Watchapp                                                     |
| Title                | `pl8s`                                                       |
| Category             | Health & Fitness                                             |
| Source code URL      | `https://github.com/cmalec/pl8s`                             |
| Website URL          | optional - leave blank (the source repo is the project page) |
| Support email        | cmalec@users.noreply.github.com                              |
| Release              | `releases/pl8s-v1.0.1.pbw` (all seven platforms, SDK 4.33.1) |
| UUID                 | `c48ee385-2cee-4663-a08a-ac5078874a7b`                       |
| Platforms            | aplite, basalt, chalk, diorite, emery, flint, gabbro         |
| Large icon (144x144) | `appstore/icon-144.png`                                      |
| Small icon (48x48)   | `appstore/icon-48.png`                                       |

**Published** at <https://apps.rebble.io/en_US/application/6aaf0625cf733a0009498c27>
(1.0.0, live). 1.0.1 changes the launcher menu icon to black - the white glyph
was invisible in the app list - so it only needs the release uploaded and
published on the existing listing.

The assets are generated, not hand-made, so the next release just reruns them (both need `pebble build` first):

```sh
python3 tools/appstore_shots.py        # screenshots, one emulator per platform
python3 tools/make_appstore_assets.py  # icons and the 720x320 banners
```

## Description

Paste this as the description of every asset collection (it is under the 1600 character limit), then add the platform line for that collection.

```text
pl8s ("plates") is a barbell percentage and plate calculator for your wrist. Enter a training max once and it shows each working weight with the exact plates to load per side, so the math is done before you walk to the rack.

- Percentages: 90/80/70/60/50 in 10% steps, or 5% steps from 90 down to 60
- Exact loads: every row is rounded to a weight your plates can actually build on a 45 lb Olympic bar
- Plate glyph: the plates for each row are drawn side view, heaviest first
- Your gym's plates: tell it how many of each size are on the rack (0-10 per side, or 99 for unlimited) and each weight snaps down to a load those plates make
- Quick to change: nudge the max 5 lb at a time, hold SELECT for the on-watch settings wizard, or use the settings page in the phone app
- Remembers everything: max, step size and plate counts survive leaving the app

Plates are colored as they are in the gym (55 red, 45 blue, 35 yellow, 25 green, 15 pink, 10 white, 5 cyan, 2.5 grey): a disc with its weight printed on the face on the large screens, a side-view bar on the compact ones, and the same drawing in black and white on 1-bit displays.

Everything runs on the watch: no phone needed at the gym.
```

Platform line to append:

| Platform | Line                                                                                         |
|----------|----------------------------------------------------------------------------------------------|
| aplite   | `Black and white display: the same rows and plates, without color.`                          |
| basalt   | `Compact color display: each plate is a colored bar.`                                        |
| chalk    | `Round color display: the rows sit inside the circular mask.`                                |
| diorite  | `Black and white display: the same rows and plates, without color.`                          |
| emery    | `Large color display: each plate is a thick disc with its weight printed on the face.`       |
| flint    | `Black and white display: the same rows and plates, without color.`                          |
| gabbro   | `Large round color display: each plate is a thick disc with its weight printed on the face.` |

## Asset collections

Screenshots are 1:1 emulator frames at the platform's own resolution, the first one being the results screen. Banners are 720x320 and frame that platform's own screenshot.

| Platform | Screenshot size | Screenshots                                                                                | Banner               |
|----------|-----------------|--------------------------------------------------------------------------------------------|----------------------|
| aplite   | 144x168         | `screenshots/aplite-results-10pct.png`, `-results-5pct`, `-max-entry`, `-settings-plates`  | `banner/aplite.png`  |
| basalt   | 144x168         | `screenshots/basalt-results-10pct.png`, `-results-5pct`, `-max-entry`, `-settings-plates`  | `banner/basalt.png`  |
| chalk    | 180x180         | `screenshots/chalk-results-10pct.png`, `-results-5pct`, `-max-entry`, `-settings-plates`   | `banner/chalk.png`   |
| diorite  | 144x168         | `screenshots/diorite-results-10pct.png`, `-results-5pct`, `-max-entry`, `-settings-plates` | `banner/diorite.png` |
| emery    | 200x228         | `screenshots/emery-results-10pct.png`, `-results-5pct`, `-max-entry`, `-settings-plates`   | `banner/emery.png`   |
| flint    | 144x168         | `screenshots/flint-results-10pct.png`, `-results-5pct`, `-max-entry`, `-settings-plates`   | `banner/flint.png`   |
| gabbro   | 260x260         | `screenshots/gabbro-results-10pct.png`, `-results-5pct`, `-max-entry`, `-settings-plates`  | `banner/gabbro.png`  |

Four screenshots per collection: the default rows, the 5% step rows, the max entry window and the plate-count wizard (showing 99, the unlimited sentinel). That leaves room for a fifth if something else is worth showing later.

A watchapp collection also takes up to three header images (also 720x320) for the carousel at the top of the listing; the banner works as the first one if the listing should have it.

## Publishing, in portal order

1. Log in at <https://dev-portal.rebble.io/> and pick **Add a Watchapp**.
2. Basic info from the table above: title, source code URL, category (Health & Fitness), the large and small icons.
3. **Add a release** and upload `releases/pl8s-v1.0.1.pbw`, then publish the release (a listing is not complete until a release is published).
4. **Manage asset collections**: create one per platform listed above - the description with its platform line, the four screenshots, the banner.
5. Publish the listing (public). Review the preview page before publishing.
6. Copy the public appstore link and the deep link once it is live.

## Before publishing

- [x] Unique, valid UUID - only ever used by pl8s
- [x] Built with a non-beta SDK (4.33.1) for all seven platforms
- [x] Release version 1.0.0, higher than the only other build (0.0.1)
- [x] Large and small icons, descriptions, screenshots and banners above
- [x] `main` pushed: the live page at <https://cmalec.github.io/pl8s/config.html> matches `config.html` byte for byte (checked with `curl -sS ... | cmp - config.html`), so the settings page sends and accepts 99 as unlimited
- [x] Read the portal's legal agreements while logging in
- [x] Published 1.0.0: <https://apps.rebble.io/en_US/application/6aaf0625cf733a0009498c27>
- [ ] Upload and publish `releases/pl8s-v1.0.1.pbw` (black launcher icon)
- [ ] After the release is public: install from the appstore on a real watch and open the settings gear, which is the one path the emulator test cannot cover end to end (it needs the phone app)
