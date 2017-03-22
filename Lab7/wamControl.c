/*********************************************************************************/
/* File: wamControl.c                                                            */
/* Purpose: To provide the code necessary to run the whack-a-mole game using a   */
/*          state machine and provided functions.                                */
/* Author: Mark Harris                                                           */
/* Date: 12-06-16                                                                */
/*********************************************************************************/
#include "wamControl.h"
#include <stdio.h>

// Necessary defines to set the maximum amount of ticks for an ADC settle and to reset the ADC timer.
#define WAM_CONTROL_ADC_WAIT_TIME 1
#define WAM_CONTROL_ADC_TIMER_RESET 0

// Necessary defines to set the default values for the static global variables.
#define WAM_CONTROL_DEFAULT_ACTIVE_MOLE_COUNT 1
#define WAM_CONTROL_DEFAULT_MS_PER_TICK 50
#define WAM_CONTROL_DEFAULT_MISS_COUNT 50

// Enum used to create the states for the whack-a-mole game.
enum wamControl_st_t
{
    wamControl_start_st,            // State used to start the state machine.
    wamControl_wait_touch_st,       // State used to wait for touch input from the user.
    wamControl_adc_wait_st,         // State used to wait for the ADC to settle.
    wamControl_wait_stop_touch_st   // State used to wait for the user to stop touching the display.
} wam_control_current_state = wamControl_start_st;

// Global variables used to determine the end of the game and the maximum amount of active moles.
static uint16_t max_miss_count;
static uint16_t max_active_moles;

// Global variables used to set the number of milliseconds per tick and the ADC timer.
static uint16_t ms_per_tick;
static uint8_t adc_timer;

/*********************************************************************************/
/* Function: wamControl_debug_print                                              */
/* Purpose: To print the current state of the state machine for debugging.       */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void wamControl_debug_print()
{
    // Create static variables storing the previous state and if we are on the first pass.
    static wamControl_st_t previous_state;
    static bool firstPass = true;

    // If the previous state is different from the current state or if we are on the first pass.
    if (previous_state != wam_control_current_state || firstPass)
    {
        // Set the first pass equal to false. Set the previous state equal to the current state.
        firstPass = false;
        previous_state = wam_control_current_state;

        // Switch on the current state to show what output to deliver.
        switch(wam_control_current_state)
        {
            // If we are in the start state, then output that result to the user.
            case wamControl_start_st:
                printf("wamControl_start_st\n\r");
                break;

            // If we are in the wait touch state, then output that result to the user.
            case wamControl_wait_touch_st:
                printf("wamControl_wait_touch_st\n\r");
                break;

            // If we are in the ADC wait state, then output that result to the user.
            case wamControl_adc_wait_st:
                printf("wamControl_adc_wait_st\n\r");
                break;

            // If we are in the wait stop touch state, then output that result to the user.
            case wamControl_wait_stop_touch_st:
                printf("wamControl_wait_stop_touch_st\n\r");
                break;
     }
  }
}

/*********************************************************************************/
/* Function: wamControl_init                                                     */
/* Purpose: To initialize the global variables for the wamControl state machine. */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void wamControl_init()
{
    // Set the static global variables to specified default values.
    max_active_moles = WAM_CONTROL_DEFAULT_ACTIVE_MOLE_COUNT;
    max_miss_count = WAM_CONTROL_DEFAULT_MISS_COUNT;
    ms_per_tick = WAM_CONTROL_DEFAULT_MS_PER_TICK;
    adc_timer = WAM_CONTROL_ADC_TIMER_RESET;
}

/*********************************************************************************/
/* Function: wamControl_setMsPerTick                                             */
/* Purpose: To set the global variable ms_per_tick.                              */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void wamControl_setMsPerTick(uint16_t msPerTick)
{
    // Set the static global variable ms_per_tick to the passed value.
    ms_per_tick = msPerTick;
}

/*********************************************************************************/
/* Function: wamControl_getMsPerTick                                             */
/* Purpose: To get the global variable ms_per_tick.                              */
/* Returns: An integer value (16 bits).                                          */
/*********************************************************************************/
uint16_t wamControl_getMsPerTick()
{
    // Return the static global variable ms_per_tick value.
    return ms_per_tick;
}

