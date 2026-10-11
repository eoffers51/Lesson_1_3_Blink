# LED module: wraps the WS2812 led_strip driver.
# led_strip is private - users of this component only see led.h.
idf_component_register(SRCS "led.c"
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES led_strip)
