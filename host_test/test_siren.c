#include <string.h>
#include "unity.h"
#include "siren.h"

// The first mode (Classic): red 400 ms, then blue 400 ms
#define CLASSIC_STEP_MS 400

static void test_starts_in_classic_with_red_on(void)
{
    siren_t siren;
    siren_init(&siren);

    TEST_ASSERT_EQUAL_STRING("Classic", siren_mode_name(&siren));
    TEST_ASSERT_TRUE(siren_current_step(&siren)->red);
    TEST_ASSERT_FALSE(siren_current_step(&siren)->blue);
}

static void test_step_stays_until_its_time_is_up(void)
{
    siren_t siren;
    siren_init(&siren);

    TEST_ASSERT_FALSE(siren_update(&siren, CLASSIC_STEP_MS - 1));
    TEST_ASSERT_TRUE(siren_current_step(&siren)->red);
}

static void test_moves_to_next_step_when_time_is_up(void)
{
    siren_t siren;
    siren_init(&siren);

    TEST_ASSERT_TRUE(siren_update(&siren, CLASSIC_STEP_MS));
    TEST_ASSERT_TRUE(siren_current_step(&siren)->blue);
    TEST_ASSERT_FALSE(siren_current_step(&siren)->red);
}

static void test_small_time_steps_add_up(void)
{
    siren_t siren;
    siren_init(&siren);

    // 20 ms at a time, like the main loop: the step changes on the 20th call (400 ms)
    for (int i = 1; i < 20; i++) {
        TEST_ASSERT_FALSE(siren_update(&siren, 20));
    }
    TEST_ASSERT_TRUE(siren_update(&siren, 20));
    TEST_ASSERT_TRUE(siren_current_step(&siren)->blue);
}

static void test_goes_back_to_first_step_after_last(void)
{
    siren_t siren;
    siren_init(&siren);

    siren_update(&siren, CLASSIC_STEP_MS); // red -> blue
    siren_update(&siren, CLASSIC_STEP_MS); // blue -> red again
    TEST_ASSERT_EQUAL(0, siren.step_index);
    TEST_ASSERT_TRUE(siren_current_step(&siren)->red);
}

static void test_modes_cycle_in_order_and_wrap(void)
{
    const char *expected[] = {"Double flash", "Fast", "RGB", "One pin", "Classic"};
    siren_t siren;
    siren_init(&siren);

    for (size_t i = 0; i < sizeof(expected) / sizeof(expected[0]); i++) {
        siren_next_mode(&siren);
        TEST_ASSERT_EQUAL_STRING(expected[i], siren_mode_name(&siren));
    }
}

static void test_new_mode_starts_from_first_step_with_fresh_timer(void)
{
    siren_t siren;
    siren_init(&siren);

    // Go to the second step and spend part of its time there
    siren_update(&siren, CLASSIC_STEP_MS);
    siren_update(&siren, 50);

    siren_next_mode(&siren);
    TEST_ASSERT_EQUAL(0, siren.step_index);
    TEST_ASSERT_EQUAL(0, siren.step_time_ms);
}

static void test_rgb_mode_uses_only_the_onboard_led(void)
{
    siren_t siren;
    siren_init(&siren);
    while (strcmp(siren_mode_name(&siren), "RGB") != 0) {
        siren_next_mode(&siren);
    }

    // Walk through every step of the mode once
    do {
        const siren_step_t *step = siren_current_step(&siren);
        TEST_ASSERT_FALSE(step->red || step->blue || step->green);
        TEST_ASSERT_TRUE(step->onboard.red || step->onboard.green || step->onboard.blue);
        siren_update(&siren, step->duration_ms);
    } while (siren.step_index != 0);
}

// Checks every step of every mode: blue and green share a pin, so they must never be on together
static void test_no_step_turns_on_blue_and_green_together(void)
{
    siren_t siren;
    siren_init(&siren);

    do {
        do {
            const siren_step_t *step = siren_current_step(&siren);
            TEST_ASSERT_FALSE_MESSAGE(step->blue && step->green, siren_mode_name(&siren));
            TEST_ASSERT_TRUE_MESSAGE(step->duration_ms > 0, siren_mode_name(&siren));
            siren_update(&siren, step->duration_ms);
        } while (siren.step_index != 0);
        siren_next_mode(&siren);
    } while (siren.mode_index != 0);
}

void run_siren_tests(void)
{
    UnitySetTestFile(__FILE__); // so failures point at this file
    RUN_TEST(test_starts_in_classic_with_red_on);
    RUN_TEST(test_step_stays_until_its_time_is_up);
    RUN_TEST(test_moves_to_next_step_when_time_is_up);
    RUN_TEST(test_small_time_steps_add_up);
    RUN_TEST(test_goes_back_to_first_step_after_last);
    RUN_TEST(test_modes_cycle_in_order_and_wrap);
    RUN_TEST(test_new_mode_starts_from_first_step_with_fresh_timer);
    RUN_TEST(test_rgb_mode_uses_only_the_onboard_led);
    RUN_TEST(test_no_step_turns_on_blue_and_green_together);
}
