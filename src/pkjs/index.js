/**
 * pl8s - phone-side JavaScript.
 *
 * Bridges the phone configuration page to the watch. The phone app shows a
 * settings gear for pl8s (declared via "capabilities": ["configurable"] in
 * package.json); tapping it fires "showConfiguration", which opens the
 * hosted config page. When the page closes with "pebblejs://close#<json>",
 * the settings are forwarded to the watch over AppMessage using the
 * messageKeys declared in package.json (STEP, MAX_LB, PLATE_*).
 */

var messageKeys = require('message_keys');

// Where the config page is hosted. Change this when you move the page.
var CONFIG_URL = 'https://cmalec.github.io/pl8s/config.html';

// Plate size -> message key name.
var PLATE_KEYS = {
  '55': 'PLATE_55',
  '45': 'PLATE_45',
  '35': 'PLATE_35',
  '25': 'PLATE_25',
  '15': 'PLATE_15',
  '10': 'PLATE_10',
  '5': 'PLATE_5',
  '2.5': 'PLATE_2P5'
};

Pebble.addEventListener('ready', function(e) {
  console.log('pl8s pkjs ready');
});

// The phone app's settings gear was tapped: open the config page.
Pebble.addEventListener('showConfiguration', function(e) {
  Pebble.openURL(CONFIG_URL);
});

// The config page closed. e.response is the JSON string that followed
// pebblejs://close# (empty if the user cancelled).
Pebble.addEventListener('webviewclosed', function(e) {
  if (!e.response || typeof e.response !== 'string' || !e.response.length) {
    console.log('pl8s config closed without a payload');
    return;
  }

  var config;
  try {
    config = JSON.parse(e.response);
  } catch (err) {
    console.log('pl8s config payload not JSON: ' + e.response);
    return;
  }

  var msg = {};
  if (typeof config.step === 'number') {
    msg[messageKeys.STEP] = config.step;
  }
  if (typeof config.max === 'number') {
    msg[messageKeys.MAX_LB] = config.max;
  }
  var plates = config.plates || {};
  Object.keys(PLATE_KEYS).forEach(function(lbs) {
    if (typeof plates[lbs] === 'number') {
      msg[messageKeys[PLATE_KEYS[lbs]]] = plates[lbs];
    }
  });

  var count = Object.keys(msg).length;
  if (count !== 0) {
    Pebble.sendAppMessage(msg,
      function() { console.log('pl8s config sent to watch (' + count + ' keys)'); },
      function() { console.log('pl8s config send failed'); });
  }
});

// The watch asked the phone to open the config page (OPEN_CONFIG message).
Pebble.addEventListener('appmessage', function(e) {
  if (e.payload && e.payload.OPEN_CONFIG) {
    if (typeof Pebble.openConfig === 'function') {
      Pebble.openConfig();
    } else {
      Pebble.openURL(CONFIG_URL);
    }
  }
});