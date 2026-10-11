# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

ESP-IDF v6.0.2 project for an **ESP32-S3-DevKitC-1** (Beetroot Academy lesson 1.3: blink the onboard LED).

## Build / flash

`idf.py` is not on PATH. Activate the IDF env in the same PowerShell call as every `idf.py` command (shell state doesn't persist between calls):

```powershell
. C:\Espressif\tools\Microsoft.v6.0.2.PowerShell_profile.ps1 *> $null; idf.py build
```

- Activation is also what puts the Espressif `cmake`/`ninja` ahead of the STM32CubeCLT ones on PATH — never build without it.
- `sdkconfig` is gitignored and may not exist. If it's missing, run `idf.py set-target esp32s3` before the first build.
- Flash with `idf.py -p COMx flash` (or use `/build-flash`). Never run `idf.py monitor` — it's interactive and will hang; ask the user to run it.
- Standard ESP-IDF layout: `main/` holds only the app (`main.c`); modules are components in `components/<name>/` with their own `CMakeLists.txt`, public headers in `include/`, sources in `SRCS`.
  - `components/led` — hardware layer wrapping `espressif/led_strip` (declared in `components/led/idf_component.yml`, `PRIV_REQUIRES led_strip`).
  - The app (police siren, `TASK.md`) uses `components/lights` (GPIO LEDs; blue/green share one tri-state pin), `components/led` (onboard WS2812, for the RGB mode), `components/button` (polled Boot button, debounced press detection) and `components/siren` (pure logic, no `REQUIRES`: private mode/step tables plus a `siren_t` state that `main.c` drives with `siren_next_mode` / `siren_update` — keep sequencing logic there, not in `main.c`). Modes cycle on each Boot press: Classic → Double flash → Fast → RGB → One pin (blue/green alternate on GPIO8 alone).
  - Registry deps go in the `idf_component.yml` of the component that uses them (fetched into `managed_components/`, gitignored). `main` lists its components in `PRIV_REQUIRES`; the top-level `CMakeLists.txt` uses `MINIMAL_BUILD`, so anything not required is not built.

## Host tests

`host_test/` is a standalone CMake project (not part of the IDF build) that runs Unity tests for `siren` and `button` on the PC. `button` gets a fake `driver/gpio.h` / `esp_err.h` from `host_test/fakes/`; the test sets the pin level with `fake_gpio_set_level`. Run them with:

```powershell
.\host_test\run_tests.ps1        # -Clean to rebuild from scratch
```

The script builds with the MinGW gcc in `C:\CTOOLS\mingw32\bin`, which it puts first on PATH; otherwise the IDF env's PATH makes gcc pick up the wrong assembler. It works with or without the IDF env active, takes `IDF_PATH` from the env (default `C:\esp\v6.0.2\esp-idf`) and exits non-zero if any test fails.

Add new tests as `static void test_...` functions plus a `RUN_TEST` line in that file's `run_*_tests()`; a new test file also needs adding to `host_test/CMakeLists.txt` and `test_main.c`.

## Hardware

- The onboard RGB LED is an addressable **WS2812**, not a plain GPIO LED — `gpio_set_level` won't light it. Use the `espressif/led_strip` component (RMT backend, 1 LED).
- The user's board is a DevKitC-1-style **clone (CH343 USB-UART on COM5)**. Its RGB LED is on **GPIO48** — confirmed on hardware; GPIO38 does nothing despite the board looking like v1.1. Keep the pin in a single `#define`.
- External LEDs: **red on GPIO9**, **blue and green share GPIO8** — blue GPIO8→LED→GND (HIGH = on), green 3V3→LED→GPIO8 (LOW = on), pin as input = both off. Never drive blue and green together. Mode button = onboard **Boot button, GPIO0** (active LOW).
- A small green LED blinks in time with log output (most likely the CH343's UART TX activity light) — it is not the RGB LED, don't mistake it for LED output.

## Code style

- This is a beginner course: keep code minimal and readable, prefer a simple `while (1)` loop with `vTaskDelay(pdMS_TO_TICKS(...))` in `app_main`, and add a short comment explaining each step.
- Use `ESP_LOGI` with a `TAG` for output rather than `printf`.
- Formatting follows `.clang-format` (ESP-IDF style: 4 spaces, function braces on their own line); a hook applies it after edits.
