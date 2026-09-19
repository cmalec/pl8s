# Raw data files

Source: <https://developer.repebble.com/guides/app-resources/raw-data-files/>

For data that is not a font or an image. When a file is included as a `raw`
resource it is **not modified in any way** from the original when the app is
built.

SDK APIs that consume raw resources include `GDrawCommand` (PDC files, see
[pdc-format.md](pdc-format.md)) and `GBitmapSequence` (APNG, see
[animated-images.md](animated-images.md)). Other uses: localized string
dictionaries, CSV data files, and so on.

## Adding a raw file

```json
"resources": {
  "media": [
    {
      "type": "raw",
      "name": "EXAMPLE_DATA_FILE",
      "file": "data.bin"
    }
  ]
}
```

## Reading bytes and byte ranges

```c
// Get resource handle
ResHandle handle = resource_get_handle(RESOURCE_ID_DATA);
```

```c
// Get size of the resource in bytes
size_t res_size = resource_size(handle);
```

Create a buffer the exact size of the resource:

```c
// Create a buffer the exact size of the raw resource
uint8_t *s_buffer = (uint8_t*)malloc(res_size);
```

Copy the whole resource in:

```c
// Copy all bytes to a buffer
resource_load(handle, s_buffer, res_size);
```

Or read a specific range from a given offset:

```c
// Read the second set of 8 bytes
resource_load_byte_range(handle, 8, s_buffer, 8);
```

Because the file is stored verbatim, a raw resource is the escape hatch for any
binary format the SDK does not process itself — including PDC and APNG files,
which the SDK expects you to hand it as raw bytes.