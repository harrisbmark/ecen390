/*********************************************************************************/
/* File: clockControl.c                                                          */
/* Purpose: To provide the function definitions to control the clock.            */
/* Author: Mark Harris                                                           */
/* Date: 10-05-16                                                                */
/*********************************************************************************/

// Required includes for this program.
#include "clockControl.h"
#include "clockDisplay.h"
#include "supportFiles/display.h"
#include <stdint.h>
#include <stdio.h>

// Required defines to clear the counters.
#define CLOCK_CONTROL_ADC_COUNTER_CLEAR 0
#define CLOCK_CONTROL_AUTO_COUNTER_CLEAR 0
#define CLOCK_CONTROL_RATE_COUNTER_CLEAR 0
#define CLOCK_CONTROL_ONE_SEC_INCR_CLEAR 0

// Required defines for the counter max values.
#define CLOCK_CONTROL_ONE_SEC_INCR 20
#define CLOCK_CONTROL_ADC_COUNTER_MAX_VALUE 1
#define CLOCK_CONTROL_AUTO_COUNTER_MAX_VALUE 10
#define CLOCK_CONTROL_RATE_COUNTER_MAX_VALUE 2

// Volatile variable used to flag when the timer is up.
volatile unsigned char timer_flag = 0;

// Enum used to create the states for the clock control state machine.
enum clockControl_st_t {clock_control_start_st, clock_control_init_st, clock_control_wait_first_touch_st, clock_control_waiting_for_touch_st, clock_control_increment_sec_st, clock_control_adc_counter_running_st, clock_control_auto_counter_running_st, clock_control_rate_counter_running_st, clock_control_rate_counter_expired_st} current_state = clock_control_start_st;

// Global variables used for the counters in the state machine.
uint32_t adc_counter = CLOCK_CONTROL_ADC_COUNTER_CLEAR;
uint32_t auto_counter = CLOCK_CONTROL_AUTO_COUNTER_CLEAR;
uint32_t rate_counter = CLOCK_CONTROL_RATE_COUNTER_CLEAR;
uint32_t sec_counter = CLOCK_CONTROL_ONE_SEC_INCR_CLEAR;

/*********************************************************************************/
/* Function: clockControl_debug_print                                            */
/* Purpose: To output the current state of the state machine to the user.        */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void clockControl_debug_print()
{
    // Static variables used to keep track of the previous state and whether or not we are in the first pass.
    static clockControl_st_t previous_state;
    static bool firstPass = true;

    // If the previous state is not equal to the current state or we are in the first pass.
    if (previous_state != current_state || firstPass)
    {
        // Set the first pass to false to keep from getting into this if statement. Set the previous state to the current state.
        firstPass = false;
        previous_state = current_state;

        // Switch statement on the current state to select options for the state machine.
        switch(current_state)
        {
            // If the state machine is in the start state, print that out to the user and break.
            case clock_control_start_st:
                printf("start_st\n\r");
                break;

            // Else, if the state machine is in the init state, print that out to the user and break.
            case clock_control_init_st:
                printf("init_st\n\r");
                break;

            // Else, if the state machine is in the wait_first_touch state, print that out to the user and break.
            case clock_control_wait_first_touch_st:
                printf("wait_first_touch_st\n\r");
                break;

            // Else, if the state machine is in the increment_sec state, print that out to the user and break.
            case clock_control_increment_sec_st:
                printf("increment_sec_st\n\r");
                break;

            // Else, if the state machine is in the waiting_for_touch state, print that out to the user and break.
            case clock_control_waiting_for_touch_st:
                printf("waiting_for_touch_st\n\r");
                break;

            // Else, if the state machine is in the adc_counter_running state, print that out to the user and break.
            case clock_control_adc_counter_running_st:
                printf("adc_timer_running_st\n\r");
                break;

            // Else, if the state machine is in the auto_counter_running state, print that out to the user and break.
            case clock_control_auto_counter_running_st:
                printf("auto_timer_running_st\n\r");
                break;

            // Else, if the state machine is in the rate_counter_running state, print that out to the user and break.
            case clock_control_rate_counter_running_st:
                printf("rate_timer_running_st\n\r");
                break;

            // Else, if the state machine is in the rate_counter_expired state, print that out to the user and break.
            case clock_control_rate_counter_expired_st:
                printf("rate_timer_expired_st\n\r");
                break;
     }
  }
}

