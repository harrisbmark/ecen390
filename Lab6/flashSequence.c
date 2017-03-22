/*********************************************************************************/
/* File: flashSequence.c                                                         */
/* Purpose: To provide the necessary code to flash one square at a time for the  */
/*          Simon game (provided a list of regions to flash in globals.h).       */
/* Author: Mark Harris                                                           */
/* Date: 11-16-16                                                                */
/*********************************************************************************/
#include "flashSequence.h"
#include "globals.h"
#include "simonDisplay.h"
#include <stdint.h>
#include <stdio.h>

// Necessary defines to set the flash and pause times for the flash sequence.
#define FLASH_SEQUENCE_FLASH_TIME 2
#define FLASH_SEQUENCE_PAUSE_TIME 4

// Necessary defines to clear the global variables used for the flash sequence.
#define FLASH_SEQUENCE_FLASH_TIME_CLEAR 0
#define FLASH_SEQUENCE_PAUSE_TIME_CLEAR 0
#define FLASH_SEQUENCE_COUNT_CLEAR 0

// Necessary define to increment the flash sequence.
#define FLASH_SEQUENCE_INCREMENT_SEQUENCE 1

// Enum used to create the states for the flash sequence.
enum flashSequence_st_t
{
    flashSequence_start_st,             // State used to start the state machine.
    flashSequence_init_st,              // State used to initialize the state machine.
    flashSequence_flash_st,             // State used to "flash" the button the LCD.
    flashSequence_pause_st,             // State used to pause the "flashed" button on the LCD.
    flashSequence_is_completed_st,      // State used to determine if the machine is done.
    flashSequence_wait_for_disable_st   // State used to wait until disabled.
} flash_sequence_current_state = flashSequence_start_st;

// Global variables used for the flags in the state machine.
static bool enable;
static bool completed;

// Global variables used for the timers in the state machine.
static uint8_t flash_timer;
static uint8_t pause_timer;

// Global variable used to count the sequence number in the state machine.
static uint8_t count;

/*********************************************************************************/
/* Function: flashSequence_debug_print                                           */
/* Purpose: To print the current state of the state machine for debugging.       */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void flashSequence_debug_print()
{
    // Create static variables storing the previous state and if we are on the first pass.
    static flashSequence_st_t previous_state;
    static bool firstPass = true;

    // If the previous state is different from the current state or if we are on the first pass.
    if (previous_state != flash_sequence_current_state || firstPass)
    {
        // Set the first pass equal to false. Set the previous state equal to the current state.
        firstPass = false;
        previous_state = flash_sequence_current_state;

        // Switch on the current state to show what output to deliver.
        switch(flash_sequence_current_state)
        {
            // If we are in the start state, then output that result to the user.
            case flashSequence_start_st:
                printf("flashSequence_start_st\n\r");
                break;

            // Otherwise, if we are in the init state, then output that result to the user.
            case flashSequence_init_st:
                printf("flashSequence_init_st\n\r");
                break;

            // Otherwise, if we are in the flash state, then output that result to the user.
            case flashSequence_flash_st:
                printf("flashSequence_flash_st\n\r");
                break;

            // Otherwise, if we are in the pause state, then output that result to the user.
            case flashSequence_pause_st:
                printf("flashSequence_pause_st\n\r");
                break;

            // Otherwise, if we are in the is completed state, then output that result to the user.
            case flashSequence_is_completed_st:
                printf("flashSequence_is_completed_st\n\r");
                break;

            // Otherwise, if we are in the wait for disable state, then output that result to the user.
            case flashSequence_wait_for_disable_st:
                printf("flashSequence_wait_for_disable_st\n\r");
                break;
     }
  }
}

/*********************************************************************************/
/* Function: flashSequence_enable                                                */
/* Purpose: To enable the flash sequence state machine.                          */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void flashSequence_enable()
{
    // Set the enable flag to true.
    enable = true;
}

/*********************************************************************************/
/* Function: flashSequence_disable                                               */
/* Purpose: To disable the flash sequence state machine.                         */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void flashSequence_disable()
{
    // Set the enable flag to false.
    enable = false;
}

/*********************************************************************************/
/* Function: flashSequence_isComplete                                            */
/* Purpose: To return the state of the complete flag.                            */
/* Returns: A boolean value.                                                     */
/*********************************************************************************/
bool flashSequence_isComplete()
{
    return completed;
}

