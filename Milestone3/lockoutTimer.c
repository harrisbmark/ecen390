#include "lockoutTimer.h"

#define LOCKOUT_TIMER_TIME_UP 1500
#define LOCKOUT_TIMER_CLEAR 0

enum lockoutTimer_st_t
{
    lockoutTimer_start_st,
    lockoutTimer_init_st,
    lockoutTimer_idle_st,
    lockoutTimer_timer_st
} lockout_timer_current_state = lockoutTimer_start_st;

static bool lockout_timer_running;
static bool lockout_timer_hit_detected;

static uint8_t lockout_timer_count;

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
            case lockoutTimer_start_st:
                printf("lockoutTimer_start_st\n\r");
                break;

            case lockoutTimer_init_st:
                printf("lockoutTimer_init_st\n\r");
                break;

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
}

// Returns true if the timer is running.
bool lockoutTimer_running()
{
    return lockout_timer_running;
}

// Standard tick function.
void lockoutTimer_tick()
{
    switch (lockout_timer_current_state)
    {
        case lockoutTimer_start_st:
            lockout_timer_current_state = lockoutTimer_init_st;
            break;

        case lockoutTimer_init_st:
            lockout_timer_current_state = lockoutTimer_idle_st;
            break;

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
                lockout_timer_current_state = lockoutTimer_idle_st;
            }

            else
            {
                lockout_timer_current_state = lockoutTimer_timer_st;
            }

            break;

        default:
            lockout_timer_current_state = lockoutTimer_start_st;
            break;
    }

    switch (lockout_timer_current_state)
    {
        case lockoutTimer_start_st:
            break;

        case lockoutTimer_init_st:
            lockout_timer_running = false;
            lockout_timer_hit_detected = false;
            lockout_timer_count = LOCKOUT_TIMER_CLEAR;
            break;

        case lockoutTimer_idle_st:
            break;

        case lockoutTimer_timer_st:
            lockout_timer_running = true;
            lockout_timer_count++;
            break;
    }
}
