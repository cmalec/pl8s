# One click actions

Source: <https://developer.repebble.com/guides/design-and-interaction/one-click-actions/>

One click actions give users instant access to their favorite one click watchapps
directly from the system launcher. Unlock your front door, call a car, take an
instant voice note — the user performs a single action by launching an app, and
takes no further action.

![Lockitron](https://developer.repebble.com/assets/images/guides/design-and-interaction/lockitron.png)

## The one click flow

Develop your one click application with a simple and elegant flow. Simplify the
process by essentially creating an application which serves a **single purpose**.

The typical flow:

1. Application is launched
2. Application performs action
3. Application displays status to user
4. Application automatically exits to watchface if the action was successful, or
   displays status message and does not exit if the action failed

For an instant voice note watchapp:

1. Application launched
2. Application performs action (take a voice note)
   1. Start listening for dictation
   2. Accept dictation response
3. Application displays a success message
4. Exit to watchface

For something like Uber, you would need to track the state of any existing booking
to prevent ordering a second car, and update the `App Glance` as the status of the
booking changes:

1. Application launched
2. If a booking exists:
   1. Refresh booking status
   2. Update `App Glance` with new status
   3. Exit to watchface
3. Application performs action (create a booking)
   1. Update AppGlance: "Your Uber is on it's way"
   2. Application displays a success message
   3. Exit to watchface

## Building a one click application

The worked example locks or unlocks the front door of a virtual house via
[Lockitron](https://lockitron.com/).

> **Link status.** `lockitron.com` and `api.lockitron.com` no longer resolve —
> verified 2026-09-19; the service is defunct. The guide's Lockitron-specific URLs
> (the vendor site and its API "getting started" page) are therefore dead. The
> **code is still the reference implementation of the pattern** and the
> [one-click-action-example](https://github.com/pebble-examples/one-click-action-example)
> repo is live — read Lockitron here as "whatever remote resource you are
> toggling", reached through PebbleKit JS.

The flow:

1. Launch the application
2. Take an action (toggle the state of the lock)
3. Update the `App Glance` to indicate the new lock state
4. Display a success message
5. Exit to watchface

To control the Lockitron you need the UUID of the lock and an access key:

```c
#define LOCKITRON_LOCK_UUID "95c22a11-4c9e-4420-adf0-11f1b36575f2"
#define LOCKITRON_ACCESS_TOKEN "99e75a775fe737bb716caf88f161460bb623d283c3561c833480f0834335668b"
```

> **Never publish your actual Lockitron access token in the appstore**, unless you
> want strangers unlocking your door! Ideally make these fields configurable using
> [Clay for Pebble](https://github.com/pebble-dev/clay).

An enum for the lock state, where 0 is unlocked, 1 is locked and anything else is
unknown, plus a static to track it:

```c
typedef enum {
  LOCKITRON_UNLOCKED,
  LOCKITRON_LOCKED,
  LOCKITRON_UNKNOWN
} LockitronLockState;

static LockitronLockState s_lockitron_state;
```

On launch, initialize `AppMessage` and wait for PebbleKit JS to say it is ready:

```c
static void prv_init(void) {
  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(256, 256);
  s_window = window_create();
  window_stack_push(s_window, false);
}

static void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *ready_tuple = dict_find(iter, MESSAGE_KEY_APP_READY);
  if (ready_tuple) {
    // PebbleKit JS is ready, toggle the Lockitron!
    prv_lockitron_toggle_state();
    return;
  }
  // ...
}
```

To toggle the state, send an `AppMessage` to PebbleKit JS with the UUID and access
key:

```c
static void prv_lockitron_toggle_state() {
  DictionaryIterator *out;
  AppMessageResult result = app_message_outbox_begin(&out);
  dict_write_cstring(out, MESSAGE_KEY_LOCK_UUID, LOCKITRON_LOCK_UUID);
  dict_write_cstring(out, MESSAGE_KEY_ACCESS_TOKEN, LOCKITRON_ACCESS_TOKEN);
  result = app_message_outbox_send();
}
```

PebbleKit JS handles the request, makes the ajax call to the Lockitron API, returns
the current lock state, and tells the app to exit back to the default watchface
using `AppExitReason`:

```c
static void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
  // ...
  Tuple *lock_state_tuple = dict_find(iter, MESSAGE_KEY_LOCK_STATE);
  if (lock_state_tuple) {
    // Lockitron state has changed
    s_lockitron_state = (LockitronLockState)lock_state_tuple->value->int32;
    // App will exit to default watchface
    app_exit_reason_set(APP_EXIT_ACTION_PERFORMED_SUCCESSFULLY);
    // Exit the application by unloading the only window
    window_stack_remove(s_window, false);
  }
}
```

Before the application terminates, update the `App Glance` with the current state
by passing the lock state into `app_glance_reload`:

```c
static void prv_deinit(void) {
  window_destroy(s_window);
  // Before the application terminates, setup the AppGlance
  app_glance_reload(prv_update_app_glance, &s_lockitron_state);
}
```

Only a single `AppGlanceSlice` is needed here, but you can have multiple slices with
varying expiration times:

```c
static void prv_update_app_glance(AppGlanceReloadSession *session, size_t limit, void *context) {
  // Check we haven't exceeded system limit of AppGlances
  if (limit < 1) return;

  // Retrieve the current Lockitron state from context
  LockitronLockState *lockitron_state = context;

  // Generate a friendly message for the current Lockitron state
  char *str = prv_lockitron_status_message(lockitron_state);
  APP_LOG(APP_LOG_LEVEL_INFO, "STATE: %s", str);

  // Create the AppGlanceSlice (no icon, no expiry)
  const AppGlanceSlice entry = (AppGlanceSlice) {
    .layout = {
      .template_string = str
    },
    .expiration_time = time(NULL)+3600
  };

  // Add the slice, and check the result
  const AppGlanceResult result = app_glance_add_slice(session, entry);
  if (result != APP_GLANCE_RESULT_SUCCESS) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "AppGlance Error: %d", result);
  }
}
```

The JavaScript side is in the
[full example](https://github.com/pebble-examples/one-click-action-example).

## Handling launch reasons

The example above automatically executes the One Click Action when the application
is launched — but there are additional launch reasons where that is **not**
appropriate.

Using `launch_reason()` you can detect why the application was started and prevent
the One Click Action from firing unnecessarily. A common case is to detect whether
the application was actually started by the user, from either the launcher or
quick launch:

```c
if(launch_reason() == APP_LAUNCH_USER || launch_reason() == APP_LAUNCH_QUICK_LAUNCH) {
  // Perform One Click
} else {
  // Display a message
}
```

## Related SDK docs

- [AppExitReason](https://developer.repebble.com/docs/c/Foundation/Exit_Reason/#AppExitReason)
- [AppGlanceSlice](https://developer.repebble.com/docs/c/Foundation/App_Glance/#AppGlanceSlice)
- [AppMessage](https://developer.repebble.com/docs/c/Foundation/AppMessage/)
- [launch_reason()](https://developer.repebble.com/docs/c/Foundation/Launch_Reason/#launch_reason)
- [One Click Action Example](https://github.com/pebble-examples/one-click-action-example)