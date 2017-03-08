#include <stdio.h>
#include <stdint.h>
#include "hitLedTimer.h"
#include "../Lab2/buttons.h"
#include "supportFiles/leds.h"
#include "supportFiles/utils.h"
#include "supportFiles/mio.h"

#define HIT_LED_TIMER_TIME_UP 6250000
#define HIT_LED_TIMER_CLEAR 0

#define HIT_LED_JF3_MIO_PIN 11
#define HIT_LED_MIO_HIGH 1
#define HIT_LED_MIO_LOW 0

#define HIT_LED_ZYBO_ON 0x1
#define HIT_LED_ZYBO_OFF 0x0

#define HIT_LED_TEST_COUNT 10
#define HIT_LED_TEST_MS_DELAY 1000

enum hitLedTimer_st_t
{
    hitLedTimer_idle_st,
    hitLedTimer_led_on_st
} hit_led_timer_current_state = hitLedTimer_idle_st;

volatile static bool hit_led_timer_running;
volatile static bool hit_led_timer_hit_detected;
volatile static uint32_t hit_led_timer_count;

void hitLedTimer_debug_print()
{
    static hitLedTimer_st_t previous_state;
    static bool firstPass = true;

    if (previous_state != hit_led_timer_current_state || firstPass)
    {
        firstPass = false;
        previous_state = hit_led_timer_current_state;

        switch(hit_led_timer_current_state)
        {
            case hitLedTimer_idle_st:
                printf("hitLedTimer_idle_st\n\r");
                break;

            case hitLedTimer_led_on_st:
                printf("hitLedTimer_led_on_st\n\r");
                break;
        }
    }
}

// Standard init function. Implement it even if it is not necessary. You may need it later.
void hitLedTimer_init()
{
    hit_led_timer_running = false;
    hit_led_timer_hit_detected = false;
    hit_led_timer_count = HIT_LED_TIMER_CLEAR;

    leds_init(true);
    mio_init(false);
    mio_setPinAsOutput(HIT_LED_JF3_MIO_PIN);
}

// Calling this starts the timer.
void hitLedTimer_start()
{
    hit_led_timer_hit_detected = true;
    hit_led_timer_running = true;
}

// Returns true if the timer is currently running.
bool hitLedTimer_running()
{
    return hit_led_timer_running;
}

// Standard tick function.
void hitLedTimer_tick()
{
    hitLedTimer_debug_print();

    switch (hit_led_timer_current_state)
    {
        case hitLedTimer_idle_st:
            if (hit_led_timer_hit_detected)
            {
                hit_led_timer_hit_detected = false;
                hitLedTimer_turnLedOn();
                hit_led_timer_current_state = hitLedTimer_led_on_st;
            }

            else
            {
                hit_led_timer_current_state = hitLedTimer_idle_st;
            }

            break;

        case hitLedTimer_led_on_st:
            if (hit_led_timer_count >= HIT_LED_TIMER_TIME_UP)
            {
                hit_led_timer_count = HIT_LED_TIMER_CLEAR;
                hitLedTimer_turnLedOff();
                hit_led_timer_current_state = hitLedTimer_idle_st;
            }

            else
            {
                hit_led_timer_current_state = hitLedTimer_led_on_st;
            }

            break;

        default:
            hit_led_timer_count = HIT_LED_TIMER_CLEAR;
            hit_led_timer_current_state = hitLedTimer_idle_st;
            break;
    }

    switch (hit_led_timer_current_state)
    {
        case hitLedTimer_idle_st:
            hit_led_timer_running = false;
            break;

        case hitLedTimer_led_on_st:
            hit_led_timer_running = true;
            hit_led_timer_count++;
            break;
    }
}

// Turns the gun's hit-LED on.
void hitLedTimer_turnLedOn()
{
    leds_write(HIT_LED_ZYBO_ON);
    mio_writePin(HIT_LED_JF3_MIO_PIN, HIT_LED_MIO_HIGH);
}

// Turns the gun's hit-LED off.
void hitLedTimer_turnLedOff()
{
    leds_write(HIT_LED_ZYBO_OFF);
    mio_writePin(HIT_LED_JF3_MIO_PIN, HIT_LED_MIO_LOW);
}

void hitLedTimer_runTest()
{
    printf("Starting hitLedTimer run test...\n\r");

    hitLedTimer_init();

    for (uint8_t i = 0; i < HIT_LED_TEST_COUNT && !(buttons_read() & BUTTONS_BTN1_MASK); i++)
    {
        printf("Testing count %d out of %d\n\r", i + 1, HIT_LED_TEST_COUNT);

        hitLedTimer_start();

        while (hitLedTimer_running() && !(buttons_read() & BUTTONS_BTN1_MASK))
        {
            hitLedTimer_tick();
        }

        utils_msDelay(HIT_LED_TEST_MS_DELAY);
    }

    hitLedTimer_turnLedOff();

    printf("Ended hitLedTimer run test.\n\r");
}
