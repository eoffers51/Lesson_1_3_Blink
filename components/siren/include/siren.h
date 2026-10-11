#pragma once

// Siren: the blinking modes and which step of them is showing right now.
// It does not touch any hardware, so it could run (and be tested) anywhere.
// The app tells it what happened (button pressed, time passed) and asks what to show.

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// A color for the onboard RGB LED, each part 0-255 (all 0 = off)
typedef struct {
    uint8_t red, green, blue;
} siren_rgb_t;

// One step of a pattern: which lights are on, and for how long.
// Fields left out of a step are 0 (off).
typedef struct {
    bool red;             // external red LED
    bool blue;            // external blue LED
    bool green;           // external green LED (shares blue's pin: never set both)
    siren_rgb_t onboard;  // onboard RGB LED color
    uint32_t duration_ms; // how long this step lasts
} siren_step_t;

// Where the siren is right now. Create one and pass it to every siren_* function.
typedef struct {
    size_t mode_index;     // which mode is running
    size_t step_index;     // which step of that mode is showing
    uint32_t step_time_ms; // how long the current step has been showing
} siren_t;

// Start at the first step of the first mode
void siren_init(siren_t *siren);

// Switch to the next mode (back to the first after the last), from its first step
void siren_next_mode(siren_t *siren);

// Tell the siren that time has passed. Returns true if it moved to a new step.
bool siren_update(siren_t *siren, uint32_t elapsed_ms);

// The step to show right now
const siren_step_t *siren_current_step(const siren_t *siren);

// Name of the running mode (for the log)
const char *siren_mode_name(const siren_t *siren);
