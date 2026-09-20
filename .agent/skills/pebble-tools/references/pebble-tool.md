# Command Line Tool

Source: https://developer.repebble.com/guides/tools-and-resources/pebble-tool/

The Pebble SDK includes a command line tool called `pebble`. This tool allows
developers to create new projects, build projects, install apps on a watch and
debug them.

## Enabling the Developer Connection

Most `pebble` commands allow interaction with a Pebble watch. This relies on a
communication channel opened between the `pebble` tool on a computer and the
Pebble mobile application on the phone.

The `pebble` tool requires two configuration steps:

1. Enable the [*Developer Connection*](https://developer.repebble.com/guides/tools-and-resources/developer-connection/) in the
Pebble mobile application.
2. Give the phone IP address to the `pebble` tool as shown below to communicate
with a watch. This is not required when using the emulator.

## Connecting to a Pebble

There are four connection types possible with the command line tool. These can
be used with any command that involves a watch, for example `install`.

Connect to a physical watch connected to a phone on the same Wi-Fi network with `IP` [IP address](#enabling-the-developer-connection):

```sh
$ pebble install --phone IP
```

Connect to an already running QEMU instance available on the `HOST` and `PORT` provided:

```sh
$ pebble install --qemu HOST:PORT
```

Connect directly to a watch using a local Bluetooth connection, where `SERIAL` is the path to the device file. On OS X, this is similar to `/dev/cu.PebbleTimeXXXX-SerialPo` or `/dev/cu.PebbleXXXX-SerialPortSe`:
> Note: Replace 'XXXX' with the actual value for the watch in question.

```sh
$ pebble install --serial SERIAL
```

## Configure the Pebble Tool

Save the IP address of the phone in an environment variable:

```sh
$ export PEBBLE_PHONE=192.168.1.42
```

Save the default choice of emulator platform:

```sh
$ export PEBBLE_EMULATOR=aplite
```

Save the default instance of QEMU:

```sh
$ export PEBBLE_QEMU=localhost:12344
```

## Debugging Output

Get up to four levels of `pebble` tool log verbosity. This is available for all
commands:

```sh
# Minimum level of verbosity
$ pebble install -v

# Maximum logging verbosity
$ pebble install -vvvv
```

## Installing Watchapps

In addition to interacting with a physical Pebble watch, the Pebble emulator
included in the local SDK can be used to run and debug apps without any physical
hardware.

Build an app, then use `pebble install` with the `--emulator` flag to choose
between an emulator platform with `aplite`, `basalt`, or `chalk`. This allows
developers to test out apps on all platforms before publishing to the Pebble
appstore. For example, to emulate Pebble Time or Pebble Time Steel:

```sh
$ pebble install --emulator basalt
```
> Note: If no `--emulator` parameter is specified, the running emulator will
> be used.

With a suitable color app installed, the emulator will load and display it:

![The Pebble emulator running a color app](https://developer.repebble.com/assets/images/guides/publishing-tools/emulator.png)

The Pebble physical buttons are emulated with the following mapping:

| Button | Key              |
| ------ | ---------------- |
| Back   | `Q`, left arrow  |
| Up     | `W`, up arrow    |
| Select | `S`, right arrow |
| Down   | `X`, down arrow  |

## Pebble Timeline in the Emulator

With the emulator, it is also possible to view any past and future pins on the
Pebble timeline by pressing Up or Down from the main watchface screen:

| ![](https://developer.repebble.com/assets/images/guides/publishing-tools/timeline-past.png) | ![](https://developer.repebble.com/assets/images/guides/publishing-tools/timeline-future.png) |
| ------------------------------------------------------------- | --------------------------------------------------------------- |
| Timeline, past view                                           | Timeline, future view                                           |

## Commands

This section summarizes the available commands that can be used in conjunction
with the `pebble` tool, on the applicable SDK versions. Most are designed to
interact with a watch with `--phone` or the emulator with `--emulator`.

### Project Management

#### new-project

```sh
$ pebble new-project [--simple] [--javascript] [--worker] [--rocky] NAME
```

Create a new project called `NAME`. This will create a directory in the
location the command is used, containing all essential project files.

There are also a number of optional parameters that allow developers to choose
features of the generated project to be created automatically:

- `--simple` - Initializes the main `.c` file with a minimal app template.

- `--javascript` - Creates a new application including a `./src/pkjs/index.js` file
to quickly start an app with a PebbleKit JS component. Read [*PebbleKit JS*](https://developer.repebble.com/guides/communication/using-pebblekit-js/) for more information.

- `--worker` - Creates a new application including a `
./worker_src/NAME_worker.c` file to quickly start an app with a background
worker. Read [*Background Worker*](https://developer.repebble.com/guides/events-and-services/background-worker/) for more
information.

- `--rocky` - Creates a new Rocky.js application. Do not use any other optional
parameters with this command. Read [Rocky.js documentation](https://developer.repebble.com/docs/rockyjs/) for more information.

#### build

```sh
$ pebble build
```

Compile and build the project into a `.pbw` file that can be installed on a
watch or the emulator.

#### install

```sh
$ pebble install [FILE]
```

Install the current project to the watch connected to the phone with the given `IP` address, or to the emulator on the `PLATFORM`. See [*Hardware Information*](https://developer.repebble.com/guides/tools-and-resources/hardware-information/) for a list of
available platforms.

For example, the Aplite platform is specified as follows:

```sh
$ pebble install --emulator aplite
```

It is also possible to use this command to install a pre-compiled `.pbw` `FILE`.
In this case, the specified file will be installed instead of the current
project.
> Note: A `FILE` parameter is not required if running `pebble install` from
> inside a buildable project directory. In this case, the `.pbw` package is
> found automatically in `./build`.

#### clean

```sh
$ pebble clean
```

Delete all build files in `./build` to prepare for a clean build, which can
resolve some state inconsistencies that may prevent the project from building.

#### convert-project

```sh
$ pebble convert-project
```

Convert an existing Pebble project to the current SDK.
> Note: This will only convert the project, the source code will still have to
> updated to match any new APIs.

### Pebble Interaction

#### logs

```sh
$ pebble logs
```

Continuously display app logs from the watch connected to the phone with the
given `IP` address, or from a running emulator.

App log output is colorized according to log level. This behavior can be forced
on or off by specifying `--color` or `--no-color` respectively.

#### screenshot

```sh
$ pebble screenshot [FILENAME]
```

Take a screenshot of the watch connected to the phone with the given `IP` address, or from any running emulator. If provided, the output is saved to `FILENAME`.

Color correction may be disabled by specifying `--no-correction`. The
auto-opening of screenshots may also be disabled by specifying `--no-open`.

#### ping

```sh
$ pebble ping
```

Send a `ping` to the watch connected to the phone with the given `IP` address,
or to any running emulator.

#### repl

```sh
$ pebble repl
```

Launch an interactive python shell with a `pebble` object to execute methods on,
using the watch connected to the phone with the given `IP` address, or to any
running emulator.

#### data-logging

##### list

```sh
$ pebble data-logging list
```

List the current data logging sessions on the watch.

##### download

```sh
$ pebble data-logging download --session-id SESSION-ID FILENAME
```

Downloads the contents of the data logging session with the given ID
(as given by `pebble data-logging list`) to the given filename.

##### disable-sends

```sh
pebble data-logging disable-sends
```

Disables automatically sending data logging to the phone. This enables
downloading data logging information without the phone's interference.
It will also suspend any other app depending on data logging, which
includes some firmware functionality. You should remember to enable
it once you are done testing.

##### enable-sends

```sh
pebble data-logging enable-sends
```

Re-enables automatically sending data logging information to the phone.

##### get-sends-enabled

```sh
pebble data-logging get-sends-enabled
```

Indicates whether data logging is automatically sent to the phone. Change
state with `enable-sends` and `disable-sends`.

### Emulator Interaction

#### gdb

```sh
$ pebble gdb
```

Use [GDB](https://www.gnu.org/software/gdb/) to step through and debug an app
running in an emulator. Some useful commands are displayed in a cheat sheet
when run with `--help` and summarized in the table below:
> Note: Emulator commands that rely on interaction with the emulator (e.g. `emu-app-config`) will not work while execution is paused with GDB.

| Command             | Description                                                                                                                                                                    |
| ------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| ctrl-C              | Pause app execution.                                                                                                                                                           |
| `continue` or `c`   | Continue app execution. The app is paused while a `(gdb)` prompt is available.                                                                                                 |
| ctrl-D or `quit`    | Quit gdb.                                                                                                                                                                      |
| `break` or `b`      | Set a breakpoint. This can be either a symbol or a position: - `b function_name` to break when entering a function. - `b file_name.c:45` to break on line 45 of `file_name.c`. |
| `step` or `s`       | Step forward one line.                                                                                                                                                         |
| `next` or `n`       | Step *over* the current line, avoiding stopping for any functions it calls into.                                                                                               |
| `finish`            | Run forward until exiting the current stack frame.                                                                                                                             |
| `backtrace` or `bt` | Print out the current call stack.                                                                                                                                              |
| `p [expression]`    | Print the result of evaluating the given `expression`.                                                                                                                         |
| `info args`         | Show the values of arguments to the current function.                                                                                                                          |
| `info locals`       | Show local variables in the current frame.                                                                                                                                     |
| `bt full`           | Show all local variables in all stack frames.                                                                                                                                  |
| `info break`        | List break points (#1 is `<app_crashed>`, and is inserted by the `pebble` tool).                                                                                               |
| `delete [n]`        | Delete breakpoint #n.                                                                                                                                                          |

Read [*Debugging with GDB*](https://developer.repebble.com/guides/debugging/debugging-with-gdb/) for help on using GDB to
debug app.

#### emu-control

```sh
$ pebble emu-control [--port PORT]
```

Send near real-time accelerometer and compass readings from a phone, tablet, or
computer to the emulator. When run, a QR code will be displayed in the terminal
containing the IP address the device should connect to. Scanning the code will
open this address in the device's browser. The IP address itself is also printed
in the terminal for manual entry. `PORT` may be specified in order to make
bookmarking the page easier.

![A QR code displayed in the terminal](https://developer.repebble.com/assets/images/guides/publishing-tools/qr-code.png)

After connecting, the device's browser will display a UI with two modes of
operation.

![The accelerometer and compass control web UI](https://developer.repebble.com/assets/images/guides/publishing-tools/accel-ui.png)

The default mode will transmit sensor readings to the emulator as they are
recorded. By unchecking the 'Use built-in sensors?' checkbox, manual values for
the compass bearing and each of the accelerometer axes can be transmitted to the
emulator by manipulating the compass rose or axis sliders. Using these UI
elements will automatically uncheck the aforementioned checkbox.

The values shown in bold are the Pebble SDK values, scaled relative to [`TRIG_MAX_ANGLE`](https://developer.repebble.com/docs/c/Foundation/Math/#TRIG_MAX_ANGLE) in the case of the compass bearing value, and in the range of
+/-4000 (+/- 4g) for the accelerometer values. The physical measurements with
associated units are shown beside in parentheses.

#### emu-app-config

```sh
$ pebble emu-app-config [--file FILE]
```

Open the app configuration page associated with the running app, if any. Uses
the HTML configuration `FILE` if provided.

See [*App Configuration*](https://developer.repebble.com/guides/user-interfaces/app-configuration/) to learn about emulator-
specific configuration behavior.

#### emu-tap

```sh
$ pebble emu-tap [--direction DIRECTION]
```

Send an accelerometer tap event to any running emulator. `DIRECTION` can be one
of `x+`, `x-`, `y+`, `y-`, `z+`, or `z-`.

#### emu-bt-connection

```sh
$ pebble emu-bt-connection --connected STATE
```

Send a Bluetooth connection event to any running emulator. `STATE` can be either `yes` or `no` for connected and disconnected events respectively.
> Note: The disconnected event may take a few seconds to occur.

#### emu-compass

```sh
$ pebble emu-compass --heading BEARING [--uncalibrated | --calibrating | --calibrated]
```

Send a compass update event to any running emulator. `BEARING` must be a number
between `0` and `360` to be the desired compass bearing. Use any of `--uncalibrated`, `--calibrating`, or `--calibrated` to set the calibration
state.

#### emu-battery

```sh
$ pebble emu-battery [--percent LEVEL] [--charging]
```

Send a battery update event to any running emulator. `LEVEL` must be a number
between `0` and `100` to represent the new battery level. The presence of `--charging` will determine whether the charging cable is plugged in.

#### emu-accel

```sh
$ pebble emu-accel DIRECTION [--file FILE]
```

Send accelerometer data events to any running emulator. `DIRECTION` can be any
of `tilt_left`, `tilt_right`, `tilt_forward`, `tilt_back`, `gravity+x`, `gravity-x`, `gravity+y`, `gravity-y`, `gravity+z`, `gravity-z` , and `custom`.
If `custom` is selected, specify a `FILE` of comma-separated x, y, and z
readings.

#### transcribe

```sh
$ pebble transcribe [message] [--error {connectivity,disabled,no-speech-detected}]
```

Run a server that will act as a transcription service. Run it before
invoking the [`Dictation`](https://developer.repebble.com/docs/c/Foundation/Dictation/) service in an app. If `message` is provided,
the dictation will be successful and that message will be provided.
If `--error` is provided, the dictation will fail with the given error. `--error` and `message` are mutually exclusive.

```sh
$ pebble transcribe "Hello, Pebble!"
```

#### kill

```sh
$ pebble kill
```

Kill both the Pebble emulator and phone simulator.

#### emu-time-format

```sh
pebble emu-time-format --format FORMAT
```

Set the time format of the emulator to either 12-hour or 24-hour format, with a `FORMAT` value of `12h` or `24h` respectively.

#### emu-set-timeline-quick-view

```sh
$ pebble emu-set-timeline-quick-view STATE
```

Show or hide the Timeline Quick View system overlay. STATE can be `on` or `off`.

#### wipe

```sh
$ pebble wipe
```

Wipe data stored for the Pebble emulator, but not the logged in Pebble account.
To wipe **all** data, specify `--everything` when running this command.

### Pebble Account Management

#### login

```sh
$ pebble login
```

Launces a browser to log into a Pebble account, enabling use of `pebble` tool
features such as pushing timeline pins.

#### logout

```sh
$ pebble logout
```

Logs out the currently logged in Pebble account from this instance of the
command line tool.

### Pebble Timeline Interaction

#### insert-pin

```sh
$ pebble insert-pin FILE [--id ID]
```

Push a JSON pin `FILE` to the Pebble timeline. Specify the pin `id` in the `FILE` as `ID`.

#### delete-pin

```sh
$ pebble delete-pin FILE [--id ID]
```

Delete a pin previously pushed with `insert-pin`, specifying the same pin `ID`.

## Data Collection for Analytics

When first run, the `pebble` tool will ask for permission to collect usage
information such as which tools are used and the most common errors. Nothing
personally identifiable is collected.

This information will help us improve the SDK and is extremely useful for us,
allowing us to focus our time on the most important parts of the SDK as
discovered through these analytics.

To disable analytics collection, run the following command to stop sending
information:

```sh
# Mac OSX
$ touch ~/Library/Application\ Support/Pebble\ SDK/NO_TRACKING

# Other platforms
$ touch ~/.pebble-sdk/NO_TRACKING
```