/*********************************************************************************/
/* Function: wamControl_tick                                                     */
/* Purpose: To run a "tick" for the whack-a-mole state machine.                  */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void wamControl_tick()
{
    // Switch on the whack-a-mole current state. This switch statement is for the transitions and Mealy outputs.
    switch (wam_control_current_state)
    {
        // If we are in the start state then immediately jump to the wait touch state.
        case wamControl_start_st:
            wam_control_current_state = wamControl_wait_touch_st;
            break;

        // Otherwise, we are in the wait touch state.
        case wamControl_wait_touch_st:
            // If the display is touched.
            if (display_isTouched())
            {
                // Clear the old touch data and reset the ADC timer.
                display_clearOldTouchData();
                adc_timer = WAM_CONTROL_ADC_TIMER_RESET;

                // Jump to the ADC wait state.
                wam_control_current_state = wamControl_adc_wait_st;
            }

            // Otherwise, stay in the wait touch state.
            else
            {
                wam_control_current_state = wamControl_wait_touch_st;
            }

            break;

        // Otherwise, if we are in the ADC wait state.
        case wamControl_adc_wait_st:
            // If the ADC timer has reached to maximum set ADC wait time.
            if (adc_timer >= WAM_CONTROL_ADC_WAIT_TIME)
            {
                // If the display is touched.
                if (display_isTouched())
                {
                    // Create a temporary display point pointer and a display touch intensity variable.
                    wamDisplay_point_t* display_point = NULL;
                    uint8_t display_touch_intensity = 0;

                    // Get the touch point data from the display and place them in our newly created pointer and variable.
                    display_getTouchedPoint(&display_point->x, &display_point->y, &display_touch_intensity);
                    wamDisplay_whackMole(display_point);

                    // Jump to the wait stop touch state.
                    wam_control_current_state = wamControl_wait_stop_touch_st;
                }

                // Otherwise, jump back to the wait touch state.
                else
                {
                    wam_control_current_state = wamControl_wait_touch_st;
                }
            }

            // Otherwise, stay in the ADC wait state.
            else
            {
                wam_control_current_state = wamControl_adc_wait_st;
            }

            break;

        // Otherwise, if we are in the wait stop touch state.
        case wamControl_wait_stop_touch_st:
            // If the display is touched, then stay in the wait stop touch state.
            if (display_isTouched())
            {
                wam_control_current_state = wamControl_wait_stop_touch_st;
            }

            // Otherwise, jump immediately to the wait touch state.
            else
            {
                wam_control_current_state = wamControl_wait_touch_st;
            }

            break;

        // Otherwise, if all else fails, jump to the start state.
        default:
            wam_control_current_state = wamControl_start_st;
            break;
    }

    // Switch on the whack-a-mole current state. This switch is used for computations completed in the state and Moore outputs.
    switch (wam_control_current_state)
    {
        // If we are in the start state, then don't do anything.
        case wamControl_start_st:
            break;

        // Otherwise, if we are in the wait touch state.
        case wamControl_wait_touch_st:
            // Update the mole tick counts.
            wamDisplay_updateAllMoleTickCounts();

            // If the number of active moles dips below the set maximum, then activate a random mole.
            if (wamDisplay_getActiveMoleCount() < max_active_moles)
            {
                wamDisplay_activateRandomMole();
            }

            break;

        // Otherwise, if we are in the ADC wait state, then increment the ADC timer.
        case wamControl_adc_wait_st:
            adc_timer++;
            break;

        // Otherwise, if we are in the wait stop touch state.
        case wamControl_wait_stop_touch_st:
            // Update the mole tick counts.
            wamDisplay_updateAllMoleTickCounts();

            // If the number of active moles dips below the set maximum, then activate a random mole.
            if (wamDisplay_getActiveMoleCount() < max_active_moles)
            {
                wamDisplay_activateRandomMole();
            }

            break;

        // Otherwise, if all else fails, don't do anything.
        default:
            break;
    }

    // Output what state we are currently in. Used for debugging.
    //wamControl_debug_print();
}

/*********************************************************************************/
/* Function: wamControl_getRandomMoleAsleepInterval                              */
/* Purpose: To return a random sleep interval.                                   */
/* Returns: An integer value (32 bits typedef as a moleTickCount_t).             */
/*********************************************************************************/
wamDisplay_moleTickCount_t wamControl_getRandomMoleAsleepInterval()
{
    // Return a random number back to the user.
    return rand();
}

/*********************************************************************************/
/* Function: wamControl_getRandomMoleAwakeInterval                               */
/* Purpose: To return a random wake interval.                                    */
/* Returns: An integer value (32 bits typedef as a moleTickCount_t).             */
/*********************************************************************************/
wamDisplay_moleTickCount_t wamControl_getRandomMoleAwakeInterval()
{
    // Return a random number back to the user.
    return rand();
}

/*********************************************************************************/
/* Function: wamControl_get_random_number                                        */
/* Purpose: To return a random number.                                           */
/* Returns: An integer value (32 bits typedef as a moleTickCount_t).             */
/*********************************************************************************/
wamDisplay_moleTickCount_t wamControl_get_random_number()
{
    // Return a random number back to the user.
    return rand();
}

/*********************************************************************************/
/* Function: wamControl_setMaxActiveMoles                                        */
/* Purpose: To set the maximum number of active moles allowed.                   */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void wamControl_setMaxActiveMoles(uint16_t count)
{
    // Set the static global variable max_active_moles to the passed value.
    max_active_moles = count;
}

/*********************************************************************************/
/* Function: wamControl_getMaxActiveMoles                                        */
/* Purpose: To get the maximum number of active moles allowed.                   */
/* Returns: An integer value (16 bits).                                          */
/*********************************************************************************/
uint16_t wamControl_getMaxActiveMoles()
{
    // Return the static global variable max_active_moles to the user.
    return max_active_moles;
}

/*********************************************************************************/
/* Function: wamControl_setRandomSeed                                            */
/* Purpose: To set the seed for the rand function.                               */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void wamControl_setRandomSeed(uint32_t seed)
{
    // Seed the rand function with the passed seed.
    srand(seed);
}

/*********************************************************************************/
/* Function: wamControl_setMaxMissCount                                          */
/* Purpose: To set the global variable max_miss_count.                           */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void wamControl_setMaxMissCount(uint16_t missCount)
{
    // Set the static global variable max_miss_count to the passed miss count.
    max_miss_count = missCount;
}

/*********************************************************************************/
/* Function: wamControl_isGameOver                                               */
/* Purpose: To inform the user if the game is over.                              */
/* Returns: A boolean value.                                                     */
/*********************************************************************************/
bool wamControl_isGameOver()
{
    // If the current miss score is greater than or equal to the maximum set miss count, return true.
    if (wamDisplay_getMissScore() >= max_miss_count)
    {
        return true;
    }

    // Otherwise, return false.
    else
    {
        return false;
    }
}
