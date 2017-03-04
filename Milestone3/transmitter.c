#include transmitter.h
#include "filter.h"

#define TRANSMITTER_FIRE_TIME 1500
#define TRANSMITTER_HIGH_TIMER_CLEAR 0
#define TRANSMITTER_LOW_TIMER_CLEAR 0
#define TRANSMITTER_FREQUENCY_CLEAR 0

enum transmitter_st_t
{
    transmitter_start_st,
    transmitter_init_st,
    transmitter_idle_st,
    transmitter_fire_high_st,
    transmitter_fire_low_st
} transmitter_current_state = transmitter_start_st;

static uint16_t transmitter_frequency;
static uint16_t transmitter_timer;
static uint16_t transmitter_high_timer;
static uint16_t transmitter_low_timer;
static bool transmitter_running;
static bool transmitter_trigger_detected;
static bool transmitter_continuous_mode;
static bool transmitter_test_mode;

// The transmitter state machine generates a square wave output at the chosen frequency
// as set by transmitter_setFrequencyNumber(). The step counts for the frequencies
// are provided in filter.h

// Standard init function.
void transmitter_init()
{
    transmitter_running = false;
    transmitter_trigger_detected = false;
    transmitter_continuous_mode = false;
    transmitter_test_mode = false;
    transmitter_frequency = TRANSMITTER_FREQUENCY_CLEAR;
}

// Starts the transmitter.
void transmitter_run()
{
    transmitter_trigger_detected = true;
}

// Returns true if the transmitter is still running.
bool transmitter_running()
{
    return transmitter_running = true;
}

// Sets the frequency number. If this function is called while the
// transmitter is running, the frequency will not be updated until the
// transmitter stops and transmitter_run() is called again.
void transmitter_setFrequencyNumber(uint16_t frequencyNumber)
{
    transmitter_frequency = filter_frequencyTickTable[frequencyNumber % FILTER_FREQUENCY_COUNT];
}

// Standard tick function.
void transmitter_tick()
{
    switch (transmitter_current_state)
    {
        case transmitter_start_st:
            transmitter_current_state = transmitter_init_st;
            break;

        case transmitter_init_st:
            transmitter_current_state = transmitter_idle_st;
            break;

        case transmitter_idle_st:
            if (trigger_enable)
            {
                transmitter_current_state = transmitter_fire_high_st;
            }

            else
            {
                transmitter_current_state = transmitter_idle_st;
            }

            break;

        case transmitter_fire_high_st:
            if (transmitter_timer >= TRANSMITTER_FIRE_TIME && !transmitter_continuous_mode)
            {
                transmitter_current_state = transmitter_idle_st;
            }

            else if (transmitter_high_timer >= transmitter_frequency && !(transmitter_timer >= TRANSMITTER_FIRE_TIME))
            {

            }

            else
            {

            }
            
            break;

        case transmitter_fire_low_st:
            break;

        default:
            break;
    }

    switch (transmitter_current_state)
    {
        case transmitter_start_st:
            break;

        case transmitter_init_st:
            break;

        case transmitter_idle_st:
            break;

        case transmitter_fire_high_st:
            break;

        case transmitter_fire_low_st:
            break;
    }
}

// Tests the transmitter.
void transmitter_runTest(){}

// Runs the transmitter continuously.
// if continuousModeFlag == true, transmitter runs continuously, otherwise, transmits one pulse-width and stops.
// To set continuous mode, you must invoke this function prior to calling transmitter_run().
// If the transmitter is in currently in continuous mode, it will stop running if this function is
// invoked with continuousModeFlag == false. It can stop immediately or wait until the last 200 ms pulse is complete.
// NOTE: while running continuously, the transmitter will change frequencies at the end of each 200 ms pulse.
void transmitter_setContinuousMode(bool continuousModeFlag)
{
    transmitter_continuous_mode = continuousModeFlag;
}

// This is provided for testing as explained in the transmitter section of the web-page. When enabled,
// debug prints are enabled to help to demonstrate the behavior of the transmitter.
void transmitter_enableTestMode()
{
    transmitter_test_mode = true;
}

// This is provided for testing as explained in the transmitter section of the web-page. When disabled,
// debug prints that were previously enabled no longer appear.
void transmitter_disableTestMode()
{
    transmitter_test_mode = false;
}