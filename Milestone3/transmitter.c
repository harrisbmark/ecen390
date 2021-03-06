#include <stdio.h>
#include "transmitter.h"
#include "filter.h"
#include "../Lab2/buttons.h"
#include "../Lab2/switches.h"
#include "supportFiles/utils.h"
#include "supportFiles/mio.h"

#define TRANSMITTER_FIRE_TIME 20000
#define TRANSMITTER_HIGH_TIMER_CLEAR 0
#define TRANSMITTER_LOW_TIMER_CLEAR 0
#define TRANSMITTER_TIMER_CLEAR 0
#define TRANSMITTER_FREQUENCY_CLEAR 0

#define TRANSMITTER_JF_MIO_PIN 13
#define TRANSMITTER_MIO_HIGH 1
#define TRANSMITTER_MIO_LOW 0

#define TRANSMITTER_FIRE_TIME_TESTING 200
#define TRANSMITTER_TEST_PERIOD_MS 10
#define TRANSMITTER_TEST_MS_DELAY 300

enum transmitter_st_t
{
    transmitter_idle_st,
    transmitter_fire_high_st,
    transmitter_fire_low_st
} transmitter_current_state = transmitter_idle_st;

volatile static uint32_t transmitter_frequency;
volatile static uint32_t transmitter_timer;
volatile static uint32_t transmitter_high_timer;
volatile static uint32_t transmitter_low_timer;
volatile static bool transmitter_is_running;
volatile static bool transmitter_trigger_detected;
volatile static bool transmitter_continuous_mode;
volatile static bool transmitter_test_mode;

// The transmitter state machine generates a square wave output at the chosen frequency
// as set by transmitter_setFrequencyNumber(). The step counts for the frequencies
// are provided in filter.h

void transmitter_debug_print()
{
    static transmitter_st_t previous_state;
    static bool firstPass = true;

    if (previous_state != transmitter_current_state || firstPass)
    {
        firstPass = false;
        previous_state = transmitter_current_state;

        switch(transmitter_current_state)
        {
            case transmitter_idle_st:
                printf("transmitter_idle_st\n\r");
                break;

            case transmitter_fire_high_st:
                printf("transmitter_fire_high_st\n\r");
                break;

            case transmitter_fire_low_st:
                printf("transmitter_fire_low_st\n\r");
                break;
        }
    }
}

// Standard init function.
void transmitter_init()
{
    transmitter_is_running = false;
    transmitter_trigger_detected = false;
    transmitter_continuous_mode = false;
    transmitter_test_mode = false;
    transmitter_frequency = TRANSMITTER_FREQUENCY_CLEAR;
    transmitter_timer = TRANSMITTER_HIGH_TIMER_CLEAR;
    transmitter_high_timer = TRANSMITTER_HIGH_TIMER_CLEAR;
    transmitter_low_timer = TRANSMITTER_LOW_TIMER_CLEAR;

    mio_init(false);
    mio_setPinAsOutput(TRANSMITTER_JF_MIO_PIN);
}

// Starts the transmitter.
void transmitter_run()
{
    transmitter_trigger_detected = true;
    transmitter_is_running = true;
}

// Returns true if the transmitter is still running.
bool transmitter_running()
{
    return transmitter_is_running;
}

// Sets the frequency number. If this function is called while the
// transmitter is running, the frequency will not be updated until the
// transmitter stops and transmitter_run() is called again.
void transmitter_setFrequencyNumber(uint16_t frequencyNumber)
{
    transmitter_frequency = filter_frequencyTickTable[frequencyNumber % FILTER_FREQUENCY_COUNT]/2;
}

