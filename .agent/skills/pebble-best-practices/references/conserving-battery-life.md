# Conserving battery life

Source: <https://developer.repebble.com/guides/best-practices/conserving-battery-life/>

Pebble's long battery life comes from a low-power display, conservative backlight
use, and letting the processor **sleep whenever possible**. Apps that misuse
high-power APIs, or that prevent power-saving mechanisms from working, detract
from the user's battery life.

The unifying principle: anything that **keeps the watch awake** costs
significantly. Frequent animations, sensors, Bluetooth and vibration are the
usual culprits.

## Time awake

### Animations and display updates

Long-running animations that cause frequent display updates are a common drain. A
watchface playing a half-second `Animation` every second drains faster than one
that does so once a minute — the latter leaves far more time for the watch to
sleep.

```c
static void tick_handler(struct tm *tick_time, TimeUnits changed) {
  // Update time
  set_time_digits(tick_time);

  // Only update once a minute
  if(tick_time->tm_sec == 0) {
    play_animation();
  }
}
```

The same applies to short-interval `Timer`s, which are another way to create
animations. Consider letting users reduce or disable animations, and removing or
shortening animations that are not essential.

**Not all animations are bad** — play them at more intelligent times. For example
when the user raises their arm to view the screen (see
[pebble_glancing_demo](https://github.com/pebble-hacks/pebble_glancing_demo)), or
only on a tap or wrist shake:

```c
static void accel_tap_handler(AccelAxisType axis, int32_t direction) {
  // Animate when the user flicks their wrist
  play_animation();
}
```

```c
accel_tap_service_subscribe(tap_handler);
```

### Tick updates

Watchfaces often tick once a second with `SECOND_UNIT` when they only update the
display once a minute. `MINUTE_UNIT` reduces how often the watch is woken:

```c
// Only tick once a minute, much more time asleep
tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
```

Extremely minimal watchfaces may use `HOUR_UNIT` to update only once per hour.
Where possible, let users disable the second-hand tick and/or animation.

**Especially important on Pebble Time Round**, where reduced battery capacity
means a watchface animating every second can drop battery life to a day or less.
Consider offering configuration options to reduce tick updates on that platform.

### Sensor usage

Frequent accelerometer and compass use prevents sleep. `AccelerometerService`
can configure sampling rate and samples per update, letting you batch data into
less frequent updates:

```c
// Batch samples into sets of 10 per callback
const uint32_t num_samples = 10;

// Sample at 10 Hz
accel_service_set_sampling_rate(ACCEL_SAMPLING_10HZ);

// With this combination, only wake up the app once per second!
accel_data_service_subscribe(num_samples, accel_data_handler);
```

`CompassService` allows a filter on heading updates, so an app is notified only on
significant angle changes:

```c
// Only update if the heading changes significantly
compass_service_set_heading_filter(TRIG_MAX_ANGLE / 36);
```

Frequent use of `Dictation` also keeps the watch awake and keeps the **Bluetooth
connection alive**. Consider the `Storage` API to remember previous input and
present a list of previous inputs instead:

```c
static void dictation_session_callback(DictationSession *session, DictationSessionStatus status,
                                       char *transcription, void *context) {
  if(status == DictationSessionStatusSuccess) {
    // Display the dictated text
    snprintf(s_last_text, sizeof(s_last_text), "Transcription:\n\n%s",
                                                                transcription);
    text_layer_set_text(s_output_layer, s_last_text);

    // Save for later!
    const int last_text_key = 0;
    persist_write_string(last_text_key, s_last_text);
  }
}
```

### Bluetooth usage

Frequent `AppMessage` use puts the Bluetooth connection into a **more responsive
state**, which consumes much more power. A short time after a message is sent, the
connection returns to low-power.

The 'sniff interval' determines how often the API checks for new messages from the
phone, and should be left in the default `SNIFF_INTERVAL_NORMAL` state as much as
possible. Cache data obtained over Bluetooth with the `Storage` API to reduce
update frequency (for example, weather information in a watchface).

If the reduced sniff state must be used to transfer large amounts of data quickly,
return to the low-power state as soon as the transfer completes:

```c
// Return to low power Bluetooth state
app_comm_set_sniff_interval(SNIFF_INTERVAL_NORMAL);
```

## Backlight usage

The backlight LED is another large consumer. System-level settings may turn it on
for a few seconds on every button press. That is out of the developer's hands, but
apps can reduce backlight on-time by **minimizing the number of button presses**
needed to operate them — for example, an `ActionBarLayer` to execute common
actions with one press instead of a long scrolling `MenuLayer`.

The `Light` API can turn the backlight on manually, but should not be used for
more than very short periods, if at all. **Apps that keep the backlight on all the
time will not last more than a few hours.** If it must stay on for an extended
period, return to automatic mode as soon as possible:

```c
// Return to automatic backlight control
light_enable(false);
```

## Vibration motor usage

As a physical electrical-to-mechanical converter, the vibration motor consumes a
lot of power. Users can use Quiet Time or disable notification vibration, but apps
can help: keep `Vibes` use to a minimum, give users the option to disable any
vibrations the app emits, and shorten custom sequences.

## Learn more

[Presentation given at the 2014 Developer Retreat](https://www.youtube.com/watch?v=TS0FPfgxAso).

Related SDK docs:
[Animation](https://developer.repebble.com/docs/c/User_Interface/Animation/),
[Timer](https://developer.repebble.com/docs/c/Foundation/Timer/),
[AccelerometerService](https://developer.repebble.com/docs/c/Foundation/Event_Service/AccelerometerService/),
[BatteryStateService](https://developer.repebble.com/docs/c/Foundation/Event_Service/BatteryStateService/),
[TickTimerService](https://developer.repebble.com/docs/c/Foundation/Event_Service/TickTimerService/),
[CompassService](https://developer.repebble.com/docs/c/Foundation/Event_Service/CompassService/),
[Vibes](https://developer.repebble.com/docs/c/User_Interface/Vibes/),
[Light](https://developer.repebble.com/docs/c/User_Interface/Light/).

Example: [Pebble Glancing Demo](https://github.com/pebble-hacks/pebble_glancing_demo).