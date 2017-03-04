#include "hitLedTimer.h"
#include "../supportFiles/leds.h"

#define HIT_LED_TIMER_TIME_UP 1500
#define HIT_LED_TIMER_CLEAR 0

#define HIT_LED_ZYBO_ON 0x000F
#define HIT_LED_ZYBO_OFF 0x0000

enum hitLedTimer_st_t
{
    hitLedTimer_start_st,
    hitLedTimer_init_st,
    hitLedTimer_idle_st,
    hitLedTimer_led_on_st
} hit_led_timer_current_state = hitLedTimer_start_st;

static bool hit_led_timer_running;
static bool hit_led_timer_hit_detected;

static uint8_t hit_led_timer_count;

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
            case hitLedTimer_start_st:
                printf("hitLedTimer_start_st\n\r");
                break;

            case hitLedTimer_init_st:
                printf("hitLedTimer_init_st\n\r");
                break;

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
}

// Calling this starts the timer.
void hitLedTimer_start()
{
    hit_led_timer_hit_detected = true;
}

// Returns true if the timer is currently running.
bool hitLedTimer_running()
{
    return hit_led_timer_running;
}

// Standard tick function.
void hitLedTimer_tick()
{
    switch (hit_led_timer_current_state)
    {
        case hitLedTimer_start_st:
            hit_led_timer_current_state = hitLedTimer_init_st;
            break;

        case hitLedTimer_init_st:
            hit_led_timer_current_state = hitLedTimer_idle_st;
            leds_init(true);
            break;

        case hitLedTimer_idle_st:
            if (hit_led_timer_hit_detected)
            {
                hit_led_timer_hit_detected = false;
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
                hit_led_timer_current_state = hitLedTimer_idle_st;
            }

            else
            {
                hit_led_timer_current_state = hitLedTimer_led_on_st;
            }

            break;

        default:
            hit_led_timer_current_state = hitLedTimer_start_st;
            break;
    }

    switch (hit_led_timer_current_state)
    {
        case hitLedTimer_start_st:
            break;

        case hitLedTimer_init_st:
            hit_led_timer_running = false;
            hit_led_timer_hit_detected = false;
            hit_led_timer_count = HIT_LED_TIMER_CLEAR;
            break;

        case hitLedTimer_idle_st:
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
}

// Turns the gun's hit-LED off.
void hitLedTimer_turnLedOff()
{
    leds_write(HIT_LED_ZYBO_OFF);
}