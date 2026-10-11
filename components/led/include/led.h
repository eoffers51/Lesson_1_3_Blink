#pragma once

// LED module: the only part of the program that knows the LED is a WS2812.
// The rest of the code just asks for a color.

#include <stdint.h>
#include "esp_err.h"

// Set up the LED on the given GPIO pin and turn it off
esp_err_t led_init(int gpio);

// Show a color: red, green, blue brightness from 0 to 255 (0, 0, 0 = off)
esp_err_t led_set_rgb(uint8_t red, uint8_t green, uint8_t blue);
