#include "driver/gpio.h"

static int s_level = 1; // released (pull-up) until a test says otherwise

esp_err_t gpio_config(const gpio_config_t *config)
{
    (void)config;
    return ESP_OK;
}

int gpio_get_level(int gpio)
{
    (void)gpio;
    return s_level;
}

void fake_gpio_set_level(int level)
{
    s_level = level;
}