// Standard tick function.
void transmitter_tick()
{
    //if (transmitter_test_mode)
      //  transmitter_debug_print();

    switch (transmitter_current_state)
    {
        case transmitter_idle_st:
            if (transmitter_trigger_detected)
            {
                transmitter_trigger_detected = false;
                mio_writePin(TRANSMITTER_JF_MIO_PIN, TRANSMITTER_MIO_HIGH);
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
                transmitter_timer = TRANSMITTER_TIMER_CLEAR;
                transmitter_high_timer = TRANSMITTER_HIGH_TIMER_CLEAR;
                mio_writePin(TRANSMITTER_JF_MIO_PIN, TRANSMITTER_MIO_LOW);
                transmitter_current_state = transmitter_idle_st;
            }

            else if (transmitter_high_timer >= transmitter_frequency)
            {
                transmitter_high_timer = TRANSMITTER_HIGH_TIMER_CLEAR;
                mio_writePin(TRANSMITTER_JF_MIO_PIN, TRANSMITTER_MIO_LOW);
                if (transmitter_test_mode)
                    printf("\n");
                transmitter_current_state = transmitter_fire_low_st;
            }

            else
            {
                transmitter_current_state = transmitter_fire_high_st;
            }
            
            break;

        case transmitter_fire_low_st:
            if (transmitter_timer >= TRANSMITTER_FIRE_TIME && !transmitter_continuous_mode)
            {
                transmitter_timer = TRANSMITTER_TIMER_CLEAR;
                transmitter_low_timer = TRANSMITTER_LOW_TIMER_CLEAR;
                mio_writePin(TRANSMITTER_JF_MIO_PIN, TRANSMITTER_MIO_LOW);
                transmitter_current_state = transmitter_idle_st;
            }

            else if (transmitter_low_timer >= transmitter_frequency)
            {
                transmitter_low_timer = TRANSMITTER_LOW_TIMER_CLEAR;
                mio_writePin(TRANSMITTER_JF_MIO_PIN, TRANSMITTER_MIO_HIGH);
                if (transmitter_test_mode)
                    printf("\n");
                transmitter_current_state = transmitter_fire_high_st;
            }

            else
            {
                transmitter_current_state = transmitter_fire_low_st;
            }

            break;

        default:
            transmitter_timer = TRANSMITTER_HIGH_TIMER_CLEAR;
            transmitter_high_timer = TRANSMITTER_HIGH_TIMER_CLEAR;
            transmitter_low_timer = TRANSMITTER_LOW_TIMER_CLEAR;
            transmitter_current_state = transmitter_idle_st;
            break;
    }

    switch (transmitter_current_state)
    {
        case transmitter_idle_st:
            transmitter_is_running = false;
            break;

        case transmitter_fire_high_st:
            transmitter_is_running = true;
            transmitter_timer++;
            transmitter_high_timer++;

            if (transmitter_test_mode)
            {
                transmitter_debug_print();
                printf("1");
            }

            break;

        case transmitter_fire_low_st:
            transmitter_is_running = true;
            transmitter_timer++;
            transmitter_low_timer++;

            if (transmitter_test_mode)
            {
                transmitter_debug_print();
                printf("0");
            }

            break;
    }
}

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

void transmitter_runTest()
{
    printf("starting transmitter_runTest()\n\r");

        buttons_init();
        switches_init();
        transmitter_init();

        printf("starting non-continuous mode\n\r");

        while (!(buttons_read() & BUTTONS_BTN1_MASK))
        {
            uint16_t switchValue = switches_read() % FILTER_FREQUENCY_COUNT;

            transmitter_setFrequencyNumber(switchValue);
            transmitter_run();

            // Not continuous mode.
            while (transmitter_running() && !(buttons_read() & BUTTONS_BTN1_MASK)) {}
            utils_msDelay(TRANSMITTER_TEST_MS_DELAY);
        }

        if ((buttons_read() & BUTTONS_BTN1_MASK))
        {
            while ((buttons_read() & BUTTONS_BTN1_MASK))
            {
                utils_msDelay(TRANSMITTER_TEST_MS_DELAY);
            };
        }

        utils_msDelay(TRANSMITTER_TEST_MS_DELAY);

        printf("starting continuous mode\n\r");

        transmitter_setContinuousMode(true);
        transmitter_run();
        while (transmitter_running() && !(buttons_read() & BUTTONS_BTN1_MASK))
        {
            uint16_t switchValue = switches_read() % FILTER_FREQUENCY_COUNT;

            transmitter_setFrequencyNumber(switchValue);
        }


        if ((buttons_read() & BUTTONS_BTN1_MASK))
        {
            while ((buttons_read() & BUTTONS_BTN1_MASK))
            {
                utils_msDelay(TRANSMITTER_TEST_MS_DELAY);
            };
        }

        transmitter_setContinuousMode(false);

        printf("transmitter_runTest() done\n\r");
}
