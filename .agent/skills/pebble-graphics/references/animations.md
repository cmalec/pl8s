# Animations

Source: <https://developer.repebble.com/guides/graphics-and-animations/animations/>

The `Animation` API smoothly animates a variety of value types from an initial
value to a new value over time, with built-in easing curves to affect how the
transition behaves.

## Using PropertyAnimations

The most common use is moving a `Layer` (or similar) around the display — to show
or hide information, or to animate the time changing in a watchface.

The simplest way to animate a `Layer` (such as a `TextLayer`) is a
`PropertyAnimation`, which animates a property of the target object. Here the
target is the frame property, a `GRect`. To animate it,
`property_animation_create_layer_frame()` is used — a convenience
`PropertyAnimation` implementation provided by the SDK.

```c
static Layer *s_layer;
```

Create the Layer during `Window` initialization:

```c
// Create the Layer
s_layer = layer_create(some_bounds);
```

Determine the start and end values of the Layer's frame — the 'from' and 'to'
locations and sizes, before and after the animation:

```c
// The start and end frames - move the Layer 40 pixels to the right
GRect start = GRect(10, 10, 20, 20);
GRect finish = GRect(50, 10, 20, 20);
```

At the appropriate time, create a `PropertyAnimation` to animate the Layer,
specifying the `start` and `finish` values:

```c
// Animate the Layer
PropertyAnimation *prop_anim = property_animation_create_layer_frame(s_layer,
                                                               &start, &finish);
```

Configure the attributes of the `Animation`, such as the delay before starting
and the total duration, both in milliseconds:

```c
// Get the Animation
Animation *anim = property_animation_get_animation(prop_anim);

// Choose parameters
const int delay_ms = 1000;
const int duration_ms = 500;

// Configure the Animation's curve, delay, and duration
animation_set_curve(anim, AnimationCurveEaseOut);
animation_set_delay(anim, delay_ms);
animation_set_duration(anim, duration_ms);
```

Finally schedule the `Animation` to play at the next possible opportunity
(usually immediately):

```c
// Play the animation
animation_schedule(anim);
```

If the app needs to know the start and end times, register `AnimationHandlers`.
The handler signatures:

```c
static void anim_started_handler(Animation *animation, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Animation started!");
}

static void anim_stopped_handler(Animation *animation, bool finished, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Animation stopped!");
}
```

Register the handlers with an optional third context parameter **before**
scheduling the Animation:

```c
// Set some handlers
animation_set_handlers(anim, (AnimationHandlers) {
  .started = anim_started_handler,
  .stopped = anim_stopped_handler
}, NULL);
```

## Other types of PropertyAnimation

Besides `property_animation_create_layer_frame()`, you can animate the origin of a
Layer's bounds with `property_animation_create_bounds_origin()`. More types of
data can be animated with custom implementations plus one of these provided update
implementations and the associated getters and setters:

- `property_animation_update_int16` — an `int16`.
- `property_animation_update_uint32` — a `uint32`.
- `property_animation_update_gpoint` — a `GPoint`.
- `property_animation_update_grect` — a `GRect`.
- `property_animation_update_gcolor8` — a `GColor8`.

## Custom animation implementations

Beyond the SDK's convenience functions, apps can implement their own `Animation`
using custom callbacks for each stage of playback. A `PropertyAnimation` is an
example of such an implementation.

The callbacks are the `.setup`, `.update`, and `.teardown` members of an
`AnimationImplementation` object. It is in `.update` that `progress` can be used to
modify the custom target of the animation — for example, some percentage of
completion:

```c
static void implementation_setup(Animation *animation) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Animation started!");
}

static void implementation_update(Animation *animation,
                                  const AnimationProgress progress) {
  // Animate some completion variable
  s_animation_percent = ((int)progress * 100) / ANIMATION_NORMALIZED_MAX;

  APP_LOG(APP_LOG_LEVEL_INFO, "Animation progress: %d%%", s_animation_percent);
}

static void implementation_teardown(Animation *animation) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Animation finished!");
}

// This needs to exist while the event loop runs
static const AnimationImplementation s_implementation = {
  .setup = implementation_setup,
  .update = implementation_update,
  .teardown = implementation_teardown
};
```

Then create a new `Animation`, specifying the custom implementation as a `const`
object pointer:

```c
// Create a new Animation
Animation *animation = animation_create();
animation_set_delay(animation, 1000);
animation_set_duration(animation, 1000);

// Create the AnimationImplementation
animation_set_implementation(animation, &s_implementation);

// Play the Animation
animation_schedule(animation);
```

The output shows the effect of the easing `AnimationCurve` on the progress value
(abridged):

```
[13:42:33] main.c:11> Animation started!
[13:42:34] main.c:19> Animation progress: 0%
[13:42:34] main.c:19> Animation progress: 0%
[13:42:34] main.c:19> Animation progress: 2%
[13:42:34] main.c:19> Animation progress: 5%
[13:42:34] main.c:19> Animation progress: 10%
[13:42:35] main.c:19> Animation progress: 17%
[13:42:35] main.c:19> Animation progress: 26%
...
[13:42:35] main.c:19> Animation progress: 97%
[13:42:35] main.c:19> Animation progress: 99%
[13:42:35] main.c:19> Animation progress: 100%
[13:42:35] main.c:23> Animation finished!
```

