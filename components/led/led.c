#include "led.h"
#include "led_strip.h"

// The ESP32 talks to the WS2812 using its RMT peripheral (precise timed pulses).
// 10 MHz gives 0.1 us steps - fine enough to build WS2812 pulses.
#define LED_RMT_RESOLUTION_HZ (10 * 1000 * 1000)

// Driver handle, only visible inside this file
static led_strip_handle_t s_strip;

esp_err_t led_init(int gpio)
{
    // Describe the LED: one WS2812 LED that expects colors in GRB order
    led_strip_config_t strip_config = {
        .strip_gpio_num = gpio,
        .max_leds = 1,
        .led_model = LED_MODEL_WS2812,
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
    };
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = LED_RMT_RESOLUTION_HZ,
    };

    // Create the driver, then start with the LED off
    esp_err_t err = led_strip_new_rmt_device(&strip_config, &rmt_config, &s_strip);
    if (err != ESP_OK) {
        return err;
    }
    return led_strip_clear(s_strip);
}

esp_err_t led_set_rgb(uint8_t red, uint8_t green, uint8_t blue)
{
    // Store the color for LED #0, then send it to the LED
    esp_err_t err = led_strip_set_pixel(s_strip, 0, red, green, blue);
    if (err != ESP_OK) {
        return err;
    }
    return led_strip_refresh(s_strip);
}
