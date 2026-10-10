# Lesson 1.3 — Blink (ESP32-S3)

Beetroot Academy embedded course, lesson 1.3: make the onboard LED of an ESP32-S3 board blink.

The program cycles the board's RGB LED through **red → off → green → off → blue → off**, changing every 0.5 seconds, and prints what it is doing to the serial log.

## Hardware

| Item | Details |
|------|---------|
| Board | ESP32-S3-DevKitC-1 compatible board (USB-UART chip: CH343) |
| LED | Onboard **WS2812** addressable RGB LED |
| LED data pin | **GPIO48** |
| Connection | USB cable to the board's UART/USB port |

No extra wiring is needed — the LED is already on the board.

## Why this LED is not a "normal" blink

On many boards, blinking an LED just means switching a GPIO pin on and off (`gpio_set_level(pin, 1)` / `gpio_set_level(pin, 0)`).

The LED on this board is different. A **WS2812** is a "smart" LED: it contains its own tiny controller chip and red, green and blue LEDs. It is controlled with a **single data wire**, and it expects a stream of very precisely timed pulses:

- Each color is sent as **8 bits** (0–255 brightness), so one LED needs **24 bits** (green, red, blue).
- A `0` bit and a `1` bit are pulses of different lengths, measured in **fractions of a microsecond** (around 0.4 µs vs 0.8 µs high time).
- After receiving its 24 bits, the LED shows that color until it receives new data.

Turning the pin on and off by hand is far too slow and imprecise for this. So the program uses the ESP32's **RMT** peripheral (Remote Control Transceiver) — hardware that can generate exact pulse trains on its own — through Espressif's [`led_strip`](https://components.espressif.com/components/espressif/led_strip) component.

## How the code works

All the code is in [`main/main.c`](main/main.c). ESP-IDF calls `app_main()` when the chip starts.

1. **Describe the LED** — `led_strip_config_t` says: the LED is on GPIO48, there is 1 LED, it is a WS2812, and it expects colors in GRB order.
2. **Configure RMT** — `led_strip_rmt_config_t` sets the RMT clock to 10 MHz, which gives 0.1 µs timing steps — fine enough to build WS2812 pulses.
3. **Create the driver** — `led_strip_new_rmt_device()` sets up the hardware and returns a handle (`led`) used for all later calls. `ESP_ERROR_CHECK` stops the program with a clear message if this fails.
4. **Blink loop** — an endless `while (1)` loop:
   - Flips `led_on` between true and false.
   - When turning **on**: picks the current color, writes it with `led_strip_set_pixel(led, 0, red, green, blue)` and sends it to the LED with `led_strip_refresh(led)`. Then moves to the next color (`color = (color + 1) % 3` wraps blue back to red).
   - When turning **off**: `led_strip_clear(led)` sends "all colors 0".
   - Logs the state with `ESP_LOGI`.
   - Waits with `vTaskDelay(pdMS_TO_TICKS(500))`. This is a FreeRTOS delay: instead of busy-waiting, the CPU is free to run other tasks during the pause.

Settings you can change at the top of `main.c`:

| Define | Meaning |
|--------|---------|
| `LED_GPIO` | Pin connected to the LED data line |
| `BLINK_PERIOD_MS` | How long each on/off step lasts, in milliseconds |

Brightness is set by the color values (`32` out of `255`). WS2812 LEDs are very bright, so small values are easier on the eyes.

## Project structure

```
├── CMakeLists.txt          # Top-level ESP-IDF project file
├── dependencies.lock       # Exact version of led_strip used
└── main/
    ├── CMakeLists.txt      # Registers main.c as the "main" component
    ├── idf_component.yml   # Declares the espressif/led_strip dependency
    └── main.c              # The blink program
```

`.vscode/` and `.devcontainer/` hold editor and container setup for the ESP-IDF VS Code extension.

## Build and flash

Requires **ESP-IDF v6.0** (installed with the Espressif installer or the VS Code ESP-IDF extension).

From an ESP-IDF terminal in the project folder:

```bash
idf.py set-target esp32s3     # only the first time
idf.py build
idf.py -p <PORT> flash        # e.g. COM3 on Windows, /dev/ttyUSB0 on Linux
idf.py -p <PORT> monitor      # view the log, exit with Ctrl+]
```

The `led_strip` component is downloaded automatically during the first build.

Expected log output:

```
I (5280) blink: LED ON: BLUE
I (5780) blink: LED OFF
I (6280) blink: LED ON: RED
I (6780) blink: LED OFF
I (7280) blink: LED ON: GREEN
I (7780) blink: LED OFF
```

## Troubleshooting — what I learned

- **The RGB LED pin depends on the board.** Espressif's DevKitC-1 v1.0 uses GPIO48 and v1.1 uses GPIO38. My board looks like a v1.1 but is a compatible board, and its LED is on **GPIO48**. With GPIO38 the RGB LED stayed dark.
- **A blinking green light is not necessarily the RGB LED.** With the wrong pin I still saw a small green light blinking in time with the program. It was most likely the USB-UART chip's activity LED, which flashes every time the board sends a log message. The colour test (red → green → blue) made the difference obvious: only the real RGB LED changes color.
- **`gpio_set_level` cannot drive a WS2812.** It needs timed data pulses, which is why this project uses RMT and `led_strip`.
