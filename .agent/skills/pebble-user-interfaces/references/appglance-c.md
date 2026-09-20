# AppGlance C API

Source: <https://developer.repebble.com/guides/user-interfaces/appglance-c/>

## Overview

An app's "glance" is the visual representation of a watchapp in the launcher and
provides glanceable information to the user. The [`App Glance`](https://developer.repebble.com/docs/c/Foundation/App_Glance/) API, added in
SDK 4.0, enables developers to programmatically set the icon and subtitle that
appears alongside their app in the launcher.

> The [`App Glance`](https://developer.repebble.com/docs/c/Foundation/App_Glance/) API is only applicable to watchapps, it is not supported by
> watchfaces.

## Glances and AppGlanceSlices

An app's glance can change over time, and is defined by zero or more
[`AppGlanceSlice`](https://developer.repebble.com/docs/c/Foundation/App_Glance/#AppGlanceSlice) each consisting of a layout (including a subtitle and
icon), as well as an expiration time. AppGlanceSlices are displayed in the order
they were added, and will persist until their expiration time, or another call
to [`app_glance_reload()`](https://developer.repebble.com/docs/c/Foundation/App_Glance/#app_glance_reload).

> To create an [`AppGlanceSlice`](https://developer.repebble.com/docs/c/Foundation/App_Glance/#AppGlanceSlice) with no expiration time, use
> [`APP_GLANCE_SLICE_NO_EXPIRATION`](https://developer.repebble.com/docs/c/Foundation/App_Glance/#APP_GLANCE_SLICE_NO_EXPIRATION)

Developers can change their watchapp’s glance by calling the
[`app_glance_reload()`](https://developer.repebble.com/docs/c/Foundation/App_Glance/#app_glance_reload) method, which first clears any existing app glance
slices, and then loads zero or more [`AppGlanceSlice`](https://developer.repebble.com/docs/c/Foundation/App_Glance/#AppGlanceSlice) as specified by the
developer.

The [`app_glance_reload()`](https://developer.repebble.com/docs/c/Foundation/App_Glance/#app_glance_reload) method is invoked with two parameters: a pointer to
an [`AppGlanceReloadCallback`](https://developer.repebble.com/docs/c/Foundation/App_Glance/#AppGlanceReloadCallback) that will be invoked after the existing app
glance slices have been cleared, and a pointer to context data. Developers can
add new [`AppGlanceSlice`](https://developer.repebble.com/docs/c/Foundation/App_Glance/#AppGlanceSlice) to their app's glance in the
[`AppGlanceReloadCallback`](https://developer.repebble.com/docs/c/Foundation/App_Glance/#AppGlanceReloadCallback).

```c
// ...
// app_glance_reload callback
static void prv_update_app_glance(AppGlanceReloadSession *session,
                                       size_t limit, void *context) {
  // Create and add app glance slices...
}

static void prv_deinit() {
  // deinit code
  // ...

  // Reload the watchapp's app glance
  app_glance_reload(prv_update_app_glance, NULL);
}
```

## The app_glance_reload Callback

The [`app_glance_reload()`](https://developer.repebble.com/docs/c/Foundation/App_Glance/#app_glance_reload) is invoked with 3 parameters, a pointer to an
[`AppGlanceReloadSession`](https://developer.repebble.com/docs/c/Foundation/App_Glance/#AppGlanceReloadSession) (which is used when invoking
[`app_glance_add_slice()`](https://developer.repebble.com/docs/c/Foundation/App_Glance/#app_glance_add_slice)) , the maximum number of slices you are able to
add (as determined by the system at run time), and a pointer to the context data
that was passed into [`app_glance_reload()`](https://developer.repebble.com/docs/c/Foundation/App_Glance/#app_glance_reload). The context data should
contain all the information required to build the
[`AppGlanceSlice`](https://developer.repebble.com/docs/c/Foundation/App_Glance/#AppGlanceSlice), and is typically cast to a specific type before being
used.

> The `limit` is currently set to 8 app glance slices per watchapp, though there
> is no guarantee that this value will remain static, and developers should
> always ensure they are not adding more slices than the limit.

![Hello World app glance in the launcher](https://developer.repebble.com/assets/images/guides/appglance-c/hello-world-app-glance.png)

In this example, we’re passing the string we would like to set as the subtitle,
by using the context parameter. The full code for this example can be found in
the [AppGlance-Hello-World](https://github.com/pebble-examples/app-glance-hello-world)
repository.

```c
static void prv_update_app_glance(AppGlanceReloadSession *session,
                                       size_t limit, void *context) {
  // This should never happen, but developers should always ensure they are
  // not adding more slices than are available
  if (limit < 1) return;

  // Cast the context object to a string
  const char *message = context;

  // Create the AppGlanceSlice
  // NOTE: When .icon is not set, the app's default icon is used
  const AppGlanceSlice entry = (AppGlanceSlice) {
    .layout = {
      .icon = APP_GLANCE_SLICE_DEFAULT_ICON,
      .subtitle_template_string = message
    },
    .expiration_time = APP_GLANCE_SLICE_NO_EXPIRATION
  };

  // Add the slice, and check the result
  const AppGlanceResult result = app_glance_add_slice(session, entry);

  if (result != APP_GLANCE_RESULT_SUCCESS) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "AppGlance Error: %d", result);
  }
}
```

> **NOTE:** When an [`AppGlanceSlice`](https://developer.repebble.com/docs/c/Foundation/App_Glance/#AppGlanceSlice) is loaded with the
> [`app_glance_add_slice()`](https://developer.repebble.com/docs/c/Foundation/App_Glance/#app_glance_add_slice) method, the slice's
> `layout.subtitle_template_string` is copied to the app's glance, meaning the
> string does not need to persist after the call to
> [`app_glance_add_slice()`](https://developer.repebble.com/docs/c/Foundation/App_Glance/#app_glance_add_slice) is made.

## Using Custom Icons

In order to use custom icons within an [`AppGlanceSlice`](https://developer.repebble.com/docs/c/Foundation/App_Glance/#AppGlanceSlice), you need to use the
new `publishedMedia` entry in the `package.json` file.

- Create your images as 25px x 25px PNG files.
- Add your images as media resources in the `package.json`.
- Then add the `publishedMedia` declaration.

You should end up with something like this:

```json
"resources": {
  "media": [
    {
      "name": "WEATHER_HOT_ICON_TINY",
      "type": "bitmap",
      "file": "hot_tiny.png"
    }
  ],
  "publishedMedia": [
    {
      "name": "WEATHER_HOT",
      "id": 1,
      "glance": "WEATHER_HOT_ICON_TINY"
    }
  ]
}
```

Then you can reference the `icon` by `name` in your
[`AppGlanceSlice`](https://developer.repebble.com/docs/c/Foundation/App_Glance/#AppGlanceSlice). You must use the prefix `PUBLISHED_ID_`. E.g.
`PUBLISHED_ID_WEATHER_HOT`.

## Subtitle Template Strings

The `subtitle_template_string` field provides developers with a string
formatting language for app glance subtitles. Developers can create a single
app glance slice which updates automatically based upon a timestamp.

For example, the template can be used to create a countdown until a timestamp
(`time_until`), or the duration since a timestamp (`time_since`). The result
from the timestamp evaluation can be output in various different time-format's,
such as:

- It's 50 days until New Year
- Your Uber will arrive in 5 minutes
- You are 15515 days old

### Template Structure

The template string has the following structure:

`{evaluation(timestamp)|format(parameters)}`

Let's take a look at a simple countdown example:

`Your Uber will arrive in 1 hr 10 min 4 sec`

In this example, we need to know the time until our timestamp:
`time_until(1467834606)`, then output the duration using an abbreviated
time-format: `%aT`.

`Your Uber will arrive in {time_until(1467834606)|format('%aT')}`

### Format Parameters

Each format parameter is comprised of an optional predicate, and a time-format,
separated by a colon. The time-format parameter is only output if the predicate
evaluates to true. If a predicate is not supplied, the time-format is output by
default.

`format(predicate:'time-format')`

#### Predicate

The predicates are composed of a comparator and time value. For example, the
difference between `now` and the timestamp evaluation is:

- `>1d` Greater than 1 day
- `<12m` Less than 12 months
- `>=6m` Greater than or equal to 6 months
- `<=1d12h` Less than or equal to 1 day, 12 hours.

The supported time units are:

- `d` (Day)
- `H` (Hour)
- `M` (Minute)
- `S` (Second)

#### Time Format

The time-format is a single quoted string, comprised of a percent sign and an
optional format flag, followed by a time unit. For example:

`'%aT'` Abbreviated time. e.g. 1 hr 10 min 4 sec

The optional format flags are:

- `a` Adds abbreviated units (translated and with proper pluralization) (overrides 'u' flag)
- `u` Adds units (translated and with proper pluralization) (overrides 'a' flag)
- `-` Negates the input for this format specifier
- `0` Pad value to the "expected" number of digits with zeros
- `f` Do not modulus the value

The following table demonstrates sample output for each time unit, and the
effects of the format flags.

| Time Unit | No flag | 'u' flag | 'a' flag | '0' flag | 'f' flag |
| --- | --- | --- | --- | --- | --- |
| **y** | `<year>` | `<year> year(s)` | `<year> yr(s)` | `<year, pad to 2>` | `<year, no modulus>` |
| output: | 4 | 4 years | 4 yr | 04 | 4 |
| **m** | `<month>` | `<month> month(s)` | `<month> mo(s)` | `<month, pad to 2>` | `<month, no modulus>` |
| output: | 8 | 8 months | 8 mo | 08 | 16 |
| **d** | `<day>` | `<day> days` | `<day> d` | `<day, pad to 2>` | `<day, no modulus>` |
| output: | 7 | 7 days | 7 d | 07 | 38 |
| **H** | `<hour>` | `<hour> hour(s)` | `<hour> hr` | `<hour, pad to 2>` | `<hour, no modulus>` |
| output: | 1 | 1 hour | 1 hr | 01 | 25 |
| **M** | `<minute>` | `<minute> minute(s)` | `<minute> min` | `<minute, pad to 2>` | `<minute, no modulus>` |
| output: | 22 | 22 minutes | 22 min | 22 | 82 |
| **S** | `<second>` | `<second> second(s)` | `<second> sec` | `<second, pad to 2>` | `<second, no modulus>` |
| output: | 5 | 5 seconds | 5 sec | 05 | 65 |
| **T** | %H:%0M:%0S (if >= 1hr)<br />%M:%0S (if >= 1m)<br />%S (otherwise) | %uH, %uM, and %uS<br />%uM, and %uS<br />%uS | %aH %aM %aS<br />%aM %aS<br />%aS | %0H:%0M:%0S (always) | %fH:%0M:%0S<br />%M:%0S<br />%S |
| output: | 1:53:20<br />53:20<br />20 | 1 hour, 53 minutes, and 20 seconds<br />53 minutes, and 20 seconds<br />20 seconds | 1 hr 53 min 20 sec<br />53 min 20 sec<br />20 sec | 01:53:20<br />00:53:20<br />00:00:20 | 25:53:20<br />53:20<br />20 |
| **R** | %H:%0M (if >= 1hr)<br />%M (otherwise) | %uH, and %uM<br />%uM | %aH %aM<br />%aM | %0H:%0M (always) | %fH:%0M<br />%M |
| output: | 23:04<br />15 | 23 hours, and 4 minutes<br />15 minutes | 23 hr 4 min<br />15 min | 23:04<br />00:15 | 47:04<br />15 |

> Note: The time units listed above are not all available for use as predicates,
> but can be used with format flags.

#### Advanced Usage

We've seen how to use a single parameter to generate our output, but for more
advanced cases, we can chain multiple parameters together. This allows for a
single app glance slice to produce different output as each parameter evaluates
successfully, from left to right.

`format(predicate:'time-format', predicate:'time-format', predicate:'time-format')`

For example, we can generate a countdown which displays different output before,
during and after the event:

- 100 days left
- 10 hr 5 min 20 sec left
- It's New Year!
- 10 days since New Year

To produce this output we could use the following template:

`{time_until(1483228800)|format(>=1d:'%ud left',>0S:'%aT left',>-1d:\"It's New Year!\", '%-ud since New Year')}`

## Adding Multiple Slices

An app's glance can change over time, with the slices being displayed in the
order they were added, and removed after the `expiration_time`. In order to add
multiple app glance slices, we simply need to create and add multiple
[`AppGlanceSlice`](https://developer.repebble.com/docs/c/Foundation/App_Glance/#AppGlanceSlice) instances, with increasing expiration times.

![Virtual Pet app glance in the launcher](https://developer.repebble.com/assets/images/guides/appglance-c/virtual-pet-app-glance.png)

In the following example, we create a basic virtual pet that needs to be fed (by
opening the app) every 12 hours, or else it runs away. When the app closes, we
update the app glance to display a new message and icon every 3 hours until the
virtual pet runs away. The full code for this example can be found in the
[AppGlance-Virtual-Pet](https://github.com/pebble-examples/app-glance-virtual-pet)
repository.

```c
// How often pet needs to be fed (12 hrs)
#define PET_FEEDING_FREQUENCY 3600*12
// Number of states to show in the launcher
#define NUM_STATES 4

// Icons associated with each state
const uint32_t icons[NUM_STATES] = {
  PUBLISHED_ID_ICON_FROG_HAPPY,
  PUBLISHED_ID_ICON_FROG_HUNGRY,
  PUBLISHED_ID_ICON_FROG_VERY_HUNGRY,
  PUBLISHED_ID_ICON_FROG_MISSING
};

// Message associated with each state
const char *messages[NUM_STATES] = {
  "Mmm, that was delicious!!",
  "I'm getting hungry..",
  "I'm so hungry!! Please feed me soon..",
  "Your pet ran away :("
};

static void prv_update_app_glance(AppGlanceReloadSession *session,
                                       size_t limit, void *context) {

  // Ensure we have sufficient slices
  if (limit < NUM_STATES) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Error: app needs %d slices (%zu available)",
                                                            NUM_STATES, limit);
  }

  time_t expiration_time = time(NULL);

  // Build and add NUM_STATES slices
  for (int i = 0; i < NUM_STATES; i++) {
    // Increment the expiration_time of the slice on each pass
    expiration_time += PET_FEEDING_FREQUENCY / NUM_STATES;

    // Set it so the last slice never expires
    if (i == (NUM_STATES - 1)) expiration_time = APP_GLANCE_SLICE_NO_EXPIRATION;

    // Create the slice
    const AppGlanceSlice slice = {
      .layout = {
        .icon = icons[i],
        .subtitle_template_string = messages[i]
      },
      .expiration_time = expiration_time
    };

    // add the slice, and check the result
    AppGlanceResult result = app_glance_add_slice(session, slice);
    if (result != APP_GLANCE_RESULT_SUCCESS) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Error adding AppGlanceSlice: %d", result);
    }
  }
}

static void prv_deinit() {
  app_glance_reload(prv_update_app_glance, NULL);
}

void main() {
  app_event_loop();
  prv_deinit();
}
```
