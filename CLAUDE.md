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
- Component deps go in `main/idf_component.yml` (fetched into `managed_components/`, gitignored); sources must be listed in `main/CMakeLists.txt` `SRCS`.

## Hardware

- The onboard RGB LED is an addressable **WS2812**, not a plain GPIO LED — `gpio_set_level` won't light it. Use the `espressif/led_strip` component (RMT backend, 1 LED).
- The user's board is a DevKitC-1-style **clone (CH343 USB-UART on COM5)**. Its RGB LED is on **GPIO48** — confirmed on hardware; GPIO38 does nothing despite the board looking like v1.1. Keep the pin in a single `#define`.
- A small green LED blinks in time with log output (most likely the CH343's UART TX activity light) — it is not the RGB LED, don't mistake it for LED output.

## Code style

- This is a beginner course: keep code minimal and readable, prefer a simple `while (1)` loop with `vTaskDelay(pdMS_TO_TICKS(...))` in `app_main`, and add a short comment explaining each step.
- Use `ESP_LOGI` with a `TAG` for output rather than `printf`.
- Formatting follows `.clang-format` (ESP-IDF style: 4 spaces, function braces on their own line); a hook applies it after edits.
