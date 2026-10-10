#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "led_strip.h"

// The onboard RGB LED (WS2812) is connected to GPIO48 on this board
#define LED_GPIO 48
// How long the LED stays on, then off (milliseconds)
#define BLINK_PERIOD_MS 500

static const char *TAG = "blink";

void app_main(void)
{
    // 1. Describe the LED: one WS2812 LED on LED_GPIO
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_GPIO,
        .max_leds = 1,
        .led_model = LED_MODEL_WS2812,
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
    };

    // 2. The ESP32 talks to the LED using its RMT peripheral (precise timed pulses)
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // 10 MHz
    };

    // 3. Create the LED driver
    led_strip_handle_t led;
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led));
    led_strip_clear(led); // start with the LED off

    bool led_on = false;
    int color = 0; // 0 = red, 1 = green, 2 = blue

    // 4. Blink forever, changing color each time the LED turns on
    while (1) {
        led_on = !led_on;

        if (led_on) {
            // Set LED #0 to one color (red, green, blue: 0-255) and send it to the LED
            if (color == 0) {
                led_strip_set_pixel(led, 0, 32, 0, 0);
                ESP_LOGI(TAG, "LED ON: RED");
            } else if (color == 1) {
                led_strip_set_pixel(led, 0, 0, 32, 0);
                ESP_LOGI(TAG, "LED ON: GREEN");
            } else {
                led_strip_set_pixel(led, 0, 0, 0, 32);
                ESP_LOGI(TAG, "LED ON: BLUE");
            }
            led_strip_refresh(led);

            // Move to the next color (after blue, go back to red)
            color = (color + 1) % 3;
        } else {
            // Turn the LED off
            led_strip_clear(led);
            ESP_LOGI(TAG, "LED OFF");
        }

        // Wait before toggling again (lets other tasks run too)
        vTaskDelay(pdMS_TO_TICKS(BLINK_PERIOD_MS));
    }
}