/*********************************************************************************/
/* Function: clockControl_tick                                                   */
/* Purpose: To control the state machine for the clock control program.          */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void clockControl_tick()
{
        // Switch on the current state. This switch statement is for the state machine transitions and mealy outputs.
    switch (current_state)
    {
                // If we are in the start state.
        case clock_control_start_st:
                        // Immediately move to the init state and break.
            current_state = clock_control_init_st;
            break;

                // Else, if we are in the init state.
        case clock_control_init_st:
                        // Immediately move to the wait_first_touch state and break.
                current_state = clock_control_wait_first_touch_st;
                    break;

                // Else, if we are in the wait_first_touch state.
        case clock_control_wait_first_touch_st:
                        // If the display is touched.
                if (display_isTouched())
                {
                            // Set the current state to the waiting_for_touch state and clear the old touch data.
                    current_state = clock_control_waiting_for_touch_st;
                    display_clearOldTouchData();
                }

                        // Otherwise.
                else
                {
                            // Stay in the current state to the wait_first_touch_state.
                    current_state = clock_control_wait_first_touch_st;
                }

                        // Break.
                break;

                // Else, if we are in the waiting_for_touch state.
        case clock_control_waiting_for_touch_st:
                        // If the display is touched.
            if (display_isTouched())
            {
                                // Set the current state to the adc_counter_running state and clear the old touch data.
                current_state = clock_control_adc_counter_running_st;
                display_clearOldTouchData();
            }

                        // Otherwise, if the display is not touched and the sec_counter is greater than or equal to it's max value.
            else if (!display_isTouched() && sec_counter >= CLOCK_CONTROL_ONE_SEC_INCR)
            {
                                // Set the current state to the increment_sec state.
                    current_state = clock_control_increment_sec_st;
            }

                        // Otherwise.
            else
            {
                                // Stay in the waiting_for_touch state.
                current_state = clock_control_waiting_for_touch_st;
            }

                        // Break.
            break;

                // Else, if we are in the increment_sec state.
        case clock_control_increment_sec_st:
                        // Set the current state to waiting_for_touch, advance the time by one second, then update the display.
                current_state = clock_control_waiting_for_touch_st;
                clockDisplay_advanceTimeOneSecond();
                clockDisplay_updateTimeDisplay(false);

                        // Break.
                break;

                // Else, if we are in the adc_counter_running state.
        case clock_control_adc_counter_running_st:
                        // If the display is not touched and the adc_counter is greater than or equal to it's max value.
            if (!display_isTouched() && adc_counter >= CLOCK_CONTROL_ADC_COUNTER_MAX_VALUE)
            {
                                // Set the current state to the waiting_for_touch state, increment the time, and update the display.
                current_state = clock_control_waiting_for_touch_st;
                clockDisplay_performIncDec();
                clockDisplay_updateTimeDisplay(false);
            }

                        // Otherwise, if the display is touched and the adc_counter is greater than or equal to it's max value.
            else if (display_isTouched() && adc_counter >= CLOCK_CONTROL_ADC_COUNTER_MAX_VALUE)
            {
                                // Set the current state to the auto_counter_running state.
                current_state = clock_control_auto_counter_running_st;
            }

                        // Otherwise.
            else
            {
                                // Stay in the adc_counter_running state.
                current_state = clock_control_adc_counter_running_st;
            }

                        // Break.
            break;

                // Else, if we are in the auto_counter_running state.
        case clock_control_auto_counter_running_st:
                        // If the display is not touched.
            if (!display_isTouched())
            {
                                // Set the current state to the waiting_for_touch state, increment the clock, and update the display.
                current_state = clock_control_waiting_for_touch_st;
                clockDisplay_performIncDec();
                clockDisplay_updateTimeDisplay(false);
            }

                        // Otherwise, if the display is touched and the auto_counter is greater than or equal to it's max value.
            else if (display_isTouched() && auto_counter >= CLOCK_CONTROL_AUTO_COUNTER_MAX_VALUE)
            {
                                // Set the current state to the reate_counter_running state, increment the clock, and update the display.
                current_state = clock_control_rate_counter_running_st;
                clockDisplay_performIncDec();
                clockDisplay_updateTimeDisplay(false);
            }

                        // Otherwise.
            else
            {
                                // Stay in the auto_counter_running state.
                current_state = clock_control_auto_counter_running_st;
            }

                        // Break.
            break;

                // Else, if we are in the rate_counter_running state.
        case clock_control_rate_counter_running_st:
                        // If the display is not touced.
            if (!display_isTouched())
            {
                                // Set the current state to the waiting_for_touch state.
                current_state = clock_control_waiting_for_touch_st;
            }

                        // Otherwise, if the display is touched and rate_counter is greater than or equal to it's max value.
            else if (display_isTouched() && rate_counter >= CLOCK_CONTROL_RATE_COUNTER_MAX_VALUE)
            {
                                // Set the current state equal to the rate_counter_expired state.
                current_state = clock_control_rate_counter_expired_st;
            }

                        // Otherwise.
            else
            {
                                // Stay in the rate_counter_running state.
                current_state = clock_control_rate_counter_running_st;
            }

                        // Break.
            break;

                // Else, if we are in the rate_counter_expired state.
        case clock_control_rate_counter_expired_st:
                        // If the display is not touched.
            if (!display_isTouched())
            {
                                // Set the current staet to the waiting_for_touch state.
                current_state = clock_control_waiting_for_touch_st;
            }

                        // Otherwise, if the display is touched.
            else if (display_isTouched())
            {
                                // Set the current state to the rate_counter_running state, increment the clock, and update the display.
                current_state = clock_control_rate_counter_running_st;
                clockDisplay_performIncDec();
                clockDisplay_updateTimeDisplay(false);
            }

                        // Break.
            break;

                // Default case (when something terrible goes wrong).
        default:
                        // Set the current state to the start state, and break.
            current_state = clock_control_start_st;
            break;
    }

        // Switch for the current_state. This switch is used for computations completed in the state (moore outputs).
    switch (current_state)
    {
            // If we are in the init state.
            case clock_control_start_st:
                    // Break, because we don't do anything in the start state.
                    break;

            // Else, if we are in the init state.
        case clock_control_init_st:
                    // Reset all of the counters to their "clear" value.
                adc_counter = CLOCK_CONTROL_ADC_COUNTER_CLEAR;
                    auto_counter = CLOCK_CONTROL_AUTO_COUNTER_CLEAR;
                    rate_counter = CLOCK_CONTROL_RATE_COUNTER_CLEAR;
                    sec_counter = CLOCK_CONTROL_ONE_SEC_INCR_CLEAR;

                    // Break.
                break;

            // Else, if we are in the wait_first_touch state.
        case clock_control_wait_first_touch_st:
                    // Break, because we don't do anything in the wait_first_touch state.
                break;

            // Else, if we are in the waiting_for_touch state.
            case clock_control_waiting_for_touch_st:
                    // Clear the adc, auto, and rate counters.
            adc_counter = CLOCK_CONTROL_ADC_COUNTER_CLEAR;
                    auto_counter = CLOCK_CONTROL_AUTO_COUNTER_CLEAR;
                    rate_counter = CLOCK_CONTROL_RATE_COUNTER_CLEAR;

                    // Increment the sec_counter (keeps control of the incrementing seconds of the clock).
                    sec_counter++;

                    // Break.
                    break;

            // Else, if we are in the increment_sec state.
        case clock_control_increment_sec_st:
                    // Clear the sec_counter to start over and break.
            sec_counter = CLOCK_CONTROL_ONE_SEC_INCR_CLEAR;
            break;

            // Else, if we are in the adc_counter_running state.
        case clock_control_adc_counter_running_st:
                    // Increment the adc_counter and break.
                    adc_counter++;
            break;

            // Else, if we are in the auto_counter_running state.
        case clock_control_auto_counter_running_st:
                    // Increment the auto_counter and break.
            auto_counter++;
            break;

            // Else, if we are in the rate_counter_running state.
        case clock_control_rate_counter_running_st:
                    // Increment the rate_counter and break.
            rate_counter++;
            break;

            // Else, if we are in the rate_counter_expired state.
        case clock_control_rate_counter_expired_st:
                    // Rest the rate_counter and break.
            rate_counter = CLOCK_CONTROL_RATE_COUNTER_CLEAR;
            break;

            // The default case (when something terrible goes wrong).
        default:
                    // Break.
            break;
    }

        // Call the clockControl_debug_print function to see what states we are in.
    clockControl_debug_print();
}
