# Content Size

Source: <https://developer.repebble.com/guides/user-interfaces/content-size/>

The [ContentSize](https://developer.repebble.com/docs/c/User_Interface/Preferences/#preferred_content_size) API, added in SDK 4.2, allows developers to dynamically
adapt their watchface and watchapp design based upon the system `Text Size` preference (*Settings > Notifications > Text Size*).

> `[upstream]` The live page omits this caveat, but the newer revision of this
> guide in `pebble-dev/developer.rebble.io` (`source/_guides/user-interfaces/content-size.md`)
> opens with: **"The ContentSize API is currently only available in SDK 4.2-BETA."**
> Treat the API as beta.

While this allows developers to create highly accessible designs, it also serves
to provide a mechanism for creating designs which are less focused upon screen
size, and more focused upon content size.

![Watchface showing 'Content Size Small' text](https://developer.repebble.com/assets/images/guides/content-size/anim.gif)

The `Text Size` setting displays the following options on all platforms:

- Small
- Medium
- Large

Whereas, the [ContentSize](https://developer.repebble.com/docs/c/User_Interface/Preferences/#preferred_content_size) API will return different content sizes based on
the `Text Size` setting, varying by platform. The list of content sizes is:

- Small
- Medium
- Large
- Extra Large

An example of the varying content sizes:

- `Text Size`: `small` on `Basalt` is `ContentSize`: `small`
- `Text Size`: `small` on `Emery` is `ContentSize`: `medium`

The following table describes the relationship between `Text Size`, `Platform` and `ContentSize`:

| Platform                              | Text Size: Small    | Text Size: Medium   | Text Size: Large         |
| ------------------------------------- | ------------------- | ------------------- | ------------------------ |
| Aplite, Basalt, Chalk, Diorite, Flint | ContentSize: Small  | ContentSize: Medium | ContentSize: Large       |
| Emery                                 | ContentSize: Medium | ContentSize: Large  | ContentSize: Extra Large |

> *At present the Text Size setting only affects notifications and some system
> UI components, but other system UI components will be updated to support
> ContentSize in future versions.*

We highly recommend that developers begin to build and update their applications
with consideration for [ContentSize](https://developer.repebble.com/docs/c/User_Interface/Preferences/#preferred_content_size) to provide the best experience to users.

## Detecting ContentSize

In order to detect the current [ContentSize](https://developer.repebble.com/docs/c/User_Interface/Preferences/#preferred_content_size) developers can use the [`preferred_content_size()`](https://developer.repebble.com/docs/c/User_Interface/Preferences/#preferred_content_size) function.

The [ContentSize](https://developer.repebble.com/docs/c/User_Interface/Preferences/#preferred_content_size) will never change during runtime, so it's perfectly
acceptable to check this once during `init()`.

```c
static PreferredContentSize s_content_size;

void init() {
  s_content_size = preferred_content_size();
  // ...
}
```

## Adapting Layouts

There are a number of different approaches to adapting the screen layout based
upon content size. You could change font sizes, show or hide design elements, or
even present an entirely different UI.

In the following example, we will change font sizes based on the [ContentSize](https://developer.repebble.com/docs/c/User_Interface/Preferences/#preferred_content_size)

```c
static TextLayer *s_text_layer;
static PreferredContentSize s_content_size;

void init() {
  s_content_size = preferred_content_size();

  // ...
  switch (s_content_size) {
    case PreferredContentSizeMedium:
      // Use a medium font
      text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
      break;
    case PreferredContentSizeLarge:
    case PreferredContentSizeExtraLarge:
      // Use a large font
      text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
      break;
    default:
      // Use a small font
      text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
      break;
  }
  // ...
}
```

## Additional Considerations

When developing an application which dynamically adjusts based on the [ContentSize](https://developer.repebble.com/docs/c/User_Interface/Preferences/#preferred_content_size) setting, try to avoid using fixed widths and heights. Calculate
coordinates and dimensions based upon the size of the root layer, [`UnobstructedArea`](https://developer.repebble.com/docs/c/User_Interface/UnobstructedArea/) and [ContentSize](https://developer.repebble.com/docs/c/User_Interface/Preferences/#preferred_content_size)