## Timers

`AppTimer` objects schedule updates to variables and objects at a later time. They
can implement frame-by-frame animations as an alternative to the `Animation` API,
and can schedule events in general (such as UI updates) while the app is open.

A thread-blocking alternative for small pauses is `psleep()`, but this is **not
recommended** for use in loops updating UI (such as a counter), or for scheduling
`AppMessage` messages, which rely on the event loop to do their work.

> To create timed events in the future that persist after an app is closed, see
> the `Wakeup` API.

When a timer elapses it calls a developer-defined `AppTimerCallback`. The callback
is called **only once**, so re-register the timer there if it should repeat.

```c
static void timer_callback(void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Timer elapsed!");
}
```

```c
const int delay_ms = 5000;

// Schedule the timer
app_timer_register(delay_ms, timer_callback, NULL);
```

Keep a reference if the timer may need cancelling or rescheduling:

```c
static AppTimer *s_timer;
```

```c
// Register the timer, and keep a handle to it
s_timer = app_timer_register(delay_ms, timer_callback, NULL);
```

```c
// Cancel the timer
app_timer_cancel(s_timer);
```

Cancelling an already-elapsed timer does nothing.

## Sequence and spawn animations

Composite animations are built from other `Animation` objects. Two types:

- A **sequence animation** plays two or more animations in **series** (one after
  another) — for example, a pair of timed animations to show and hide a Layer.
- A **spawn animation** plays two or more animations in **parallel**. It acts the
  same as creating and starting several animations at once, but can itself be
  included inside a sequence animation.

> Composite animations can be composed of other composite animations.

### Important considerations

- Any single animation **cannot appear more than once** in the list used to create
  a more complex animation.
- A composite animation **assumes ownership** of its component animations once
  created.
- Once added to a composite animation, a component becomes **immutable** — it can
  only be read, not written to. Attempts to modify it will fail.
- Once added to a composite animation, it **cannot be reused** to build a
  different composite animation.

### Creating a sequence animation

First create the component `Animation` objects:

```c
// Create the first Animation
PropertyAnimation *prop_anim = property_animation_create_layer_frame(s_layer,
                                                               &start, &finish);
Animation *animation_a = property_animation_get_animation(prop_anim);

// Set some properties
animation_set_delay(animation_a, 1000);
animation_set_duration(animation_a, 500);

// Clone the first, modify the duration and reverse it.
Animation *animation_b = animation_clone(animation_a);
animation_set_reverse(animation_b, true);
animation_set_duration(animation_b, 1000);
```

#### Using a list

Up to 20 `Animation` objects can be passed as parameters to
`animation_sequence_create()`. The list **must** be terminated with `NULL`.

```c
// Create the sequence
Animation *sequence = animation_sequence_create(animation_a, animation_b, NULL);

// Play the sequence
animation_schedule(sequence);
```

#### Using an array

Components can also be given as a dynamically allocated array, passed to
`animation_sequence_create_from_array()` along with the array size:

```c
const uint32_t array_length = 2;

// Create the array
Animation **arr = (Animation**)malloc(array_length * sizeof(Animation*));
arr[0] = animation_a;
arr[1] = animation_b;

// Create the sequence, set to loop forever
Animation *sequence = animation_sequence_create_from_array(arr, array_length);
animation_set_play_count(sequence, ANIMATION_DURATION_INFINITE);

// Play the sequence
animation_schedule(sequence);

// Destroy the array
free(arr);
```

`[SDK]` The guide passes `ANIMATION_DURATION_INFINITE` to
`animation_set_play_count()`. Both it and `ANIMATION_PLAY_COUNT_INFINITE` are
defined as `UINT32_MAX` in `pebble.h`, so it works, but the SDK's own doc comment
for `animation_set_play_count` names `ANIMATION_PLAY_COUNT_INFINITE`. Prefer that
for readability.

### Creating a spawn animation

Same shape as a sequence: build components, then start them together.

```c
// Create the first animation
Animation *animation_a = animation_create();
animation_set_duration(animation_a, 1000);

// Clone the first, modify the duration and reverse it.
Animation *animation_b = animation_clone(animation_a);
animation_set_reverse(animation_b, true);
animation_set_duration(animation_b, 300);
```

```c
// Create the spawn animation
Animation *spawn = animation_spawn_create(animation_a, animation_b, NULL);

// Play the animation
animation_schedule(spawn);
```

Or with an array:

```c
const uint32_t array_length = 2;

// Create the array
Animation **arr = (Animation**)malloc(array_length * sizeof(Animation*));
arr[0] = animation_a;
arr[1] = animation_b;

// Create the sequence and set the play count to 3
Animation *spawn = animation_spawn_create_from_array(arr, array_length);
animation_set_play_count(spawn, 3);

// Play the spawn animation
animation_schedule(spawn);

// Destroy the array
free(arr);
```

## Related SDK docs and examples

- [Animation](https://developer.repebble.com/docs/c/User_Interface/Animation/)
- [Timer](https://developer.repebble.com/docs/c/Foundation/Timer/)
- [AnimationImplementation](https://developer.repebble.com/docs/c/User_Interface/Animation/#AnimationImplementation)
- [Composite Animations Example](https://github.com/pebble-examples/composite-animations-example)
- [Feature Property Animation](https://github.com/pebble-examples/feature-property-animation)