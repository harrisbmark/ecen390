/*********************************************************************************/
/* File: buttonHandler.c                                                         */
/* Purpose: To provide the interface between the LCD display and regions used    */
/*          for the "Simon" game.                                                */
/* Author: Mark Harris                                                           */
/* Date: 11-16-16                                                                */
/*********************************************************************************/
#include "buttonHandler.h"
#include "simonDisplay.h"
#include "supportFiles/display.h"

// Necessary defines for the ADC timer.
#define BUTTON_HANDLER_ADC_WAIT_TIME 1
#define BUTTON_HANDLER_ADC_CLEAR_TIME 0

// Enum used to create the states for the button handler.
enum buttonHandler_st_t
{
    buttonHandler_start_st,             // State used to start the state machine.
    buttonHandler_init_st,              // State used to initialize the button handler state machine.
    buttonHandler_wait_for_touch_st,    // State used to wait for a user to touch the LCD.
    buttonHandler_wait_for_adc_st,      // State used to wait for the ADC to settle.
    buttonHandler_is_touched_st,        // State used to determine if the LCD display is touched.
    buttonHandler_wait_for_disable_st   // State used to wait for the calling function to disable the button handler.
} button_handler_current_state = buttonHandler_start_st;

// Global variables used for the flags in the state machine.
static bool enable;
static bool is_released;

// Global variable used for the timer in the state machine.
static uint8_t adc_timer;

/*********************************************************************************/
/* Function: buttonHandler_getRegionNumber                                       */
/* Purpose: To return the region where the LCD is touched.                       */
/* Returns: An 8 bit integer value.                                              */
/*********************************************************************************/
uint8_t buttonHandler_getRegionNumber()
{
    // Create and set the x, y, and intensity of the touch display to 0.
    int16_t display_touch_x = 0;
    int16_t display_touch_y = 0;
    uint8_t display_touch_intensity = 0;

    // Get the touched points from the display.
    display_getTouchedPoint(&display_touch_x, &display_touch_y, &display_touch_intensity);

    // Return the computed region on the LCD.
    return simonDisplay_computeRegionNumber(display_touch_x, display_touch_y);
}

/*********************************************************************************/
/* Function: buttonHandler_enable                                                */
/* Purpose: To enable the state machine.                                         */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void buttonHandler_enable()
{
    // Set enable to true.
    enable = true;
}

/*********************************************************************************/
/* Function: buttonHandler_disable                                               */
/* Purpose: To disable the state machine.                                        */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void buttonHandler_disable()
{
    // Set enable to false.
    enable = false;
}

/*********************************************************************************/
/* Function: buttonHandler_releaseDetected                                       */
/* Purpose: To return if a touch release on the LCD screen has been detected.    */
/* Returns: A boolean value.                                                     */
/*********************************************************************************/
bool buttonHandler_releaseDetected()
{
    // Return the state of the released flag.
    return is_released;
}

/*********************************************************************************/
/* Function: buttonHandler_isTouched                                             */
/* Purpose: To return if the user is currently touching the display.             */
/* Returns: A boolean value.                                                     */
/*********************************************************************************/
bool buttonHandler_isTouched()
{
    // Return if the display is touched.
    return display_isTouched();
}

