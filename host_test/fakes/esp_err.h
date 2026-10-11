#pragma once

// Fake of ESP-IDF's esp_err.h for host tests: just the error codes we use.

typedef int esp_err_t;

#define ESP_OK 0
#define ESP_FAIL -1
