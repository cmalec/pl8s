# Pebble Draw Command file format

Source: <https://developer.rebble.com/guides/app-resources/pdc-format/>

Pebble Draw Commands (PDCs) are vector image files: a binary resource holding
the instructions for each stroke, fill, and so on that makes up the image.

> **Important:** All fields are in **little-endian** format.

A reference implementation, with the guide's noted usage limitations, is
[`svg2pdc.py`](https://github.com/pebble-examples/cards-example/blob/master/tools/svg2pdc.py)
— see <https://developer.rebble.com/tutorials/advanced/vector-animations#creating-compatible-files>.

## Component types

In ascending order of abstraction:

- **Draw Command** — an instruction for a single line or path to be drawn.
- **Draw Command List** — a set of Draw Commands that make up a shape.
- **Draw Command Frame** — a Draw Command List with a configurable duration, making
  up one animation frame. Many of these are used in a Draw Command Sequence.
- **Draw Command Image** — a single vector image.
- **Draw Command Sequence** — a set of Draw Command Frames that make up an
  animated sequence of vector images.

## Versions

| PDC Format Version | Implemented  |
|--------------------|--------------|
| 1                  | Firmware 3.0 |

## File format components

All offsets below are relative to the start of the component, except for the
Image and Sequence tables, whose offsets are relative to the start of the
enclosing **file** (hence the jump to `8`, past the 8-byte file header).

### Point

| Field | Offset (bytes) | Size (bytes) | Description |
|-------|----------------|--------------|-------------|
| X     | 0              | 2            | X axis coordinate. Has one of two formats depending on the Draw Command type (see below): Path/Circle type: signed integer. Precise path type: 13.3 fixed point. |
| Y     | 2              | 2            | Y axis coordinate. Has one of two formats depending on the Draw Command type (see below): Path/Circle type: signed integer. Precise path type: 13.3 fixed point. |

### View Box

| Field  | Offset (bytes) | Size (bytes) | Description                              |
|--------|----------------|--------------|------------------------------------------|
| Width  | 0              | 2            | Width of the view box (signed integer).  |
| Height | 2              | 2            | Height of the view box (signed integer). |

### Pebble Draw Command

| Field            | Offset (bytes) | Size (bytes) | Description |
|------------------|----------------|--------------|-------------|
| Type             | 0              | 1            | Draw command type. Possible values are: `0` - Invalid, `1` - Path, `2` - Circle, `3` - Precise path |
| Flags            | 1              | 1            | Bit 0: Hidden (Draw Command should not be drawn). Bits 1-7: Reserved. |
| Stroke color     | 2              | 1            | Pebble color (integer). |
| Stroke width     | 3              | 1            | Stroke width (unsigned integer). |
| Fill color       | 4              | 1            | Pebble color (integer). |
| Path open/radius | 5              | 2            | Path/Precise path type: Bit 0 indicates whether the path is drawn open (`1`) or closed (`0`). Circle type: radius of the circle. |
| Number of points | 7              | 2            | Number of points (n) in the point array. See below. |
| Point array      | 9              | n x 4        | The number of points (n) points. |

### Pebble Draw Command List

| Field              | Offset (bytes) | Size (bytes)             | Description |
|--------------------|----------------|--------------------------|-------------|
| Number of commands | 0              | 2                        | Number of Draw Commands in this Draw Command List. (`0` is invalid). |
| Draw Command array | 2              | n x size of Draw Command | List of Draw Commands in the format specified above. |

### Pebble Draw Command Frame

| Field        | Offset (bytes) | Size (bytes)              | Description |
|--------------|----------------|---------------------------|-------------|
| Duration     | 0              | 2                         | Duration of the frame in milliseconds. If `0`, the frame will not be shown at all (unless it is the last frame in a sequence). |
| Command list | 2              | Size of Draw Command List | Pebble Draw Command List in the format specified above. |

### Pebble Draw Command Image

| Field                 | Offset (bytes) | Size (bytes)              | Description |
|-----------------------|----------------|---------------------------|-------------|
| Version               | 8              | 1                         | File version. |
| Reserved              | 9              | 1                         | Reserved field. Must be `0`. |
| View box              | 10             | 4                         | Bounding box of the image. All Draw Commands are drawn relative to the top left corner of the view box. |
| Command list          | 14             | Size of Draw Command List | Pebble Draw Command List in the format specified above. |

### Pebble Draw Command Sequence

| Field                 | Offset (bytes) | Size (bytes)                   | Description |
|-----------------------|----------------|--------------------------------|-------------|
| Version               | 8              | 1                              | File version. |
| Reserved              | 9              | 1                              | Reserved field. Must be `0`. |
| View box              | 10             | 4                              | Bounding box of the sequence. All Draw Commands are drawn relative to the top left corner of the view box. |
| Play count            | 14             | 2                              | Number of times to repeat the sequence. A value of `0` will result in no playback at all, whereas a value of `0xFFFF` will repeat indefinitely. |
| Frame count           | 16             | 2                              | Number of frames in the sequence. `0` is invalid. |
| Frame list            | 18             | n x size of Draw Command Frame | Array of Draw Command Frames in the format specified above. |

## File formats

### Pebble Draw Command Image File

| Field      | Offset (bytes) | Size (bytes)                       | Description |
|------------|----------------|------------------------------------|-------------|
| Magic word | 0              | 4                                  | ASCII characters spelling "PDCI". |
| Image size | 4              | 4                                  | Size of the Pebble Draw Command Image (in bytes). |
| Image      | 8              | Size of Pebble Draw Command Image. | The Draw Command Image in the format specified above. |

### Pebble Draw Command Sequence File

| Field         | Offset (bytes) | Size (bytes)                  | Description |
|---------------|----------------|-------------------------------|-------------|
| Magic word    | 0              | 4                             | ASCII characters spelling "PDCS". |
| Sequence size | 4              | 4                             | Size of the Pebble Draw Command Sequence (in bytes). |
| Sequence      | 8              | Size of Draw Command Sequence | The Draw Command Sequence in the format specified above. |

Both sizes are 4 bytes and the payload starts at offset 8, which is why the
component tables above begin at offset `8`.

## Related SDK docs and examples

- [Draw Commands](https://developer.rebble.com/docs/c/Graphics/Draw_Commands/)
- [LayerUpdateProc](https://developer.rebble.com/docs/c/User_Interface/Layers/#LayerUpdateProc)
- [Graphics](https://developer.rebble.com/docs/c/Graphics/)
- [PDC Sequence example](https://github.com/pebble-examples/pdc-sequence)
- [Weather Cards example](https://github.com/pebble-examples/cards-example)