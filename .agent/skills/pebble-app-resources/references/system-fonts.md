# System fonts

Source: <https://developer.rebble.com/guides/app-resources/system-fonts/>

Every system font identifier available in the Pebble SDK, sorted by family.
System fonts are fetched with `fonts_get_system_font(FONT_KEY_...)` and **must
not be destroyed**. See [fonts.md](fonts.md) for usage and for the number-only
limitations.

Preview images for each key live under
`https://developer.rebble.com/assets/images/guides/app-resources/fonts/<name>.png`,
where `<name>` is the lowercase key suffixed `_preview` (1-bit rendering) or
`_emery` (color rendering) — e.g. `gothic_24_preview.png`, `gothic_24_emery.png`,
`leco_26_bold_preview.png`, `roboto_49_bold_subset_emery.png`. Emoji glyphs live
under `/assets/images/guides/pebble-apps/resources/emoji-glyphs/{small,large,jumbo}/`.

## Raster Gothic

| Key                       |
|---------------------------|
| `FONT_KEY_GOTHIC_14`      |
| `FONT_KEY_GOTHIC_14_BOLD` |
| `FONT_KEY_GOTHIC_18`      |
| `FONT_KEY_GOTHIC_18_BOLD` |
| `FONT_KEY_GOTHIC_24`      |
| `FONT_KEY_GOTHIC_24_BOLD` |
| `FONT_KEY_GOTHIC_28`      |
| `FONT_KEY_GOTHIC_28_BOLD` |

## Bitham

| Key                                 |
|-------------------------------------|
| `FONT_KEY_BITHAM_30_BLACK`          |
| `FONT_KEY_BITHAM_34_MEDIUM_NUMBERS` |
| `FONT_KEY_BITHAM_42_BOLD`           |
| `FONT_KEY_BITHAM_42_LIGHT`          |
| `FONT_KEY_BITHAM_42_MEDIUM_NUMBERS` |

## Roboto / Droid Serif

| Key                              |
|----------------------------------|
| `FONT_KEY_ROBOTO_CONDENSED_21`   |
| `FONT_KEY_ROBOTO_BOLD_SUBSET_49` |
| `FONT_KEY_DROID_SERIF_28_BOLD`   |

## LECO

| Key                                   | Availability         |
|---------------------------------------|----------------------|
| `FONT_KEY_LECO_20_BOLD_NUMBERS`       |                      |
| `FONT_KEY_LECO_26_BOLD_NUMBERS_AM_PM` |                      |
| `FONT_KEY_LECO_28_LIGHT_NUMBERS`      |                      |
| `FONT_KEY_LECO_32_BOLD_NUMBERS`       |                      |
| `FONT_KEY_LECO_36_BOLD_NUMBERS`       |                      |
| `FONT_KEY_LECO_38_BOLD_NUMBERS`       |                      |
| `FONT_KEY_LECO_42_NUMBERS`            |                      |
| `FONT_KEY_LECO_60_NUMBERS_AM_PM`      | Emery and newer only |
| `FONT_KEY_LECO_60_BOLD_NUMBERS_AM_PM` | Emery and newer only |

## Obtaining system font files

For design mockups, the families are available from their foundries:

- **Raster Gothic** — Mark Simonson, <http://www.marksimonson.com/>
- **Gotham (Bitham)** — <http://www.typography.com/fonts/gotham/overview/>
- **Droid Serif** — Google Fonts, <https://www.google.com/fonts/specimen/Droid+Serif>
- **LECO 1976** — <https://www.myfonts.com/fonts/carnoky/leco-1976/>

## Using emoji fonts

A subset of the built-in system fonts supports a set of emoji characters:
**Gothic 24, Gothic 24 Bold, Gothic 18, and Gothic 18 Bold**. The full range is
not included.

```c
text_layer_set_text(s_layer, "Smiley face: \U0001F603");
```

The escape is the Unicode codepoint, so the table below is directly usable in a
C string literal. *Jumbomoji* is the 80x80 SVG PebbleOS substitutes when a
notification body is just that one emoji — the ones listed have a named file
under `emoji-glyphs/jumbo/`, the rest have none.

