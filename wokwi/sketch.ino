// Police siren lights - Beetroot Academy lesson 1.3 (ESP32-S3, ESP-IDF, C)
// Source: https://github.com/eoffers51/Lesson_1_3_Blink
//
// Wokwi compiles this project with the Arduino ESP32 core, which has its own app_main().
// So main.c here renames ours to siren_app_main() and setup() starts it.
// Everything else is the repo's ESP-IDF code (led.c is a Wokwi-only version).
// Press the BOOT button to switch modes.

extern "C" void siren_app_main(void);

void setup()
{
    siren_app_main(); // never returns: the siren loop runs forever
}

void loop() {}
