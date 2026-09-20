# App Exit Reason

Source: <https://developer.repebble.com/guides/user-interfaces/app-exit-reason/>

Introduced in SDK v4.0, the [`AppExitReason`](https://developer.repebble.com/docs/c/Foundation/Exit_Reason/#AppExitReason) API allows developers to provide a
reason when terminating their application. The system uses these reasons to
determine where the user should be sent when the current application terminates.

At present, there are only two [`AppExitReason`](https://developer.repebble.com/docs/c/Foundation/Exit_Reason/#AppExitReason) states when exiting an application, but this may change in future updates.

### APP_EXIT_NOT_SPECIFIED

This is the default state and when the current watchapp terminates. The user is
returned to their previous location. If you do not specify an [`AppExitReason`](https://developer.repebble.com/docs/c/Foundation/Exit_Reason/#AppExitReason),
this state will be used automatically.

```c
static void prv_deinit() {
    // Optional, default behavior
    // App will exit to the previous location in the system
    exit_reason_set(APP_EXIT_NOT_SPECIFIED);
}
```

### APP_EXIT_ACTION_PERFORMED_SUCCESSFULLY

This state is primarily provided for developers who are creating one click
action applications. When the current watchapp terminates, the user is returned
to the default watchface.

```c
static void prv_deinit() {
    // App will exit to default watchface
    exit_reason_set(APP_EXIT_ACTION_PERFORMED_SUCCESSFULLY);
}
```
