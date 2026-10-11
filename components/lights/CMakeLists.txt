# Lights module: two plain LEDs on GPIO pins.
idf_component_register(SRCS "lights.c"
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES esp_driver_gpio)
