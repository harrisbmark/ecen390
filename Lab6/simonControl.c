/*********************************************************************************/
/* File: simonControl.c                                                          */
/* Purpose: To provide the code necessary to run the Simon game using the        */
/*          provided and designed state machines.                                */
/* Author: Mark Harris                                                           */
/* Date: 11-16-16                                                                */
/*********************************************************************************/
#include "simonControl.h"
#include "simonDisplay.h"
#include "flashSequence.h"
#include "verifySequence.h"
#include "buttonHandler.h"
#include "globals.h"
#include "supportFiles/display.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

// Necessary defines to set the maximum timer wait length.
#define SIMON_CONTROL_ADC_WAIT_TIME 1
#define SIMON_CONTROL_CORRECT_SEQUENCE_TIME 60
#define SIMON_CONTROL_CONTINUE_WAIT_TIME 60
#define SIMON_CONTROL_SHOW_MAX_SEQUENCE_WAIT_TIME 80

// Necessary defines to clear the timers.
#define SIMON_CONTROL_IDLE_TIMER_CLEAR 0
#define SIMON_CONTROL_ADC_TIMER_CLEAR 0
#define SIMON_CONTROL_CORRECT_SEQUENCE_TIMER_CLEAR 0
#define SIMON_CONTROL_CONTINUE_TIMER_CLEAR 0
#define SIMON_CONTROL_SHOW_MAX_SEQUENCE_TIMER_CLEAR 0

// Necessary defines to clear the global level variables.
#define SIMON_CONTROL_MAX_LEVEL_CLEAR 4
#define SIMON_CONTROL_CURRENT_LEVEL_CLEAR 1
#define SIMON_CONTROL_TOTAL_LEVEL_CLEAR 0

// Necessary defines to set the square count and the maximum sequence string size.
#define SIMON_CONTROL_SQUARE_COUNT 4
#define SIMON_CONTROL_SEQUENCE_STRING_SIZE 21

// Necessary defines to set the default background color, text color, and text size.
#define SIMON_CONTROL_BACKGROUND_COLOR DISPLAY_BLACK
#define SIMON_CONTROL_TEXT_COLOR DISPLAY_WHITE
#define SIMON_CONTROL_TEXT_SIZE 2

// Necessary defines to set the text, text size and x,y padding for the splash screen title.
#define SIMON_CONTROL_SPLASH_TEXT_H1 "SIMON"
#define SIMON_CONTROL_SPLASH_TEXT_H1_SIZE 5
#define SIMON_CONTROL_SPLASH_H1_X DISPLAY_WIDTH / 4
#define SIMON_CONTROL_SPLASH_H1_Y DISPLAY_HEIGHT / 2.5

// Necessary defines to set the text, text size and, x,y padding for the splash screen subtitle.
#define SIMON_CONTROL_SPLASH_TEXT_H2 "Touch to start"
#define SIMON_CONTROL_SPLASH_TEXT_H2_SZIE 3
#define SIMON_CONTROL_SPLASH_H2_X DISPLAY_WIDTH / 9
#define SIMON_CONTROL_SPLASH_H2_Y DISPLAY_HEIGHT / 1.75

// Necessary defines to set the text, text size and, x,y padding for the "correct" message text.
#define SIMON_CONTROL_CORRECT_MESSAGE_TEXT "Noice!"
#define SIMON_CONTROL_CORRECT_MESSAGE_TEXT_SIZE 3
#define SIMON_CONTROL_CORRECT_MESSAGE_X DISPLAY_WIDTH / 3
#define SIMON_CONTROL_CORRECT_MESSAGE_Y DISPLAY_HEIGHT / 2.75

// Necessary defines to set the text, text size, and x,y padding for the continue message.
#define SIMON_CONTROL_CONTINUE_MESSAGE_TEXT "Touch for new level"
#define SIMON_CONTROL_CONTINUE_MESSAGE_TEXT_SIZE 2.5
#define SIMON_CONTROL_CONTINUE_MESSAGE_X DISPLAY_WIDTH / 8
#define SIMON_CONTROL_CONTINUE_MESSAGE_Y DISPLAY_HEIGHT / 2.5

// Necessary defines to set the text, text size, and x,y padding for the longest sequence message.
#define SIMON_CONTROL_LONGEST_SEQUENCE_MESSAGE_TEXT "Longest sequence: %d"
#define SIMON_CONTROL_LONGEST_SEQUENCE_MESSAGE_TEXT_SIZE 2.5
#define SIMON_CONTROL_LONGEST_SEQUENCE_MESSAGE_X DISPLAY_WIDTH / 8
#define SIMON_CONTROL_LONGEST_SEQUENCE_MESSAGE_Y DISPLAY_HEIGHT / 2.5

