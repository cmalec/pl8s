# AppGlance REST API

Source: <https://developer.repebble.com/guides/user-interfaces/appglance-rest/>

**Important Note**

This API requires the forthcoming v4.1 of the Pebble mobile application in
order to display App Glances on the connected watch.

## Overview

This guide explains how to use the AppGlance REST API. The [`App Glance`](https://developer.repebble.com/docs/c/Foundation/App_Glance/) API
was added in SDK 4.0 and enables developers to programmatically set the icon and
subtitle that appears alongside their app in the launcher.

If you want to learn more about [`App Glance`](https://developer.repebble.com/docs/c/Foundation/App_Glance/), please read the [*AppGlance C API*](https://developer.repebble.com/guides/user-interfaces/appglance-c/) guide.

## The REST API

The AppGlance REST API shares many similarities with the existing [*timeline API*](https://developer.repebble.com/guides/pebble-timeline/timeline-public/).
Developers can push slices to the their app's glance using their own backend
servers. Slices are created using HTTPS requests to the Pebble AppGlance REST
API.

#### Creating Slices

To create a slice, send a `PUT` request to the following URL scheme:

```
PUT https://timeline-api.getpebble.com/v1/user/glance
```

> `[SDK]` **That host is defunct.** `timeline-api.getpebble.com` is the original
> Pebble endpoint and no longer resolves; the live service is
> **`https://timeline-api.rebble.io/v1/user/glance`** (verified 2026-09-19 —
> `timeline-api.rebble.io` resolves, and the newer revision of this guide in
> `pebble-dev/developer.rebble.io` already uses it). The rest of the request shape
> below is unchanged.

Use the following headers, where `X-User-Token` is the user's
timeline token (read [*Get a Timeline Token*](https://developer.repebble.com/guides/pebble-timeline/timeline-js/#get-a-timeline-token) to learn how to get a token):

```
Content-Type: application/json
X-User-Token: a70b23d3820e9ee640aeb590fdf03a56
```

Include the JSON object as the request body from a file such as `glance.json`. A
sample of an object is shown below:

```json
{
  "slices": [
    {
      "layout": {
        "icon": "system://images/GENERIC_CONFIRMATION",
        "subtitleTemplateString": "Success!"
      }
    }
  ]
}
```

#### Curl Example

```sh
$ curl -X PUT https://timeline-api.getpebble.com/v1/user/glance \
    --header "Content-Type: application/json" \
    --header "X-User-Token: a70b23d3820e9ee640aeb590fdf03a56" \
    -d @glance.json
OK
```

#### Slice Icons

There are two types of resources which can be used for AppGlance icons.

- You can use system images. E.g. `system://images/HOTEL_RESERVATION`
- You can use custom images by utilizing the
[*Published Media*](https://developer.repebble.com/guides/tools-and-resources/app-metadata/#published-media) `name`. E.g. `app://images/*name*`

#### Subtitle Template Strings

The `subtitle_template_string` field provides developers with a string
formatting language for app glance subtitles. Read more in the [*AppGlance C guide*](https://developer.repebble.com/guides/user-interfaces/appglance-c/#subtitle-template-strings).

#### Expiring Slices

When you want your slice to expire automatically, just provide an `expirationTime` in [ISO date-time](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/toISOString) format and the system will automatically remove it upon expiry.

```json
{
  "slices": [
    {
      "layout": {
        "icon": "system://images/GENERIC_CONFIRMATION",
        "subtitleTemplateString": "Success!"
      },
      "expirationTime": "2016-12-31T23:59:59.000Z"
    }
  ]
}
```

#### Creating Multiple Slices

Because `slices` is an array, you can send multiple slices within a single
request. The system is responsible for displaying the correct entries based on
the `expirationTime` provided in each slice.

```json
{
  "slices": [
    {
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
    }
  ]
}
```

#### Updating Slices

There isn't a concept of updating an AppGlance slice, just send a request to
the REST API with new slices and any existing slices will be replaced.

#### Deleting Slices

All you need to do is send an empty slices array to the REST API and any
existing slices will be removed.

```json
{
  "slices": []
}
```

### Additional Notes

We will not display App Glance slices for SDK 3.0 applications under any
circumstances. Your watchapp needs to be compiled with SDK 4.0 in order to
support App Glances.
