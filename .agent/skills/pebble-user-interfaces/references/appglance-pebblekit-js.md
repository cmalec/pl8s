# AppGlance in PebbleKit JS

Source: <https://developer.repebble.com/guides/user-interfaces/appglance-pebblekit-js/>

## Overview

This guide explains how to manage your app's glances via PebbleKit JS. The [`App Glance`](https://developer.repebble.com/docs/c/Foundation/App_Glance/) API was added in SDK 4.0 and enables developers to programmatically set the icon and subtitle that appears alongside their app in the launcher.

If you want to learn more about [`App Glance`](https://developer.repebble.com/docs/c/Foundation/App_Glance/), please read the [*AppGlance C API*](https://developer.repebble.com/guides/user-interfaces/appglance-c/) guide.

### Creating Slices

To create a slice, call `Pebble.appGlanceReload()`. The first parameter is an array of AppGlance slices, followed by a callback for success and one for failure.

```javascript
// Construct the app glance slice object
var appGlanceSlices = [{
  "layout": {
    "icon": "system://images/HOTEL_RESERVATION",
    "subtitleTemplateString": "Nice Slice!"
  }
}];

function appGlanceSuccess(appGlanceSlices, appGlanceReloadResult) {
  console.log('SUCCESS!');
};

function appGlanceFailure(appGlanceSlices, appGlanceReloadResult) {
  console.log('FAILURE!');
};

// Trigger a reload of the slices in the app glance
Pebble.appGlanceReload(appGlanceSlices, appGlanceSuccess, appGlanceFailure);
```

### Slice Icons

There are two types of resources which can be used for AppGlance icons.

- You can use system images. E.g. `system://images/HOTEL_RESERVATION`
- You can use custom images by utilizing the [*Published Media*](https://developer.repebble.com/guides/tools-and-resources/app-metadata/#published-media) `name`. E.g. `app://images/*name*`

### Subtitle Template Strings

The `subtitle_template_string` field provides developers with a string formatting language for app glance subtitles. Read more in the [*AppGlance C guide*](https://developer.repebble.com/guides/user-interfaces/appglance-c/#subtitle-template-strings).

### Expiring Slices

When you want your slice to expire automatically, just provide an `expirationTime` in [ISO date-time](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/toISOString) format and the system will automatically remove it upon expiry.

```javascript
var appGlanceSlices = [{
  "layout": {
    "icon": "system://images/HOTEL_RESERVATION",
    "subtitleTemplateString": "Nice Slice!"
  },
  "expirationTime": "2016-12-31T23:59:59.000Z"
}];
```

### Creating Multiple Slices

Because `appGlanceSlices` is an array, we can pass multiple slices within a single function call. The system is responsible for displaying the correct entries based on the `expirationTime` provided in each slice.

```javascript
var appGlanceSlices = [{
  "layout": {
    "icon": "system://images/DINNER_RESERVATION",
    "subtitleTemplateString": "Lunchtime!"
  },
  "expirationTime": "2017-01-01T12:00:00.000Z"
},
{
  "layout": {
    "icon": "system://images/RESULT_MUTE",
    "subtitleTemplateString": "Nap Time!"
  },
  "expirationTime": "2017-01-01T14:00:00.000Z"
}];
```

### Updating Slices

There isn't a concept of updating an AppGlance slice, just call `Pebble.appGlanceReload()` with the new slices and any existing slices will be replaced.

### Deleting Slices

All you need to do is pass an empty slices array and any existing slices will be removed.

```javascript
Pebble.appGlanceReload([], appGlanceSuccess, appGlanceFailure);
```

### Related SDK Docs

- [AppGlanceSlice](https://developer.repebble.com/docs/c/Foundation/App_Glance/#AppGlanceSlice)

### Examples

- [PebbleKit JS Example](https://github.com/pebble-examples/app-glance-pebblekit-js-example)
