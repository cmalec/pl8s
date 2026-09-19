# Modular app architecture

Source: <https://developer.repebble.com/guides/best-practices/modular-app-architecture/>

Most Pebble projects (such as a simple watchface) work fine as a single-file
project — all code in one `.c` file. As that file grows it becomes harder to track
where components live and how they interact. A hypothetical app may have many
`Window`s, communicate over `AppMessage` with many data types, persist a large
number of items, or include components valuable in other projects.

The Pebble SDK itself is composed of separate modules — `Window`, `Layer`,
`AppMessage` — each with an implementation separate from the rest and a clearly
defined interface that rarely changes.

Advantages of a modular approach:

- App `Window`s can be kept separate and are easier to work on.
- A clearly defined interface between components ensures internal changes do not
  affect other modules.
- Modules can be re-used in other projects, or made into sharable libraries.
- Inter-component variable dependencies do not occur, which otherwise cause
  problems if their type or size changes.
- Sub-component complexity is hidden in each module.
- Simpler individual files promote maintainability.
- Modules can be more easily tested.

## A basic project

```sh
$ pebble new-project modular-project
```

The default file structure:

```
modular-project/
  resources/
  src/
    modular-project.c
  package.json
  wscript
```

`modular-project.c` contains the entire app — `main()`, `init()` and `deinit()`,
plus a `Window` and a child `TextLayer`. For most projects this is perfectly
adequate. When the `.c` file grows **to several hundred lines** and incorporates
several sub-components with many points of interaction through shared variables,
the complexity reaches the point where new techniques are needed.

## Creating a module

A 'module' is a C header and source file **pair**: a `.h` describing the module's
interface, and a `.c` containing the logic. The header holds the include guard and
all function prototypes the module makes available to others.

Making a sub-component into a module removes the need for messy global variables
and defines a clear interface between components. Modules live in a `modules`
directory inside the project's `src`, keeping them separate from other components:

```
modular-project/
  resources/
  src/
    modules/
      data.h
      data.c
    modular-project.c
  package.json
  wscript
```

The example module manages a dynamically allocated array of integers, with an
interface for setting and getting values. The array itself is **private to the
module** thanks to the `static` keyword — other components call the getters and
setters with the correct parameters per the module's interface, without worrying
about implementation details.

`src/modules/data.h`

```c
#pragma once         // Prevent errors by being included multiple times

#include <pebble.h>  // Pebble SDK symbols

void data_init(int array_length);

void data_deinit();

void data_set_array_value(int index, int new_value);

int data_get_array_value(int index);
```

`src/modules/data.c`

```c
#include "data.h"

static int* s_array;

void data_init(int array_length) {
  if(!s_array) {
    s_array = (int*)malloc(array_length * sizeof(int));
  }
}

void data_deinit() {
  if(s_array) {
    free(s_array);
    s_array = NULL;
  }
}

void data_set_array_value(int index, int new_value) {
  s_array[index] = new_value;
}

int data_get_array_value(int index) {
  return s_array[index];
}
```

## Keep multiple windows separate

The `Window Stack` lifecycle makes keeping each `Window` separate easy: each has a
`.load` and `.unload` handler that should create and destroy its UI components and
other data.

Move the first `Window`'s code out of `src/modular-project.c` into `src/windows/`
as 'main_window':

`src/windows/main_window.h`

```c
#pragma once

#include <pebble.h>

void main_window_push();
```

`src/windows/main_window.c`

```c
#include "main_window.h"

static Window *s_window;

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
}

static void window_unload(Window *window) {
  window_destroy(s_window);
}

void main_window_push() {
  if(!s_window) {
    s_window = window_create();
    window_set_window_handlers(s_window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload,
    });
  }
  window_stack_push(s_window, true);
}
```

Note the shape of the interface: a single `*_push()` function per window module.
The window's `Window*` and handlers stay `static` inside the `.c`, so callers
never touch them.

## Keeping main clear

After moving the `Window` code out, rename the main `.c` to `main.c` to reflect its
contents — it now shows a high-level overview of the app as a whole. `#include`
the required modules and windows, and initialize/deinitialize them:

`src/main.c`

```c
#include <pebble.h>

#include "modules/data.h"
#include "windows/main_window.h"

static void init() {
  const int array_size = 16;
  data_init(array_size);

  main_window_push();
}

static void deinit() {
  data_deinit();
}

int main() {
  init();
  app_event_loop();
  deinit();
}
```

Resulting structure:

```
modular-project/
  resources/
  src/
    modules/
      data.h
      data.c
    windows/
      main_window.h
      main_window.c
    main.c
  package.json
  wscript
```

With this structure the maintainability of the project does not suffer as it
grows. A useful module can even be shared as a library, which is preferable to
pasting chunks of code with messy dependencies elsewhere.

## Example

[Modular App Example](https://github.com/pebble-examples/modular-app-example/)