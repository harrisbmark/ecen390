/*********************************************************************************/
/* File: verifySequence.c                                                        */
/* Purpose: To provide the code necessary to verify an input sequence.           */
/* Author: Mark Harris                                                           */
/* Date: 11-16-16                                                                */
/*********************************************************************************/
#include "verifySequence.h"
#include "buttonHandler.h"
#include "globals.h"
#include <stdint.h>
#include <stdio.h>

// Necessary define for the length of time to wait for input from the user.
#define VERIFY_SEQUENCE_WAIT_TIME 20

// Necessary defines to clear the wait timer and counter.
#define VERIFY_SEQUENCE_WAIT_TIME_CLEAR 0
#define VERIFY_SEQUENCE_COUNT_CLEAR 0

// Enum used to create the states for the verify sequence state machine.
enum verifySequence_st_t
{
    verifySequence_start_st,            // State used to start the state machine.
    verifySequence_init_st,             // State used to initialize the variables in the state machine.
    verifySequence_get_sequence_st,     // State used to get the next sequence from the user.
    verifySequence_completed_st         // State used to know when the state machine has completed.
} verify_sequence_current_state = verifySequence_start_st;

// Global variables used for flags in the verify sequence state machine.
static bool enable;
static bool time_out_error;
static bool user_input_error;
static bool completed;

// Global variables used for a timer and counter in the verify sequence state machine.
static uint8_t wait_timer;
static uint8_t count;

/*********************************************************************************/
/* Function: verifySequence_debug_print                                          */
/* Purpose: To print the current state of the state machine for debugging.       */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void verifySequence_debug_print()
{
    // Create static variables storing the previous state and if we are on the first pass.
    static verifySequence_st_t previous_state;
    static bool firstPass = true;

    // If the previous state is different from the current state or if we are on the first pass.
    if (previous_state != verify_sequence_current_state || firstPass)
    {
        // Set the first pass equal to false. Set the previous state equal to the current state.
        firstPass = false;
        previous_state = verify_sequence_current_state;

        // Switch on the current state to show what output to deliver.
        switch(verify_sequence_current_state)
        {
            // If we are in the start state, then output that result to the user.
            case verifySequence_start_st:
                printf("verifySequence_start_st\n\r");
                break;

            // Otherwise, if we are in the init state, then output that result to the user.
            case verifySequence_init_st:
                printf("verifySequence_init_st\n\r");
                break;

            // Otherwise, if we are in the get sequence state, then output that result to the user.
            case verifySequence_get_sequence_st:
                printf("verifySequence_get_sequence_st\n\r");
                break;

            // Otherwise, if we are in the completed state, then output that result to the user.
            case verifySequence_completed_st:
                printf("verifySequence_completed_st\n\r");
                break;
     }
  }
}

/*********************************************************************************/
/* Function: verifySequence_enable                                               */
/* Purpose: To enable the verify sequence state machine.                         */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void verifySequence_enable()
{
    // Set the enable flag to true.
    enable = true;
}

/*********************************************************************************/
/* Function: verifySequence_disable                                              */
/* Purpose: To disable the verify sequence state machine.                        */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void verifySequence_disable()
{
    // Set the enable flag to false.
    enable = false;
}

/*********************************************************************************/
/* Function: verifySequence_isTimeOutError                                       */
/* Purpose: To return the state of the time-out flag.                            */
/* Returns: A boolean value.                                                     */
/*********************************************************************************/
bool verifySequence_isTimeOutError()
{
    // Return the state of the time out error flag.
    return time_out_error;
}

/*********************************************************************************/
/* Function: verifySequence_isUserInputError                                     */
/* Purpose: To return the state of the user input error flag.                    */
/* Returns: A boolean value.                                                     */
/*********************************************************************************/
bool verifySequence_isUserInputError()
{
    // Return the state of the user input error flag.
    return user_input_error;
}

/*********************************************************************************/
/* Function: verifySequence_isComplete                                           */
/* Purpose: To return the state of the completed flag.                           */
/* Returns: A boolean value.                                                     */
/*********************************************************************************/
bool verifySequence_isComplete()
{
    // Return the state of the completed flag.
    return completed;
}

