# Example implementations

Source: <https://developer.repebble.com/guides/design-and-interaction/implementation/>

Resources and links to code examples that help implement the UI designs and
interaction patterns recommended in the other guides in this section.

## UI components and patterns

SDK 3.x components combined with the *Common Design Styles* keep the user
experience consistent and intuitive. Some are components available in the SDK,
others are example implementations designed for adaptation and re-use.

| Pattern | Preview | Description |
|---------|---------|-------------|
| [MenuLayer](https://developer.repebble.com/docs/c/User_Interface/Layers/MenuLayer/) | ![menu layer](https://developer.repebble.com/assets/images/guides/design-and-interaction/menulayer.png) | Show many items in a list, allow scrolling between them, and choose an option. |
| [StatusBarLayer](https://developer.repebble.com/docs/c/User_Interface/Layers/StatusBarLayer/) | ![status bar](https://developer.repebble.com/assets/images/guides/design-and-interaction/alarm-list~basalt.png) | Display the time at the top of the Window, optionally extended with additional data. |
| [Radio Button List](https://github.com/pebble-examples/ui-patterns/blob/master/src/windows/radio_button_window.c) | ![radio button list](https://developer.repebble.com/assets/images/guides/design-and-interaction/radio-button.png) | Allow the user to specify one choice out of a list. |
| [Checkbox List](https://github.com/pebble-examples/ui-patterns/blob/master/src/windows/checkbox_window.c) | ![checkbox list](https://developer.repebble.com/assets/images/guides/design-and-interaction/checkbox-list.png) | Allow the user to choose multiple different options from a list. |
| [List Message](https://github.com/pebble-examples/ui-patterns/blob/master/src/windows/list_message_window.c) | ![list message](https://developer.repebble.com/assets/images/guides/design-and-interaction/list-message.png) | Provide a hint to help the user choose from a list of options. |
| [Message Dialog](https://github.com/pebble-examples/ui-patterns/blob/master/src/windows/dialog_message_window.c) | ![message dialog](https://developer.repebble.com/assets/images/guides/design-and-interaction/dialog-message.gif) | Show an important message using a bold fullscreen alert. |
| [Choice Dialog](https://github.com/pebble-examples/ui-patterns/blob/master/src/windows/dialog_choice_window.c) | ![choice dialog](https://developer.repebble.com/assets/images/guides/design-and-interaction/dialog-choice-patterns.png) | Present the user with an important choice, using the action bar and icons to speed up decision making. |
| [PIN Entry](https://github.com/pebble-examples/ui-patterns/blob/master/src/windows/pin_window.c) | ![pin entry](https://developer.repebble.com/assets/images/guides/design-and-interaction/pin.png) | Enable the user to input integer data. |
| [Text Animation](https://github.com/pebble-examples/ui-patterns/blob/master/src/windows/text_animation_window.c) | ![text animation](https://developer.repebble.com/assets/images/guides/design-and-interaction/text-change-anim.gif) | Example animation to highlight a change in a text field. |
| [Progress Bar](https://github.com/pebble-examples/ui-patterns/blob/master/src/windows/progress_bar_window.c) | ![progress bar](https://developer.repebble.com/assets/images/guides/design-and-interaction/progress-bar.gif) | Example progress bar implementation on top of a `StatusBarLayer`. |
| [Progress Layer](https://github.com/pebble-examples/ui-patterns/blob/master/src/windows/progress_layer_window.c) | ![progress layer](https://developer.repebble.com/assets/images/guides/design-and-interaction/progresslayer.gif) | Example implementation of the system progress bar layer. |

All eleven source links and all image URLs above were verified live on
2026-09-19. The patterns come from the
[`ui-patterns`](https://github.com/pebble-examples/ui-patterns) example app —
clone that repo rather than copying individual files if you want more than one.

## Example apps

Existing apps are a good starting point for designing (or improving) a UI. Many
can be found on the appstore with links to their source code.

### Cards Example (Weather)

The weather [cards-example](https://github.com/pebble-examples/cards-example)
embodies the 'card' design pattern. Consisting of a single layout, it displays all
the crucial weather-related data in summary without the need for further layers of
navigation. Instead, the buttons are reserved for scrolling between whole sets of
data pertaining to different cities. The number of 'cards' is shown in the
top-right hand corner to let the user know that there is more data present to be
scrolled through, using the pre-existing Up and Down button action affordances the
user has already learned. This helps avoid implementing a novel navigation
pattern, which saves time for both the user and the developer.

![weather cards](https://developer.repebble.com/assets/images/guides/design-and-interaction/weather.gif)

When the user presses the appropriate buttons to scroll through sets of data, the
changing information is animated with fast, snappy, and highly visible animations
to reinforce the idea of old data moving out of the layout and being physically
replaced by new data.

## Cross-references in this skill

- What each pattern is *for*, and when to choose it: [recommended.md](recommended.md#common-design-styles)
- Round-display adjustments to these patterns: [in-the-round.md](in-the-round.md)
- Ready-made action bar and system icons: [app-assets.md](../../pebble-app-resources/references/app-assets.md)