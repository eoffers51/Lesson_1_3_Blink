#pragma once

// Button module: a push button wired GPIO -> button -> GND (like the Boot button).

#include <stdbool.h>
#include "esp_err.h"

// Set up the button pin as an input with a pull-up resistor
esp_err_t button_init(int gpio);

// Call this regularly (every ~20 ms). Returns true once each time the button is pressed.
// Contact bounce is filtered out, so one press is never counted twice.
bool button_pressed(void);