/*********************************************************************************/
/* Function: verifySequence_tick                                                 */
/* Purpose: To run a "tick" for the verify sequence state machine.               */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void verifySequence_tick()
{
    // Switch on the verify sequence current state. This switch statement is for the state machine transitions and Mealy outputs.
    switch (verify_sequence_current_state)
    {
        // If we are in the start state, then jump immediately to the init state.
        case verifySequence_start_st:
            verify_sequence_current_state = verifySequence_init_st;
            break;

        // Otherwise, if we are in the init state.
        case verifySequence_init_st:
            // If the enable flag is true, then jump to the get sequence state.
            if (enable)
            {
                verify_sequence_current_state = verifySequence_get_sequence_st;
            }

            // Otherwise, stay in the init state.
            else
            {
                verify_sequence_current_state = verifySequence_init_st;
            }

            break;

        // Otherwise, if we are in the get sequence state.
        case verifySequence_get_sequence_st:
            // If the wait timer has reached the set maximum wait time and the display isn't being touched.
            if (wait_timer >= VERIFY_SEQUENCE_WAIT_TIME && !buttonHandler_isTouched())
            {
                // Set the time out error flag to true and jump to the completed state.
                time_out_error = true;
                verify_sequence_current_state = verifySequence_completed_st;
            }

            // Otherwise, if the wait timer has reached the set maximum wait time, a release has not been detected, and the display is being touched..
            else if (wait_timer >= VERIFY_SEQUENCE_WAIT_TIME && !buttonHandler_releaseDetected() && buttonHandler_isTouched())
            {
                // Set the time out error flag to true and stay in the get sequence state.
                time_out_error = true;
                verify_sequence_current_state = verifySequence_get_sequence_st;
            }

            // Otherwise, if the count has reached the set iteration length, then jump to the completed state.
            else if (count >= globals_getSequenceIterationLength())
            {
                verify_sequence_current_state = verifySequence_completed_st;
            }

            // Otherwise, if a release was detected.
            else if (buttonHandler_releaseDetected())
            {
                // Clear the wait timer and disable the button handler.
                wait_timer = VERIFY_SEQUENCE_WAIT_TIME_CLEAR;
                buttonHandler_disable();

                // If the region number is not equal to the sequence value value at the current count, then set the user input error flag to true.
                if (buttonHandler_getRegionNumber() != globals_getSequenceValue(count++))
                {
                    user_input_error = true;
                }

                // Stay in the get sequence state.
                verify_sequence_current_state = verifySequence_get_sequence_st;
            }

            // Otherwise, stay in the get sequence state.
            else
            {
                verify_sequence_current_state = verifySequence_get_sequence_st;
            }

            break;

        // Otherwise, if we are in the completed state.
        case verifySequence_completed_st:
            // If the enable flag is true, then stay in the completed state.
            if (enable)
            {
                verify_sequence_current_state = verifySequence_completed_st;
            }

            // Otherwise, jump to the init state.
            else
            {
                verify_sequence_current_state = verifySequence_init_st;
            }

            break;

        // Otherwise, if all else fails, jump to the start state.
        default:
            verify_sequence_current_state = verifySequence_start_st;
            break;
    }

    // Switch on the verify sequence current state. This switch is used for computations completed in the state and Moore outputs.
    switch (verify_sequence_current_state)
    {
        // If we aree in the start state, then initialize the enable flag to false.
        case verifySequence_start_st:
            enable = false;
            break;

        // Otherwise, if we are in the init state.
        case verifySequence_init_st:
            // Clear the timers and set the flags to false.
            wait_timer = VERIFY_SEQUENCE_WAIT_TIME_CLEAR;
            count = VERIFY_SEQUENCE_COUNT_CLEAR;
            completed = false;
            time_out_error = false;
            user_input_error = false;
            break;

        // Otherwise, if we are in the get sequence state.
        case verifySequence_get_sequence_st:
            // Increment the wait timer and enable the button handler.
            wait_timer++;
            buttonHandler_enable();
            break;

        // Otherwise, if we are in the completed state, then set the completed flag to true.
        case verifySequence_completed_st:
            completed = true;
            break;

        // Otherwise, if all else fails, don't do anything.
        default:
            break;
    }
}