// Enum used to create the states for the Simon game.
enum simonControl_st_t
{
    simonControl_start_st,                      // State used to start the state machine.
    simonControl_splash_screen_st,              // State used to show a "splash screen."
    simonControl_splash_screen_wait_adc_st,     // State used to wait until the ADC has settled.
    simonControl_init_st,                       // State used to initialize the state machine.
    simonControl_wait_flash_sequence_st,        // State used to wait until the flash sequence is done.
    simonControl_wait_verify_sequence_st,       // State used to wait until the verify sequence function is complete.
    simonControl_is_game_over_st,               // Sate used to see if the game (level) is over or not.
    simonControl_correct_sequence_st,           // State used to verify if the sequence was correct or not.
    simonControl_continue_st,                   // State used to wait for the user to pick continue or not.
    simonControl_continue_wait_adc_st,          // State used to wait until the ADC has settled.
    simonControl_show_max_sequence_count_st     // State used to show the count of the longest sequence.
} simon_control_current_state = simonControl_start_st;

// Global variable used to create a randomized sequence.
static uint8_t sequence[GLOBALS_MAX_FLASH_SEQUENCE];

// Global variables used for the levels in the Simon game.
static uint8_t max_level;
static uint8_t current_level;
static uint8_t total_level;

// Global variables used for the timers in the state machine.
static uint32_t idle_timer;
static uint8_t adc_timer;
static uint8_t correct_sequence_timer;
static uint8_t continue_timer;
static uint8_t show_max_sequence_timer;

/*********************************************************************************/
/* Function: simonControl_debug_print                                            */
/* Purpose: To print the current state of the state machine for debugging.       */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void simonControl_debug_print()
{
    // Create static variables storing the previous state and if we are on the first pass.
    static simonControl_st_t previous_state;
    static bool firstPass = true;

    // If the previous state is different from the current state or if we are on the first pass.
    if (previous_state != simon_control_current_state || firstPass)
    {
        // Set the first pass equal to false. Set the previous state equal to the current state.
        firstPass = false;
        previous_state = simon_control_current_state;

        // Switch on the current state to show what output to deliver.
        switch(simon_control_current_state)
        {
            // If we are in the start state, then output that result to the user.
            case simonControl_start_st:
                printf("simonControl_start_st\n\r");
                break;

            // Otherwise, if we are in the splash screen state, then output that result to the user.
            case simonControl_splash_screen_st:
                printf("simonControl_splash_screen_st\n\r");
                break;

            // Otherwise, if we are in the splash screen wait ADC state, then output that result to the user.
            case simonControl_splash_screen_wait_adc_st:
                printf("simonControl_splash_screen_wait_adc_st\n\r");
                break;

            // Otherwise, if we are in the init state, then output that result to the user.
            case simonControl_init_st:
                printf("simonControl_init_st\n\r");
                break;

            // Otherwise, if we are in the wait flash sequence state, then output that result to the user.
            case simonControl_wait_flash_sequence_st:
                printf("simonControl_wait_flash_sequence_st\n\r");
                break;

            // Otherwise, if we are in the wait verify sequence state, then output that result to the user.
            case simonControl_wait_verify_sequence_st:
                printf("simonControl_wait_verify_sequence_st\n\r");
                break;

            // Otherwise, if we are in the is game over state, then output that result to the user.
            case simonControl_is_game_over_st:
                printf("simonControl_is_game_over_st\n\r");
                break;

            // Otherwise, if we are in the correct sequence state, then output that result to the user.
            case simonControl_correct_sequence_st:
                printf("simonControl_correct_sequence_st\n\r");
                break;

            // Otherwise, if we are in the continue state, then output that result to the user.
            case simonControl_continue_st:
                printf("simonControl_continue_st\n\r");
                break;

            // Otherwise, if we are in the continue wait ADC state, then output that result to the user.
            case simonControl_continue_wait_adc_st:
                printf("simonControl_continue_wait_adc_st\n\r");
                break;

            // Otherwise, if we are in the show max sequence count state, then output that result to the user.
            case simonControl_show_max_sequence_count_st:
                printf("simonControl_show_max_sequence_count_st\n\r");
                break;
     }
  }
}

