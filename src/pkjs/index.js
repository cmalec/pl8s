/**
 * pl8s - phone-side JavaScript.
 *
 * Bridges the phone configuration page to the watch. The phone app shows a
 * settings gear for pl8s (declared via "capabilities": ["configurable"] in
 * package.json); tapping it fires "showConfiguration", which opens the
 * hosted config page. The page mirrors the on-watch settings wizard (step,
 * max, per-plate counts) and prefills the current values, which the watch
 * pushes on request (SYNC). When the page closes with "pebblejs://close#<json>",
 * the settings are forwarded back to the watch over AppMessage using the
 * messageKeys declared in package.json (STEP, MAX_LB, PLATE_*).
 */

var messageKeys = require('message_keys');

// Where the config page is hosted. Change this when you move the page.
var CONFIG_URL = 'https://cmalec.github.io/pl8s/config.html';

var STATE_KEY = 'pl8s_state';  // localStorage cache of the watch's settings

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

function requestState() {
  // Ask the watch for its current settings.
  var req = {};
  req[messageKeys.SYNC] = 1;
  Pebble.sendAppMessage(req, null, function() {
    console.log('pl8s state request failed; config page will use defaults');
  });
}

// Render the cached state as a query string for the config page.
function stateQuery() {
  var state = JSON.parse(localStorage.getItem(STATE_KEY) || 'null');
  if (!state || typeof state.step !== 'number' ||
      typeof state.max !== 'number' || !state.plates) {
    return '';
  }
  var parts = ['step=' + state.step, 'max=' + state.max];
  Object.keys(PLATE_KEYS).forEach(function(lbs) {
    if (typeof state.plates[lbs] === 'number') {
      parts.push('p' + lbs.replace('.', 'p') + '=' + state.plates[lbs]);
    }
  });
  return '?' + parts.join('&');
}

Pebble.addEventListener('ready', function(e) {
  requestState();
});

// The phone app's settings gear was tapped: open the config page prefilled
// with the last known watch state (and refresh it in the background).
Pebble.addEventListener('showConfiguration', function(e) {
  Pebble.openURL(CONFIG_URL + stateQuery());
  requestState();
});

// The watch pushes its settings (reply to a SYNC request).
Pebble.addEventListener('appmessage', function(e) {
  var p = e.payload;
  var plates = {};
  var ok = (typeof p.step === 'number' && typeof p.max === 'number');
  Object.keys(PLATE_KEYS).forEach(function(lbs) {
    var v = p[PLATE_KEYS[lbs]];
    if (typeof v === 'number') {
      plates[lbs] = v;
    } else {
      ok = false;
    }
  });
  if (ok) {
    localStorage.setItem(STATE_KEY, JSON.stringify({step: p.step, max: p.max, plates: plates}));
  }
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