/*********************************************************************************/
/* Function: flashSequence_tick                                                  */
/* Purpose: To run a "tick" for the flash sequence state machine.                */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void flashSequence_tick()
{
    // Switch on the flash sequence current state. This switch statement is for the state machine transitions and Mealy outputs.
    switch (flash_sequence_current_state)
    {
        // If we are in the start state, then jump immediately to the init state.
        case flashSequence_start_st:
            flash_sequence_current_state = flashSequence_init_st;
            break;

        // Otherwise, if we are in the init state.
        case flashSequence_init_st:
            // If the enable flag is true.
            if (enable)
            {
                // Draw a square at the location of the sequence based on our local count value then jump to the flash state.
                simonDisplay_drawSquare(globals_getSequenceValue(count), false);
                flash_sequence_current_state = flashSequence_flash_st;
            }

            // Otherwise, state in the init state.
            else
            {
                flash_sequence_current_state = flashSequence_init_st;
            }

            break;

        // Otherwise, if we are in the flash state.
        case flashSequence_flash_st:
            // If the flash timer is greater than the maximum set flash time.
            if (flash_timer >= FLASH_SEQUENCE_FLASH_TIME)
            {
                // Erase the square at the location of the sequence based on our local count value then jump to the pause state.
                simonDisplay_drawSquare(globals_getSequenceValue(count), true);
                flash_sequence_current_state = flashSequence_pause_st;
            }

            // Otherwise, stay in the flash state.
            else
            {
                flash_sequence_current_state = flashSequence_flash_st;
            }

            break;

        // Otherwise, if we are in the pause state.
        case flashSequence_pause_st:
            // If the pause timer is greater than the maximum set pause time then jump to the is completed state.
            if (pause_timer >= FLASH_SEQUENCE_PAUSE_TIME)
            {
                flash_sequence_current_state = flashSequence_is_completed_st;
            }

            // Otherwise, stay in the pause state.
            else
            {
                flash_sequence_current_state = flashSequence_pause_st;
            }

            break;

        // Otherwise, if we are in the is completed state.
        case flashSequence_is_completed_st:
            // If count is greater than the sequence iteration length minus the increment count.
            if (count >= globals_getSequenceIterationLength() - FLASH_SEQUENCE_INCREMENT_SEQUENCE)
            {
                // Set the completed flag to true and jump to the wait for disable state.
                completed = true;
                flash_sequence_current_state = flashSequence_wait_for_disable_st;
            }

            // Otherwise.
            else
            {
                // Increment the local count and jump to the init state.
                count++;
                flash_sequence_current_state = flashSequence_init_st;
            }

            break;

        // Otherwise, if we are in the wait for disable state.
        case flashSequence_wait_for_disable_st:
            // If the enable flag is set to true then stay in the wait for disable state.
            if (enable)
            {
                flash_sequence_current_state = flashSequence_wait_for_disable_st;
            }

            // Otherwise.
            else
            {
                /// Set the completed flag to false, clear the local counter variable, and then jump to the init state.
                completed = false;
                count = FLASH_SEQUENCE_COUNT_CLEAR;
                flash_sequence_current_state = flashSequence_init_st;
            }

            break;

        // Otherwise, if all else fails, jump to the start state.
        default:
            flash_sequence_current_state = flashSequence_start_st;
            break;
    }

    // Switch on the flash sequence current state. This switch is used for computations completed in the state and Moore outputs.
    switch (flash_sequence_current_state)
    {
        // If we are in the start state.
        case flashSequence_start_st:
            // Set the enable flag to false and clear the local count variable.
            enable = false;
            count = FLASH_SEQUENCE_COUNT_CLEAR;
            break;

        // Otherwise, if we are in the init state.
        case flashSequence_init_st:
            // Clear the flash and pause timer.
            flash_timer = FLASH_SEQUENCE_FLASH_TIME_CLEAR;
            pause_timer = FLASH_SEQUENCE_PAUSE_TIME_CLEAR;
            break;

        // Otherwise, if we are in the flash state, then increment the flash timer.
        case flashSequence_flash_st:
            flash_timer++;
            break;

        // Otherwise, if we are in the pause state, then increment the pause timer.
        case flashSequence_pause_st:
            pause_timer++;
            break;

        // Otherwise, if we are in the is completed state, don't do anything.
        case flashSequence_is_completed_st:
            break;

        // Otherwise, if we are in the wait for disable state, don't do anything.
        case flashSequence_wait_for_disable_st:
            break;

        // Otherwise, if all else fails, don't do anything.
        default:
            break;
    }
}
