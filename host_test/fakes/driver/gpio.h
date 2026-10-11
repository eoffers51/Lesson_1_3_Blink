#pragma once

// Fake of ESP-IDF's GPIO driver for host tests.
// There is no real pin: the test decides what the pin reads with fake_gpio_set_level().

#include <stdint.h>
#include "esp_err.h"

typedef enum {
    GPIO_MODE_INPUT = 1,
} gpio_mode_t;

typedef enum {
    GPIO_PULLUP_DISABLE = 0,
    GPIO_PULLUP_ENABLE = 1,
} gpio_pullup_t;

typedef struct {
    uint64_t pin_bit_mask;
    gpio_mode_t mode;
    gpio_pullup_t pull_up_en;
} gpio_config_t;

esp_err_t gpio_config(const gpio_config_t *config);
int gpio_get_level(int gpio);

// Test control: the level every pin reads from now on (1 = released, 0 = pressed)
void fake_gpio_set_level(int level);
