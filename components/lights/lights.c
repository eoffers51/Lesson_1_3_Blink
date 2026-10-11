#include "lights.h"
#include "driver/gpio.h"

// Pin numbers, only visible inside this file
static int s_red_gpio;
static int s_blue_green_gpio;

// Drive the shared pin HIGH or LOW (set the level first, so it never glitches)
static esp_err_t drive_shared_pin(int level)
{
    esp_err_t err = gpio_set_level(s_blue_green_gpio, level);
    if (err != ESP_OK) {
        return err;
    }
    return gpio_set_direction(s_blue_green_gpio, GPIO_MODE_OUTPUT);
}

esp_err_t lights_init(int red_gpio, int blue_green_gpio)
{
    s_red_gpio = red_gpio;
    s_blue_green_gpio = blue_green_gpio;

    // Red: a plain output
    gpio_config_t red_config = {
        .pin_bit_mask = 1ULL << red_gpio,
        .mode = GPIO_MODE_OUTPUT,
    };
    esp_err_t err = gpio_config(&red_config);
    if (err != ESP_OK) {
        return err;
    }

    // Blue/green: starts as an input with no pull resistors (not driving = both off)
    gpio_config_t shared_config = {
        .pin_bit_mask = 1ULL << blue_green_gpio,
        .mode = GPIO_MODE_INPUT,
    };
    err = gpio_config(&shared_config);
    if (err != ESP_OK) {
        return err;
    }

    // Start with all LEDs off
    return lights_set(false, false, false);
}

esp_err_t lights_set(bool red, bool blue, bool green)
{
    // Blue and green share one pin: it can't be HIGH and LOW at the same time
    if (blue && green) {
        return ESP_ERR_INVALID_ARG;
    }

    // Red: HIGH (1) = on, LOW (0) = off
    esp_err_t err = gpio_set_level(s_red_gpio, red);
    if (err != ESP_OK) {
        return err;
    }

    // Shared pin: HIGH = blue on, LOW = green on, input (not driving) = both off
    if (blue) {
        return drive_shared_pin(1);
    }
    if (green) {
        return drive_shared_pin(0);
    }
    return gpio_set_direction(s_blue_green_gpio, GPIO_MODE_INPUT);
}
