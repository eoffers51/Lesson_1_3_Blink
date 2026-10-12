# 🚨 Lesson 1.3 — Police Siren Lights (ESP32-S3)

[![CI](https://github.com/eoffers51/Lesson_1_3_Blink/actions/workflows/ci.yml/badge.svg)](https://github.com/eoffers51/Lesson_1_3_Blink/actions/workflows/ci.yml)
![Board](https://img.shields.io/badge/board-ESP32--S3--DevKitC--1-E7352C)
![Framework](https://img.shields.io/badge/ESP--IDF-v6.0.2-blue)
![Tests](https://img.shields.io/badge/host%20tests-Unity-brightgreen)
[![Simulate on Wokwi](https://img.shields.io/badge/simulate-Wokwi-6c4cbb)](https://wokwi.com/projects/477675527233048577)

Beetroot Academy homework, lesson 1.3 ([`TASK.md`](TASK.md)). An ESP32-S3 flashes red and blue LEDs like police siren lights. The **Boot** button switches between five modes.

## ✅ Requirements

| Requirement | Done | How |
|---|:---:|---|
| Two GPIO pins, LEDs through resistors | ✅ | Red on **GPIO9**, blue on **GPIO8**, 220–330 Ω each |
| Red and blue blink alternately, in a loop | ✅ | **Classic** mode, `while (1)` in `app_main` |
| ⭐ More complex pattern | ✅ | **Double flash** mode |
| ⭐ Third LED | ✅ | External **green** LED + onboard **RGB** LED |
| ⭐ Vary the speed | ✅ | **Fast** mode |
| ⭐ Two LEDs on one GPIO | ✅ | **One pin** mode: blue and green alternate on GPIO8 alone |

## 💡 Modes (press Boot to go to the next one)

| # | Mode | What you see |
|---|------|--------------|
| 1 | **Classic** | Red and blue take turns, 0.4 s each |
| 2 | **Double flash** | Red flashes twice, then blue flashes twice |
| 3 | **Fast** | Red and blue take turns, 0.1 s each |
| 4 | **RGB** | Onboard RGB LED goes red → green → blue |
| 5 | **One pin** | Blue and green take turns, both driven by GPIO8 |

## 🔁 State machine

Each Boot press moves to the next mode (`siren_next_mode`), always starting from that mode's first step:

```mermaid
stateDiagram-v2
    state "Classic" as Classic
    state "Double flash" as DoubleFlash
    state "Fast" as Fast
    state "RGB" as RGB
    state "One pin" as OnePin

    [*] --> Classic: power on
    Classic --> DoubleFlash: Boot pressed
    DoubleFlash --> Fast: Boot pressed
    Fast --> RGB: Boot pressed
    RGB --> OnePin: Boot pressed
    OnePin --> Classic: Boot pressed
```

Inside a mode, `siren_update` moves to the next step when its time is up and wraps back to the first one. For example, **Classic**:

```mermaid
stateDiagram-v2
    state "Red on" as Red
    state "Blue on" as Blue

    [*] --> Red
    Red --> Blue: after 400 ms
    Blue --> Red: after 400 ms
```

## 🔌 Wiring

![Wiring diagram](docs/wiring.svg)

<details>
<summary>Text version</summary>

```text
GPIO9 ──[R]──►|── red   ── GND      GPIO9 HIGH  → red on
GPIO8 ──[R]──►|── blue  ── GND      GPIO8 HIGH  → blue on
3V3   ──[R]──►|── green ── GPIO8    GPIO8 LOW   → green on
GPIO8 as input                      → blue and green both off
GPIO48 = onboard WS2812 RGB LED,  GPIO0 = Boot button (active LOW)
```

</details>

**Two LEDs on one pin:** a GPIO has three states — HIGH (blue on), LOW (green on) and input (both off: blue + green need ~5 V in series, more than 3.3 V). Blue and green can never be on together; `lights_set()` rejects that.

**Onboard LED:** it is an addressable WS2812, so `gpio_set_level` can't light it. It is driven through the RMT peripheral with Espressif's `led_strip` component.

## 🖥️ Simulate in Wokwi

**[▶ Open the Wokwi project](https://wokwi.com/projects/477675527233048577)** and press the green ▶ button (the first build can take a minute or two on Wokwi's free servers). Click **BOOT** to switch modes.

The Wokwi project runs this repo's own C code (`siren`, `lights`, `button`, `main.c`). Wokwi compiles ESP-IDF code with the Arduino ESP32 core, so [`wokwi/`](wokwi) has two small Wokwi-only files: `sketch.ino` starts our `app_main`, and `led.c` drives the RGB LED without the `led_strip` component. The `ESP_LOGI` mode messages don't appear in Wokwi's serial monitor.

- **VS Code:** install the *Wokwi Simulator* extension, run `idf.py build`, then *Wokwi: Start Simulator* — this runs the real firmware ([`wokwi.toml`](wokwi.toml) points to the build, [`diagram.json`](diagram.json) holds the circuit).

Wokwi's DevKitC-1 has its RGB LED on GPIO38 and no clickable Boot button, so the diagram adds a NeoPixel on **GPIO48** and a button on **GPIO0** to match this board. Wokwi also smooths LED brightness, so in **Double flash** the two quick 60 ms flashes blend into one and **Fast** looks like both LEDs glowing; on the real board you see the real blinking.

## 🏗️ Code structure

```text
main/main.c            setup + while (1) loop: read button, update siren, show step (20 ms tick)
components/siren/      🧠 modes as step tables + timing — pure C, no hardware
components/lights/     💡 red/blue/green LEDs (GPIO, tri-state shared pin)
components/led/        🌈 onboard WS2812 (led_strip / RMT)
components/button/     🔘 Boot button with debouncing
host_test/             🧪 Unity tests that run on the PC
```

- Each mode is a `const` table of steps, so a new mode is one table + one row — no `if`/`switch` changes:
  ```c
  static const siren_step_t CLASSIC[] = {
      {.red = true, .duration_ms = 400},
      {.blue = true, .duration_ms = 400},
  };
  ```
- The loop runs every 20 ms instead of sleeping a whole step, so the button responds instantly in every mode.
- The button is debounced (a press counts once, after stable readings).

## 🧪 Tests and CI

16 host tests (Unity) check the siren logic and button debouncing on the PC, including that **no mode ever turns on blue and green together**.

```powershell
.\host_test\run_tests.ps1
```

GitHub Actions runs the tests and builds the firmware on every push; `v*` tags publish a release with the binaries.

## 🔧 Build and flash

```bash
idf.py set-target esp32s3     # first time only
idf.py build
idf.py -p COM5 flash monitor  # exit monitor with Ctrl+]
```
