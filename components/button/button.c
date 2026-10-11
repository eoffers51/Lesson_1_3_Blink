#include "button.h"
#include "driver/gpio.h"

// A new reading must stay the same this many checks in a row before we believe it.
// Contacts "bounce" (flicker) for a few ms when pressed or released; this ignores that.
#define BUTTON_STABLE_READS 2

static int s_gpio;
static bool s_down;      // the button's state after debouncing
static bool s_last_raw;  // the last raw reading
static int s_same_reads; // how many checks in a row gave the same raw reading

// Read the pin: pressed pulls it to GND, so 0 means down
static bool read_down(void)
{
    return gpio_get_level(s_gpio) == 0;
}

esp_err_t button_init(int gpio)
{
    s_gpio = gpio;

    // Input with pull-up: reads 1 when released, 0 when pressed (pulled to GND)
    gpio_config_t config = {
        .pin_bit_mask = 1ULL << gpio,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    esp_err_t err = gpio_config(&config);
    if (err != ESP_OK) {
        return err;
    }

    // Start from the real state, so a button held at startup is not a "press"
    s_down = read_down();
    s_last_raw = s_down;
    s_same_reads = BUTTON_STABLE_READS;
    return ESP_OK;
}

bool button_pressed(void)
{
    // Count how long the raw reading has stayed the same
    bool raw = read_down();
    if (raw != s_last_raw) {
        s_last_raw = raw;
        s_same_reads = 0;
    } else if (s_same_reads < BUTTON_STABLE_READS) {
        s_same_reads++;
    }

    // Not stable yet, or nothing changed: no press
    if (s_same_reads < BUTTON_STABLE_READS || raw == s_down) {
        return false;
    }

    // Stable new state: report a press only when it went up -> down
    s_down = raw;
    return s_down;
}