/*********************************************************************************/
/* Function: buttonHandler_tick                                                  */
/* Purpose: To run a "tick" for the button handler state machine.                */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void buttonHandler_tick()
{
    // Switch on the button handler current state. This switch statement is for the state machine transitions and Mealy outputs.
    switch (button_handler_current_state)
    {
        // If we are in the start state, then jump immediately to the init state.
        case buttonHandler_start_st:
            button_handler_current_state = buttonHandler_init_st;
            break;

        // Otherwise, if we are in the init state.
        case buttonHandler_init_st:
            // If the state machine is told to enable then jump to the wait for touch state.
            if (enable)
            {
                button_handler_current_state = buttonHandler_wait_for_touch_st;
            }

            // Otherwise, stay in the init state.
            else
            {
                button_handler_current_state = buttonHandler_init_st;
            }

            break;

        // Otherwise, if we are in the wait for touch state.
        case buttonHandler_wait_for_touch_st:
            // If the display has detected a touch.
            if (display_isTouched())
            {
                // Clear the old touch data and jump to the wait for ADC state.
                display_clearOldTouchData();
                button_handler_current_state = buttonHandler_wait_for_adc_st;
            }

            // Otherwise, stay in the wait for touch state.
            else
            {
                button_handler_current_state = buttonHandler_wait_for_touch_st;
            }

            break;

        // Otherwise, if we are in the wait for ADC state.
        case buttonHandler_wait_for_adc_st:
            // If the timer for the ADC is greater than the set maximum time and the display is still touched.
            if (adc_timer >= BUTTON_HANDLER_ADC_WAIT_TIME && display_isTouched())
            {
                // Draw a square at the touched position and jump to the is touched state.
                simonDisplay_drawSquare(buttonHandler_getRegionNumber(), false);
                button_handler_current_state = buttonHandler_is_touched_st;
            }

            // Otherwise, if the ADC timer is greater than the set maximum time and the display is no longer detected as touched, jump to the wait for touch state.
            else if (adc_timer >= BUTTON_HANDLER_ADC_WAIT_TIME && !display_isTouched())
            {
                button_handler_current_state = buttonHandler_wait_for_touch_st;
            }

            // Otherwise, stay in the wait for ADC state.
            else
            {
                button_handler_current_state = buttonHandler_wait_for_adc_st;
            }

            break;

        // Otherwise, if we are in the is touched state.
        case buttonHandler_is_touched_st:
            // If the display is detected as being touched, stay in the is touched state.
            if (display_isTouched())
            {
                button_handler_current_state = buttonHandler_is_touched_st;
            }

            // Otherwise.
            else
            {
                // Set the released flag to true, erase the bigger square, redraw the button, and then jump to the wait for disable state.
                is_released = true;
                simonDisplay_drawSquare(buttonHandler_getRegionNumber(), true);
                simonDisplay_drawButton(buttonHandler_getRegionNumber(), false);
                button_handler_current_state = buttonHandler_wait_for_disable_st;
            }

            break;

        // Otherwise, if we are in the wait for disable state.
        case buttonHandler_wait_for_disable_st:
            // If the enable flag is true.
            if (enable)
            {
                // Set the released flag to false and jump to the init state.
                is_released = false;
                button_handler_current_state = buttonHandler_init_st;
            }

            // Otherwise, stay in the wait for disable state.
            else
            {
                button_handler_current_state = buttonHandler_wait_for_disable_st;
            }

            break;

        // Otherwise, if all else fails, jump to the start state.
        default:
            button_handler_current_state = buttonHandler_start_st;
            break;
    }

    // Switch on the button handler current state. This switch is used for computations completed in the state and Moore outputs.
    switch (button_handler_current_state)
    {
        // If we are in the start state, initialize enable to false.
        case buttonHandler_start_st:
            enable = false;
            break;

        // Otherwise, if we are in the init state.
        case buttonHandler_init_st:
            // Set the released flag to false and clear the ADC timer.
            is_released = false;
            adc_timer = BUTTON_HANDLER_ADC_CLEAR_TIME;
            break;

        // Otherwise, if we are in the wait for touch state, don't do anything.
        case buttonHandler_wait_for_touch_st:
            break;

        // Otherwise, if we are in the wait for ADC state, then increment the ADC timer.
        case buttonHandler_wait_for_adc_st:
            adc_timer++;

        // Otherwise, if we are in the the is touched state, don't do anything.
        case buttonHandler_is_touched_st:
            break;

        // Otherwise, if we are in the wait for disable state, don't do anything.
        case buttonHandler_wait_for_disable_st:
            break;

        // Otherwise, if all else fails, don't do anything.
        default:
            break;
    }
}