| Codepoint | Symbol | Jumbomoji file | Name |
|-----------|--------|----------------|------|
| `U+231A`  | ⌚ | | Watch *(new)* |
| `U+263A`  | ☺ | | White Smiling Face |
| `U+2620`  | ☠ | | Skull and Crossbones *(new)* |
| `U+26A7`  | ⚧ | | Transgender Symbol *(new)* |
| `U+2705`  | ✅ | | Check Mark Button *(new)* |
| `U+270B`  | ✋ | | Raised Hand |
| `U+270C`  | ✌ | | Victory Hand |
| `U+2728`  | ✨ | | Sparkles *(new)* |
| `U+274E`  | ❎ | | Cross Mark Button *(new)* |
| `U+2757`  | ❗ | | Red Exclamation Mark *(new)* |
| `U+2763`  | ❣ | | Heavy Heart Exclamation *(new)* |
| `U+2764`  | ❤ | `Pebble_80x80_Emoji_heart.svg` | Heavy Black Heart |
| `U+2B50`  | ⭐ | | Star *(new)* |
| `U+1F319` | 🌙 | | Crescent Moon *(new)* |
| `U+1F31F` | 🌟 | | Glowing Star *(new)* |
| `U+1F337` | 🌷 | | Tulip *(new)* |
| `U+1F338` | 🌸 | | Cherry Blossom *(new)* |
| `U+1F33A` | 🌺 | | Hibiscus *(new)* |
| `U+1F340` | 🍀 | | Four Leaf Clover *(new)* |
| `U+1F37A` | 🍺 | | Beer Mug |
| `U+1F37B` | 🍻 | | Clinking Beer Mugs |
| `U+1F389` | 🎉 | | Party Popper |
| `U+1F3B6` | 🎶 | | Musical Notes *(new)* |
| `U+1F3F3` | 🏳 | | Waving White Flag *(new)* |
| `U+1F425` | 🐥 | | Front-Facing Baby Chick |
| `U+1F440` | 👀 | | Eyes *(new)* |
| `U+1F44D` | 👍 | `Pebble_80x80_Emoji_thumbs_up.svg` | Thumbs Up |
| `U+1F44E` | 👎 | | Thumbs Down |
| `U+1F480` | 💀 | | Skull *(new)* |
| `U+1F493` | 💓 | | Beating Heart |
| `U+1F494` | 💔 | | Broken Heart |
| `U+1F495` | 💕 | | Two Hearts |
| `U+1F496` | 💖 | | Sparkling Heart |
| `U+1F497` | 💗 | | Growing Heart |
| `U+1F498` | 💘 | | Heart with Arrow |
| `U+1F499` | 💙 | `Pebble_80x80_Emoji_heart.svg` | Blue Heart |
| `U+1F49A` | 💚 | `Pebble_80x80_Emoji_heart.svg` | Green Heart |
| `U+1F49B` | 💛 | `Pebble_80x80_Emoji_heart.svg` | Yellow Heart |
| `U+1F49C` | 💜 | `Pebble_80x80_Emoji_heart.svg` | Purple Heart |
| `U+1F49D` | 💝 | | Heart with Ribbon |
| `U+1F49E` | 💞 | | Revolving Hearts |
| `U+1F49F` | 💟 | | Heart Decoration |
| `U+1F4A1` | 💡 | | Light Bulb *(new)* |
| `U+1F4A3` | 💣 | | Bomb *(new)* |
| `U+1F4A5` | 💥 | | Collision *(new)* |
| `U+1F4A9` | 💩 | | Pile of Poo |
| `U+1F4AF` | 💯 | | Hundred Points *(new)* |
| `U+1F5A4` | 🖤 | | Black Heart *(new)* |
| `U+1F600` | 😀 | `Pebble_80x80_Emoji_big_open_smile.svg` | Grinning Face |
| `U+1F601` | 😁 | `Pebble_80x80_Emoji_smiling_with_teeth.svg` | Beaming Face with Smiling Eyes |
| `U+1F602` | 😂 | `Pebble_80x80_Emoji_laughing_with_tears.svg` | Face with Tears of Joy |
| `U+1F603` | 😃 | `Pebble_80x80_Emoji_big_open_smile.svg` | Smiling Face with Open Mouth |
| `U+1F604` | 😄 | `Pebble_80x80_Emoji_big_smile.svg` | Smiling Face with Smiling Eyes |
| `U+1F605` | 😅 | | Smiling Face with Cold Sweat |
| `U+1F606` | 😆 | | Smiling Face with Closed Eyes |
| `U+1F607` | 😇 | | Smiling Face with Halo |
| `U+1F608` | 😈 | | Smiling Face with Horns |
| `U+1F609` | 😉 | `Pebble_80x80_Emoji_wink.svg` | Winking Face |
| `U+1F60A` | 😊 | `Pebble_80x80_Emoji_smiling_blush.svg` | Smiling Face with Smiling Eyes |
| `U+1F60B` | 😋 | | Face Savoring Food |
| `U+1F60C` | 😌 | | Relieved Face |
| `U+1F60D` | 😍 | `Pebble_80x80_Emoji_smiling_hearts.svg` | Smiling Face with Heart-Eyes |
| `U+1F60E` | 😎 | | Smiling Face with Sunglasses |
| `U+1F60F` | 😏 | | Smirking Face |
| `U+1F610` | 😐 | | Neutral Face |
| `U+1F611` | 😑 | | Expressionless Face |
| `U+1F612` | 😒 | | Unamused Face |
| `U+1F613` | 😓 | | Face with Cold Sweat |
| `U+1F614` | 😔 | | Pensive Face |
| `U+1F615` | 😕 | | Confused Face |
| `U+1F616` | 😖 | | Confounded Face |
| `U+1F617` | 😗 | | Kissing Face |
| `U+1F618` | 😘 | `Pebble_80x80_Emoji_kissing_with_heart.svg` | Face Blowing a Kiss |
| `U+1F619` | 😙 | | Kissing Face with Smiling Eyes |
| `U+1F61A` | 😚 | | Kissing Face with Closed Eyes |
| `U+1F61B` | 😛 | | Face with Tongue |
| `U+1F61C` | 😜 | `Pebble_80x80_Emoji_wink_tongue.svg` | Winking Face with Tongue |
| `U+1F61D` | 😝 | | Squinting Face with Tongue |
| `U+1F61E` | 😞 | `Pebble_80x80_Emoji_sad.svg` | Disappointed Face |
| `U+1F61F` | 😟 | `Pebble_80x80_Emoji_sad.svg` | Worried Face |
| `U+1F620` | 😠 | | Angry Face |
| `U+1F621` | 😡 | | Pouting Face |
| `U+1F622` | 😢 | | Crying Face |
| `U+1F623` | 😣 | | Persevering Face |
| `U+1F624` | 😤 | | Face with Steam from Nose |
| `U+1F625` | 😥 | | Sad but Relieved Face |
| `U+1F626` | 😦 | | Frowning Face with Open Mouth |
| `U+1F627` | 😧 | | Anguished Face |
| `U+1F628` | 😨 | | Fearful Face |
| `U+1F629` | 😩 | | Weary Face |
| `U+1F62A` | 😪 | | Sleepy Face |
| `U+1F62B` | 😫 | | Tired Face |
| `U+1F62C` | 😬 | | Grimacing Face |
| `U+1F62D` | 😭 | | Loudly Crying Face |
| `U+1F62E` | 😮 | | Face with Open Mouth |
| `U+1F62F` | 😯 | | Hushed Face |
| `U+1F630` | 😰 | | Anxious Face with Sweat |
| `U+1F631` | 😱 | | Face Screaming in Fear |
| `U+1F632` | 😲 | | Astonished Face |
| `U+1F633` | 😳 | | Flushed Face |
| `U+1F634` | 😴 | | Sleeping Face |
| `U+1F635` | 😵 | | Dizzy Face |
| `U+1F636` | 😶 | | Face Without Mouth |
| `U+1F637` | 😷 | | Face with Medical Mask |
| `U+1F643` | 🙃 | | Upside-Down Face *(new)* |
| `U+1F644` | 🙄 | | Face with Rolling Eyes *(new)* |
| `U+1F64F` | 🙏 | | Folded Hands |
| `U+1F917` | 🤗 | | Hugging Face *(new)* |
| `U+1F918` | 🤘 | | Sign of the Horns *(new)* |
| `U+1F91D` | 🤝 | | Handshake *(new)* |
| `U+1F923` | 🤣 | | Rolling on the Floor Laughing *(new)* |
| `U+1F924` | 🤤 | | Drooling Face *(new)* |
| `U+1F929` | 🤩 | | Star-Struck *(new)* |
| `U+1F92A` | 🤪 | | Zany Face *(new)* |
| `U+1F92C` | 🤬 | | Face with Symbols on Mouth *(new)* |
| `U+1F92E` | 🤮 | | Face Vomiting *(new)* |
| `U+1F970` | 🥰 | | Smiling Face with Hearts *(new)* |
| `U+1F97A` | 🥺 | | Pleading Face *(new)* |

### Deprecated emoji symbols

The following emoji characters are **no longer available on the Aplite
platform**. The guide shows them only as an image
(`/assets/images/guides/pebble-apps/resources/emoji-unsupported.png`), with no
codepoint list — look there before assuming an emoji renders on Aplite.