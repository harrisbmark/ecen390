#include <stdio.h>
#include <stdint.h>
#include "lockoutTimer.h"
#include "../Lab2/buttons.h"
#include "supportFiles/utils.h"

#define LOCKOUT_TIMER_TIME_UP 100
#define LOCKOUT_TIMER_CLEAR 0

#define LOCKOUT_TEST_COUNT 10
#define LOCKOUT_TEST_MS_DELAY 1000

enum lockoutTimer_st_t
{
    lockoutTimer_idle_st,
    lockoutTimer_timer_st
} lockout_timer_current_state = lockoutTimer_idle_st;

volatile static bool lockout_timer_running;
volatile static bool lockout_timer_hit_detected;
volatile static uint8_t lockout_timer_count;

void lockoutTimer_debug_print()
{
    static lockoutTimer_st_t previous_state;
    static bool firstPass = true;

    if (previous_state != lockout_timer_current_state || firstPass)
    {
        firstPass = false;
        previous_state = lockout_timer_current_state;

        switch(lockout_timer_current_state)
        {
            case lockoutTimer_idle_st:
                printf("lockoutTimer_idle_st\n\r");
                break;

            case lockoutTimer_timer_st:
                printf("lockoutTimer_timer_st\n\r");
                break;
        }
    }
}

// Standard init function. Implement even if you don't find it necessary at present.
// Might be handy later.
void lockoutTimer_init()
{
    lockout_timer_running = false;
    lockout_timer_hit_detected = false;
    lockout_timer_count = LOCKOUT_TIMER_CLEAR;
}

// Calling this starts the timer.
void lockoutTimer_start()
{
    lockout_timer_hit_detected = true;
    lockout_timer_running = true;
}

// Returns true if the timer is running.
bool lockoutTimer_running()
{
    return lockout_timer_running;
}

// Standard tick function.
void lockoutTimer_tick()
{
    lockoutTimer_debug_print();

    switch (lockout_timer_current_state)
    {
        case lockoutTimer_idle_st:
            if (lockout_timer_hit_detected)
            {
                lockout_timer_hit_detected = false;
                lockout_timer_current_state = lockoutTimer_timer_st;
            }

            else
            {
                lockout_timer_current_state = lockoutTimer_idle_st;
            }

            break;

        case lockoutTimer_timer_st:
            if (lockout_timer_count >= LOCKOUT_TIMER_TIME_UP)
            {
                lockout_timer_count = LOCKOUT_TIMER_CLEAR;
                lockout_timer_current_state = lockoutTimer_idle_st;
            }

            else
            {
                lockout_timer_current_state = lockoutTimer_timer_st;
            }

            break;

        default:
            lockout_timer_count = LOCKOUT_TIMER_CLEAR;
            lockout_timer_current_state = lockoutTimer_idle_st;
            break;
    }

    switch (lockout_timer_current_state)
    {
        case lockoutTimer_idle_st:
            lockout_timer_running = false;
            break;

        case lockoutTimer_timer_st:
            lockout_timer_running = true;
            lockout_timer_count++;
            break;
    }
}

void lockoutTimer_runTest()
{
    printf("Started lockoutTimer run test...\n\r");

    lockoutTimer_init();

    for (uint8_t i = 0; i < LOCKOUT_TEST_COUNT && !(buttons_read() & BUTTONS_BTN1_MASK); i++)
    {
        printf("Test %d of %d\n\r", i + 1, LOCKOUT_TEST_COUNT);

        lockoutTimer_start();

        while (lockoutTimer_running() && !(buttons_read() & BUTTONS_BTN1_MASK))
        {
            lockoutTimer_tick();
        }

        utils_msDelay(LOCKOUT_TEST_MS_DELAY);
    }

    printf("Ended lockoutTimer run test.\n\r");
}
