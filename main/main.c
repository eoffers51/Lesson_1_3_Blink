#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "lights.h"
#include "led.h"
#include "button.h"
#include "siren.h"

// External LEDs (each through its own resistor to GND)
#define RED_LED_GPIO 9
#define BLUE_GREEN_LED_GPIO 8 // blue (HIGH = on) and green (LOW = on) share this pin
// The onboard RGB LED (WS2812)
#define ONBOARD_LED_GPIO 48
// The Boot button on the board
#define BUTTON_GPIO 0
// How often the loop checks the button and the timer (milliseconds)
#define LOOP_PERIOD_MS 20

static const char *TAG = "siren";

// Show one step on the external LEDs and the onboard LED
static esp_err_t show_step(const siren_step_t *step)
{
    esp_err_t err = lights_set(step->red, step->blue, step->green);
    if (err != ESP_OK) {
        return err;
    }
    return led_set_rgb(step->onboard.red, step->onboard.green, step->onboard.blue);
}

void app_main(void)
{
    // 1. Set up the LEDs and the button (stops with a clear error message if it fails)
    ESP_ERROR_CHECK(lights_init(RED_LED_GPIO, BLUE_GREEN_LED_GPIO));
    ESP_ERROR_CHECK(led_init(ONBOARD_LED_GPIO));
    ESP_ERROR_CHECK(button_init(BUTTON_GPIO));

    // 2. Start with the first mode
    siren_t siren;
    siren_init(&siren);
    ESP_LOGI(TAG, "Mode: %s (press Boot to change)", siren_mode_name(&siren));
    ESP_ERROR_CHECK(show_step(siren_current_step(&siren)));

    // 3. Run forever
    while (1) {
        // Each press of Boot switches to the next mode
        if (button_pressed()) {
            siren_next_mode(&siren);
            ESP_LOGI(TAG, "Mode: %s", siren_mode_name(&siren));
            ESP_ERROR_CHECK(show_step(siren_current_step(&siren)));
        }

        // Wait a little (lets other tasks run too)
        vTaskDelay(pdMS_TO_TICKS(LOOP_PERIOD_MS));

        // Tell the siren time has passed; show the new step if it moved on
        if (siren_update(&siren, LOOP_PERIOD_MS)) {
            ESP_ERROR_CHECK(show_step(siren_current_step(&siren)));
        }
    }
}
