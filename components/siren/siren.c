#include <assert.h>
#include "siren.h"

// Number of elements in an array
#define COUNT_OF(array) (sizeof(array) / sizeof((array)[0]))
// Brightness of the onboard RGB LED (0-255). It is very bright, so keep it low.
#define ONBOARD_BRIGHTNESS 32

// A blinking mode: a list of steps that repeats forever
typedef struct {
    const char *name;          // text for the log
    const siren_step_t *steps; // the steps, in order
    size_t step_count;         // how many steps there are
} siren_mode_t;

// Mode 1: classic - red and blue take turns
static const siren_step_t CLASSIC[] = {
    {.red = true, .duration_ms = 400},
    {.blue = true, .duration_ms = 400},
};

// Mode 2: double flash - red flashes twice, then blue flashes twice
static const siren_step_t DOUBLE_FLASH[] = {
    {.red = true, .duration_ms = 60},  {.duration_ms = 60}, {.red = true, .duration_ms = 60},  {.duration_ms = 180},
    {.blue = true, .duration_ms = 60}, {.duration_ms = 60}, {.blue = true, .duration_ms = 60}, {.duration_ms = 180},
};

// Mode 3: fast - same as classic, but much quicker
static const siren_step_t FAST[] = {
    {.red = true, .duration_ms = 100},
    {.blue = true, .duration_ms = 100},
};

// Mode 4: RGB - external LEDs off, the onboard LED cycles red -> green -> blue
static const siren_step_t RGB[] = {
    {.onboard = {.red = ONBOARD_BRIGHTNESS}, .duration_ms = 500},
    {.onboard = {.green = ONBOARD_BRIGHTNESS}, .duration_ms = 500},
    {.onboard = {.blue = ONBOARD_BRIGHTNESS}, .duration_ms = 500},
};

// Mode 5: one pin - blue and green take turns, both driven by the same GPIO
static const siren_step_t ONE_PIN[] = {
    {.blue = true, .duration_ms = 400},
    {.green = true, .duration_ms = 400},
};

// All modes, in the order the button switches through them.
// To add a mode, write a new step list above and add a row here.
static const siren_mode_t MODES[] = {
    {"Classic", CLASSIC, COUNT_OF(CLASSIC)}, {"Double flash", DOUBLE_FLASH, COUNT_OF(DOUBLE_FLASH)},
    {"Fast", FAST, COUNT_OF(FAST)},          {"RGB", RGB, COUNT_OF(RGB)},
    {"One pin", ONE_PIN, COUNT_OF(ONE_PIN)},
};

// The running mode (checks the index is valid while developing)
static const siren_mode_t *current_mode(const siren_t *siren)
{
    assert(siren->mode_index < COUNT_OF(MODES));
    return &MODES[siren->mode_index];
}

void siren_init(siren_t *siren)
{
    siren->mode_index = 0;
    siren->step_index = 0;
    siren->step_time_ms = 0;
}

void siren_next_mode(siren_t *siren)
{
    siren->mode_index = (siren->mode_index + 1) % COUNT_OF(MODES);
    siren->step_index = 0;
    siren->step_time_ms = 0;
}

bool siren_update(siren_t *siren, uint32_t elapsed_ms)
{
    // Still time left in this step: nothing changes
    siren->step_time_ms += elapsed_ms;
    if (siren->step_time_ms < siren_current_step(siren)->duration_ms) {
        return false;
    }

    // Step is over: go to the next one (back to the first after the last)
    siren->step_index = (siren->step_index + 1) % current_mode(siren)->step_count;
    siren->step_time_ms = 0;
    return true;
}

const siren_step_t *siren_current_step(const siren_t *siren)
{
    const siren_mode_t *mode = current_mode(siren);
    assert(siren->step_index < mode->step_count);
    return &mode->steps[siren->step_index];
}

const char *siren_mode_name(const siren_t *siren)
{
    return current_mode(siren)->name;
}