/*********************************************************************************/
/* Function: simonControl_initialize_display                                     */
/* Purpose: To initialize the LCD display for the Simon game.                    */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void simonControl_initialize_display()
{
    // Initialize the display, fill the screen with the set background color, and set the default text color and size.
    display_init();
    display_fillScreen(SIMON_CONTROL_BACKGROUND_COLOR);
    display_setTextColor(SIMON_CONTROL_TEXT_COLOR);
    display_setTextSize(SIMON_CONTROL_TEXT_SIZE);
}

/*********************************************************************************/
/* Function: simonControl_draw_all_buttons                                       */
/* Purpose: To draw the buttons for the Simon game on the LCD.                   */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void simonControl_draw_all_buttons()
{
    // Draw the buttons on the LCD screen.
    simonDisplay_drawAllButtons();
}

/*********************************************************************************/
/* Function: simonControl_erase_all_buttons                                      */
/* Purpose: To erase the buttons for the Simon game on the LCD.                  */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void simonControl_erase_all_buttons()
{
    // Erase the buttons on the LCD screen.
    simonDisplay_eraseAllButtons();
}

/*********************************************************************************/
/* Function: simonControl_print_splash_message                                   */
/* Purpose: To print a splash screen message on the LCD screen.                  */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void simonControl_print_splash_message(bool erase)
{
    // Set the text color for the splash screen message.
    display_setTextColor((erase ? SIMON_CONTROL_BACKGROUND_COLOR : SIMON_CONTROL_TEXT_COLOR));

    // Set the cursor, text size, then print the message for the splash screen title.
    display_setCursor(SIMON_CONTROL_SPLASH_H1_X, SIMON_CONTROL_SPLASH_H1_Y);
    display_setTextSize(SIMON_CONTROL_SPLASH_TEXT_H1_SIZE);
    display_println(SIMON_CONTROL_SPLASH_TEXT_H1);

    // Set the cursor, text size, then print the message for the splash screen subtitle.
    display_setCursor(SIMON_CONTROL_SPLASH_H2_X, SIMON_CONTROL_SPLASH_H2_Y);
    display_setTextSize(SIMON_CONTROL_SPLASH_TEXT_H2_SZIE);
    display_println(SIMON_CONTROL_SPLASH_TEXT_H2);
}

/*********************************************************************************/
/* Function: simonControl_print_correct_sequence_message                         */
/* Purpose: To print a message for getting the correct sequence through the      */
/*          "level" on the LCD screen.                                           */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void simonControl_print_correct_sequence_message(bool erase)
{
    // Set the cursor, text size, text color, then print the message for the correct sequence.
    display_setCursor(SIMON_CONTROL_CORRECT_MESSAGE_X, SIMON_CONTROL_CORRECT_MESSAGE_Y);
    display_setTextSize(SIMON_CONTROL_CORRECT_MESSAGE_TEXT_SIZE);
    display_setTextColor((erase ? SIMON_CONTROL_BACKGROUND_COLOR : SIMON_CONTROL_TEXT_COLOR));
    display_println(SIMON_CONTROL_CORRECT_MESSAGE_TEXT);
}

/*********************************************************************************/
/* Function: simonControl_print_continue_message                                 */
/* Purpose: To print a continue message on the LCD screen.                       */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void simonControl_print_continue_message(bool erase)
{
    // Set the cursor, text size, text color, then print a message for whether or not to continue.
    display_setCursor(SIMON_CONTROL_CONTINUE_MESSAGE_X, SIMON_CONTROL_CONTINUE_MESSAGE_Y);
    display_setTextSize(SIMON_CONTROL_CONTINUE_MESSAGE_TEXT_SIZE);
    display_setTextColor((erase ? SIMON_CONTROL_BACKGROUND_COLOR : SIMON_CONTROL_TEXT_COLOR));
    display_print(SIMON_CONTROL_CONTINUE_MESSAGE_TEXT);
}

/*********************************************************************************/
/* Function: simonControl_print_max_sequence_count                               */
/* Purpose: To print the longest sequence recorded for the current Simon game.   */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void simonControl_print_max_sequence_count(bool erase)
{
    // Set the string used to print the longest sequence (using the total level variable).
    char sequence_string[SIMON_CONTROL_SEQUENCE_STRING_SIZE];
    sprintf(sequence_string, SIMON_CONTROL_LONGEST_SEQUENCE_MESSAGE_TEXT, total_level);

    // Set the cursor, text size, text color, then print the longest sequence message.
    display_setCursor(SIMON_CONTROL_LONGEST_SEQUENCE_MESSAGE_X, SIMON_CONTROL_LONGEST_SEQUENCE_MESSAGE_Y);
    display_setTextSize(SIMON_CONTROL_LONGEST_SEQUENCE_MESSAGE_TEXT_SIZE);
    display_setTextColor((erase ? SIMON_CONTROL_BACKGROUND_COLOR : SIMON_CONTROL_TEXT_COLOR));
    display_print(sequence_string);
}

