#include "unity.h"
#include "button.h"
#include "driver/gpio.h"

#define PRESSED 0  // the pin reads 0 when the button is down
#define RELEASED 1 // the pull-up makes it read 1 when the button is up

// Calls button_pressed() a number of times (like the main loop does) and counts the presses
static int count_presses(int polls)
{
    int presses = 0;
    for (int i = 0; i < polls; i++) {
        if (button_pressed()) {
            presses++;
        }
    }
    return presses;
}

static void test_no_press_while_released(void)
{
    fake_gpio_set_level(RELEASED);
    button_init(0);

    TEST_ASSERT_EQUAL(0, count_presses(10));
}

static void test_holding_the_button_counts_once(void)
{
    fake_gpio_set_level(RELEASED);
    button_init(0);

    fake_gpio_set_level(PRESSED);
    TEST_ASSERT_EQUAL(1, count_presses(10));
}

static void test_press_is_reported_only_once_stable(void)
{
    fake_gpio_set_level(RELEASED);
    button_init(0);

    // The new reading must repeat before it is believed, so the first two checks say no
    fake_gpio_set_level(PRESSED);
    TEST_ASSERT_FALSE(button_pressed());
    TEST_ASSERT_FALSE(button_pressed());
    TEST_ASSERT_TRUE(button_pressed());
}

static void test_releasing_is_not_a_press(void)
{
    fake_gpio_set_level(RELEASED);
    button_init(0);
    fake_gpio_set_level(PRESSED);
    count_presses(10);

    fake_gpio_set_level(RELEASED);
    TEST_ASSERT_EQUAL(0, count_presses(10));
}

static void test_two_separate_presses_count_twice(void)
{
    fake_gpio_set_level(RELEASED);
    button_init(0);

    int presses = 0;
    for (int i = 0; i < 2; i++) {
        fake_gpio_set_level(PRESSED);
        presses += count_presses(10);
        fake_gpio_set_level(RELEASED);
        presses += count_presses(10);
    }
    TEST_ASSERT_EQUAL(2, presses);
}

static void test_contact_bounce_is_ignored(void)
{
    fake_gpio_set_level(RELEASED);
    button_init(0);

    // The reading flickers every check: it is never stable, so it is not a press
    for (int i = 0; i < 10; i++) {
        fake_gpio_set_level(i % 2 ? RELEASED : PRESSED);
        TEST_ASSERT_FALSE(button_pressed());
    }
}

static void test_button_held_at_startup_is_not_a_press(void)
{
    fake_gpio_set_level(PRESSED);
    button_init(0);

    TEST_ASSERT_EQUAL(0, count_presses(10));
}

void run_button_tests(void)
{
    UnitySetTestFile(__FILE__); // so failures point at this file
    RUN_TEST(test_no_press_while_released);
    RUN_TEST(test_holding_the_button_counts_once);
    RUN_TEST(test_press_is_reported_only_once_stable);
    RUN_TEST(test_releasing_is_not_a_press);
    RUN_TEST(test_two_separate_presses_count_twice);
    RUN_TEST(test_contact_bounce_is_ignored);
    RUN_TEST(test_button_held_at_startup_is_not_a_press);
}
