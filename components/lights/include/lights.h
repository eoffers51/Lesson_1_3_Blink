#pragma once

// Lights module: the external red, blue and green LEDs.
//
// Wiring:
//   red:   red_gpio        -> resistor -> red LED   -> GND   (pin HIGH = on)
//   blue:  blue_green_gpio -> resistor -> blue LED  -> GND   (pin HIGH = on)
//   green: 3V3 -> resistor -> green LED -> blue_green_gpio   (pin LOW  = on)
//
// Blue and green share ONE pin, so only one of them can be on at a time.
// To turn both off, the shared pin is switched to input (it stops driving):
// the only path left is 3V3 -> green -> blue -> GND, which needs more than
// 3.3 V to light the LEDs, so both stay dark.

#include <stdbool.h>
#include "esp_err.h"

// Set up the LED pins and turn all LEDs off
esp_err_t lights_init(int red_gpio, int blue_green_gpio);

// Turn each LED on (true) or off (false).
// Blue and green can't both be on: that returns ESP_ERR_INVALID_ARG.
esp_err_t lights_set(bool red, bool blue, bool green);