/*********************************************************************************/
/* Function: simonControl_generate_random_sequence                               */
/* Purpose: To generate a random sequence for the Simon game.                    */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void simonControl_generate_random_sequence(uint32_t passed_seed)
{
    // Seed the rand function with a passed value.
    srand(passed_seed);

    // For loop to iterate through the global sequence variable.
    for (uint16_t i = 0; i < GLOBALS_MAX_FLASH_SEQUENCE; i++)
    {
        // Set the current index in the sequence to a random integer modulated to the square count.
        sequence[i] = rand() % SIMON_CONTROL_SQUARE_COUNT;
    }

    // Set the globals sequence to our new, randomly generated sequence.
    globals_setSequence(sequence, max_level);
}

/*********************************************************************************/
/* Function: simonControl_tick                                                   */
/* Purpose: To run a "tick" for the Simon state machine.                         */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void simonControl_tick()
{
    // Switch on the Simon control current state. This switch statement is for the state machine transitions and Mealy outputs.
    switch(simon_control_current_state)
    {
        // If we are in the start state.
        case simonControl_start_st:
            // Print the splash message and jump to the splash screen state.
            simonControl_print_splash_message(false);
            simon_control_current_state = simonControl_splash_screen_st;
            break;

        // Otherwise, if we are in the splash screen state.
        case simonControl_splash_screen_st:
            // If the display has been touched.
            if (display_isTouched())
            {
                // Clear the old touch data and jump to the splash screen wait ADC state.
                display_clearOldTouchData();
                simon_control_current_state = simonControl_splash_screen_wait_adc_st;
            }

            // Otherwise, stay in the splash screen state.
            else
            {
                simon_control_current_state = simonControl_splash_screen_st;
            }

            break;

        // Otherwise, if we are in the splash screen wait ADC state.
        case simonControl_splash_screen_wait_adc_st:
            // If the ADC timer has reached the set maximum value and the display is still reported as being touched.
            if (adc_timer >= SIMON_CONTROL_ADC_WAIT_TIME && display_isTouched())
            {
                // Clear the maximum level, erase the splash screen message, clear the old touch data, enable flash sequence, and jump to the init state.
                max_level = SIMON_CONTROL_MAX_LEVEL_CLEAR;
                simonControl_print_splash_message(true);
                display_clearOldTouchData();
                flashSequence_enable();
                simon_control_current_state = simonControl_init_st;
            }

            // Otherwise, if the ADC timer has reached the set maximum value and the display is no longer being touched.
            else if (adc_timer >= SIMON_CONTROL_ADC_WAIT_TIME && !display_isTouched())
            {
                // Clear the ADC timer and jump back to the splash screen state.
                adc_timer = SIMON_CONTROL_ADC_TIMER_CLEAR;
                simon_control_current_state = simonControl_splash_screen_st;
            }

            // Otherwise, stay in the splash screen wait ADC state.
            else
            {
                simon_control_current_state = simonControl_splash_screen_wait_adc_st;
            }

            break;

        // Otherwise, if we are in the init state.
        case simonControl_init_st:
            // Generate a random sequence and jump to the wait flash sequence state.
            simonControl_generate_random_sequence(idle_timer);
            simon_control_current_state = simonControl_wait_flash_sequence_st;
            break;

        // Otherwise, if we are in the wait flash sequence state.
        case simonControl_wait_flash_sequence_st:
            // If the flash sequence reports that it is complete.
            if (flashSequence_isComplete())
            {
                // Disable the flash sequence, run a tick, then enable it again.
                flashSequence_disable();
                flashSequence_tick();
                flashSequence_enable();

                // Draw all the buttons on the LCD screen then jump to the wait verify sequence state.
                simonDisplay_drawAllButtons();
                simon_control_current_state = simonControl_wait_verify_sequence_st;
            }

            // Otherwise, stay in the wait flash sequence state.
            else
            {
                simon_control_current_state = simonControl_wait_flash_sequence_st;
            }

            break;

        // Otherwise, if we are in the wait verify sequence state.
        case simonControl_wait_verify_sequence_st:
            // If the verify sequence reports that it is complete.
            if (verifySequence_isComplete())
            {
                // Erase all the buttons and jump to the is game over state.
                simonControl_erase_all_buttons();
                simon_control_current_state = simonControl_is_game_over_st;
            }

            // Otherwise, stay in the wait verify sequence state.
            else
            {
                simon_control_current_state = simonControl_wait_verify_sequence_st;
            }

            break;

        // Otherwise, if we are in the is game over state.
        case simonControl_is_game_over_st:
            // If we have any errors.
            if (verifySequence_isUserInputError() || verifySequence_isTimeOutError())
            {
                // Disable the verify sequence, tick the verify sequence, then enable it again.
                verifySequence_disable();
                verifySequence_tick();
                verifySequence_enable();

                // Print the longest sequence to the user and jump to the show max sequence count state.
                simonControl_print_max_sequence_count(false);
                simon_control_current_state = simonControl_show_max_sequence_count_st;
            }

            // Otherwise, if the current level is greater than the max level.
            else if (current_level >= max_level)
            {
                // Make the total level equal to the current level if the current level is higher.
                if (current_level > total_level)
                {
                    total_level = current_level;
                }

                // Disable the verify sequence, tick the verify sequence, then enable it again.
                verifySequence_disable();
                verifySequence_tick();
                verifySequence_enable();

                // Print the correct sequences for the level have been input and jump to the correct sequence state.
                simonControl_print_correct_sequence_message(false);
                simon_control_current_state = simonControl_correct_sequence_st;
            }

            // Otherwise.
            else
            {
                // Make the total level equal to the current level if the current level is higher.
                if (current_level > total_level)
                {
                    total_level = current_level;
                }

                // Disable the verify sequence, tick the verify sequence, then enable it again.
                verifySequence_disable();
                verifySequence_tick();
                verifySequence_enable();

                // Increase the globals iteration length then jump to the wait flash sequence state.
                globals_setSequenceIterationLength(++current_level);
                simon_control_current_state = simonControl_wait_flash_sequence_st;
            }

            break;

        // Otherwise, if we are in the correct sequence state.
        case simonControl_correct_sequence_st:
            // If the correct sequence timer has reached the set maximum correct sequence timer value.
            if (correct_sequence_timer >= SIMON_CONTROL_CORRECT_SEQUENCE_TIME)
            {
                // Erase the correct sequence message, print the continue message, then jump to the continue state.
                simonControl_print_correct_sequence_message(true);
                simonControl_print_continue_message(false);
                simon_control_current_state = simonControl_continue_st;
            }

            // Otherwise, stay in the correct sequence state.
            else
            {
                simon_control_current_state = simonControl_correct_sequence_st;
            }

            break;

        // Otherwise, if we are in the continue state.
        case simonControl_continue_st:
            // If the continue timer has reached the set maximum control timer wait time.
            if (continue_timer >= SIMON_CONTROL_CONTINUE_WAIT_TIME)
            {
                // Erase the continue message, print the max sequence count, and jump to the show max sequence count state.
                simonControl_print_continue_message(true);
                simonControl_print_max_sequence_count(false);
                simon_control_current_state = simonControl_show_max_sequence_count_st;
            }

            // Otherwise, if the display is touched.
            else if (display_isTouched())
            {
                // Erase the continue message, clear the old touch data, and jump to the continue wait ADC state.
                simonControl_print_continue_message(true);
                display_clearOldTouchData();
                simon_control_current_state = simonControl_continue_wait_adc_st;
            }

            // Otherwise, stay in the continue state.
            else
            {
                simon_control_current_state = simonControl_continue_st;
            }

            break;

        // Otherwise, if we are in the continue wait ADC state.
        case simonControl_continue_wait_adc_st:
            // If the ADC timer has reached the set maximum ADC wait time and the display is still reported as touched.
            if (adc_timer >= SIMON_CONTROL_ADC_WAIT_TIME && buttonHandler_isTouched())
            {
                // Increase the maximum level, clear the old touch data, enable flash sequence, and jump to the init state.
                max_level++;
                display_clearOldTouchData();
                flashSequence_enable();
                simon_control_current_state = simonControl_init_st;
            }

            // Otherwise, if the ADC timer has reached the set maximum ADC wait time and the display is not touched.
            else if (adc_timer >= SIMON_CONTROL_ADC_WAIT_TIME && !buttonHandler_isTouched())
            {
                // Clear the old touch data and jump back to the continue state.
                display_clearOldTouchData();
                simon_control_current_state = simonControl_continue_st;
            }

            // Otherwise, stay in the continue wait ADC state.
            else
            {
                simon_control_current_state = simonControl_continue_wait_adc_st;
            }

            break;

        // Otherwise, if we are in the show max sequence count state.
        case simonControl_show_max_sequence_count_st:
            // If the show max sequence timer has reached the set maximum show max sequence wait time.
            if (show_max_sequence_timer >= SIMON_CONTROL_SHOW_MAX_SEQUENCE_WAIT_TIME)
            {
                // Clear the ADC timer, erase the max sequence count, print the splash message, erase the total level count, then jump to the splash screen state.
                adc_timer = SIMON_CONTROL_ADC_TIMER_CLEAR;
                simonControl_print_max_sequence_count(true);
                simonControl_print_splash_message(false);
                total_level = SIMON_CONTROL_TOTAL_LEVEL_CLEAR;
                simon_control_current_state = simonControl_splash_screen_st;
            }

            // Otherwise, stay in the show max sequence count state.
            else
            {
                simon_control_current_state = simonControl_show_max_sequence_count_st;
            }

            break;

        // Otherwise, if all else fails, go to the start state.
        default:
            simon_control_current_state = simonControl_start_st;
            break;
    }

    // Switch on the Simon control current state. This switch is used for computations completed in the state and Moore outputs.
    switch(simon_control_current_state)
    {
        // If we are in the start state.
        case simonControl_start_st:
            // Clear the idle timer, ADC timer, and the total level count.
            idle_timer = SIMON_CONTROL_IDLE_TIMER_CLEAR;
            adc_timer = SIMON_CONTROL_ADC_TIMER_CLEAR;
            total_level = SIMON_CONTROL_TOTAL_LEVEL_CLEAR;
            break;

        // Otherwise, if we are in the splash screen state then increment the idle timer.
        case simonControl_splash_screen_st:
            idle_timer++;
            break;

        // Otherwise, if we are in the splash screen wait ADC state.
        case simonControl_splash_screen_wait_adc_st:
            // Increment the idle timer and ADC timer.
            idle_timer++;
            adc_timer++;
            break;

        // Otherwise, if we are in the init state.
        case simonControl_init_st:
            // Clear the ADC timer, the correct sequence timer, and clear the current level (while also passing that value to the globals sequence iteration length).
            adc_timer = SIMON_CONTROL_ADC_TIMER_CLEAR;
            correct_sequence_timer = SIMON_CONTROL_CORRECT_SEQUENCE_TIMER_CLEAR;
            globals_setSequenceIterationLength(current_level = SIMON_CONTROL_CURRENT_LEVEL_CLEAR);

            // Enable the flash sequence and the verify sequence.
            flashSequence_enable();
            verifySequence_enable();
            break;

        // Otherwise, if we are in the wait flash sequence state.
        case simonControl_wait_flash_sequence_st:
            // Increment the idle timer and tick the flash sequence.
            idle_timer++;
            flashSequence_tick();
            break;

        // Otherwise, if we are in the wait verify sequence state.
        case simonControl_wait_verify_sequence_st:
            idle_timer++;
            verifySequence_tick();
            buttonHandler_tick();
            break;

        // Otherwise, if we are in the game over state.
        case simonControl_is_game_over_st:
            // Clear the continue timer and the show max sequence timer.
            continue_timer = SIMON_CONTROL_CONTINUE_TIMER_CLEAR;
            show_max_sequence_timer = SIMON_CONTROL_SHOW_MAX_SEQUENCE_TIMER_CLEAR;
            break;

        // Otherwise, if we are in the correct sequence state, then increment the correct sequence timer.
        case simonControl_correct_sequence_st:
            correct_sequence_timer++;
            break;

        // Otherwise, if we are in the continue state.
        case simonControl_continue_st:
            // Increment the idle timer and the continue timer.
            idle_timer++;
            continue_timer++;
            break;

        // Otherwise, if we are in the continue wait ADC state.
        case simonControl_continue_wait_adc_st:
            // Increment the idle timer and the ADC timer.
            idle_timer++;
            adc_timer++;
            break;

        // Otherwise, if we are in the show max sequence count state.
        case simonControl_show_max_sequence_count_st:
            // Increment the idle timer and the show max sequence timer.
            idle_timer++;
            show_max_sequence_timer++;
            break;

        // Otherwise, if all else fails, don't do anything.
        default:
            break;
    }
}
