/*********************************************************************************/
/* File: clockDisplay.c                                                          */
/* Purpose: To provide the function definitions to output the display of the     */
/*          clock.                                                               */
/* Author: Mark Harris                                                           */
/* Date: 10-05-16                                                                */
/*********************************************************************************/

// Required includes for the program.
#include "clockDisplay.h"
#include "supportFiles/display.h"
#include <stdio.h>

// Required defines to set the test period and test count amounts.
#define CLOCK_DISPLAY_TEST_PERIOD 50000000
#define CLOCK_DISPLAY_TEST_COUNT_MIN_SEC 30
#define CLOCK_DISPLAY_TEST_COUNT_HOUR 13
#define CLOCK_DISPLAY_TEST_COUNT_DOUBLE_HOUR 2*CLOCK_DISPLAY_TEST_COUNT_HOUR
#define CLOCK_DISPLAY_TEST_COUNT_DOUBLE_MIN_SEC 2*CLOCK_DISPLAY_TEST_COUNT_MIN_SEC

// Required defines to get the default values used for the display.
#define CLOCK_DISPLAY_HALF_DISPLAY DISPLAY_HEIGHT/2
#define CLOCK_DISPLAY_COLOR DISPLAY_GREEN
#define CLOCK_DISPLAY_BACKGROUND_COLOR DISPLAY_BLACK
#define CLOCK_DISPLAY_TEXT_SIZE 6

// Required defines for the display to know where and how many triangles to display.
#define CLOCK_DISPLAY_FIRST_TRIANGLE_OFFSET -1
#define CLOCK_DISPLAY_LAST_TRIANGLE_OFFSET 1

// Required defines fo the characters on the display.
#define CLOCK_DISPLAY_CHARACTER_HEIGHT CLOCK_DISPLAY_TEXT_SIZE*12
#define CLOCK_DISPLAY_CHARACTER_WIDTH CLOCK_DISPLAY_TEXT_SIZE*5
#define CLOCK_DISPLAY_CHARACTER_HALF_HEIGHT CLOCK_DISPLAY_CHARACTER_HEIGHT/2
#define CLOCK_DISPLAY_CHARACTER_HALF_WIDTH CLOCK_DISPLAY_CHARACTER_WIDTH/2
#define CLOCK_DISPLAY_CHARACTER_PADDING_Y 2.25*CLOCK_DISPLAY_PADDING_Y
#define CLOCK_DISPLAY_CHARACTER_PADDING_X (CLOCK_DISPLAY_CHARACTER_WIDTH + CLOCK_DISPLAY_TEXT_SIZE)

// Required defines for the triangle position and size on the display.
#define CLOCK_DISPLAY_TRIANGLE_CENTER_X DISPLAY_WIDTH/2
#define CLOCK_DISPLAY_TRIANGLE_CENTER_Y DISPLAY_HEIGHT/2
#define CLOCK_DISPLAY_TRIANGLE_HEIGHT 0.5*CLOCK_DISPLAY_CHARACTER_HEIGHT
#define CLOCK_DISPLAY_TRIANGLE_WIDTH 2.25*CLOCK_DISPLAY_CHARACTER_WIDTH
#define CLOCK_DISPLAY_TRIANGLE_HALF_HEIGHT CLOCK_DISPLAY_TRIANGLE_HEIGHT/2
#define CLOCK_DISPLAY_TRIANGLE_HALF_WIDTH CLOCK_DISPLAY_TRIANGLE_WIDTH/2

// Required defines for the padding on the display.
#define CLOCK_DISPLAY_PADDING_Y CLOCK_DISPLAY_TEXT_SIZE
#define CLOCK_DISPLAY_PADDING_X (CLOCK_DISPLAY_CHARACTER_WIDTH + 1.5*CLOCK_DISPLAY_TEXT_SIZE)

// Required defines for the time size and positions for the clock display.
#define CLOCK_DISPLAY_TIME_CHAR_SIZE 1
#define CLOCK_DISPLAY_TIME_STRING_SIZE 8
#define CLOCK_DISPLAY_TIME_STRING_WORD 2
#define CLOCK_DISPLAY_TIME_STRING_HOUR 0
#define CLOCK_DISPLAY_TIME_STRING_MINUTE 3
#define CLOCK_DISPLAY_TIME_STRING_SECOND 6
#define CLOCK_DISPLAY_TIME_FIRST_DIGIT 10

// Required defines for the maximum hour, minute, second, and roll overs.
#define CLOCK_DISPLAY_TIME_MAX_HOUR 13
#define CLOCK_DISPLAY_TIME_MAX_MINUTE 60
#define CLOCK_DISPLAY_TIME_MAX_SECOND 60
#define CLOCK_DISPLAY_HOUR_ROLL_OVER 1
#define CLOCK_DISPLAY_ROLL_OVER 0

// Required defines used to know which part of the display is touched.
#define CLOCK_DISPLAY_TOUCH_X_0 DISPLAY_WIDTH/3
#define CLOCK_DISPLAY_TOUCH_X_1 2*DISPLAY_WIDTH/3
#define CLOCK_DISPLAY_TOUCH_X_2 DISPLAY_WIDTH
#define CLOCK_DISPLAY_TOUCH_Y_0 DISPLAY_HEIGHT/2
#define CLOCK_DISPLAY_TOUCH_Y_1 DISPLAY_HEIGHT

// Required defines to set the default value of the clock.
#define CLOCK_DISPLAY_DEFAULT_HOUR 12
#define CLOCK_DISPLAY_DEFAULT_MINUTE 0
#define CLOCK_DISPLAY_DEFAULT_SECOND 0

// Global variables used to keep track of the current and previous time.
int16_t clock_display_current_time_hour = CLOCK_DISPLAY_DEFAULT_HOUR;
int16_t clock_display_current_time_minute = CLOCK_DISPLAY_DEFAULT_MINUTE;
int16_t clock_display_current_time_second = CLOCK_DISPLAY_DEFAULT_SECOND;
int16_t clock_display_last_time_hour = clock_display_current_time_hour;
int16_t clock_display_last_time_minute = clock_display_current_time_minute;
int16_t clock_display_last_time_second = clock_display_current_time_second;

/*********************************************************************************/
/* Function: clockDisplay_print_time                                             */
/* Purpose: To output the current time to the LCD display for the Zybo board.    */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void clockDisplay_print_time(int16_t index, int16_t value, uint32_t color, bool full_word_update)
{
    // Create a character array used to store the values being changed on the display.
    char output_value[CLOCK_DISPLAY_TIME_STRING_WORD];

    // If we are doing a full word update (two characters).
    if (full_word_update)
    {
        // If the position of the display is the hour position.
        if (index == CLOCK_DISPLAY_TIME_STRING_HOUR)
        {
            // Set the output value to have a leading space instead of a 0.
            sprintf(output_value, "%2hd", value);
        }

        // Otherwise, set the output value to have the leading 0.
        else
        {
            sprintf(output_value, "%02hd", value);
        }
    }

    // Otherwise.
    else
    {
        // Get the first digit of the display and add a leading space to it. Put that value in the output value.
        value %= CLOCK_DISPLAY_TIME_FIRST_DIGIT;
        sprintf(output_value, "%2hd", value);
    }

    // Put the output value into the actual character array.
    puts(output_value);

    // Set the text color to the passed color, set the cursor to the correct position, and then output the value calculated.
    display_setTextColor(color);
    display_setCursor((CLOCK_DISPLAY_TRIANGLE_CENTER_X + CLOCK_DISPLAY_FIRST_TRIANGLE_OFFSET*(CLOCK_DISPLAY_TRIANGLE_WIDTH + CLOCK_DISPLAY_PADDING_X) - CLOCK_DISPLAY_TRIANGLE_HALF_WIDTH) + index * CLOCK_DISPLAY_CHARACTER_PADDING_X, (CLOCK_DISPLAY_HALF_DISPLAY - CLOCK_DISPLAY_CHARACTER_HALF_HEIGHT) + CLOCK_DISPLAY_CHARACTER_PADDING_Y);
    display_print(output_value);
}

/*********************************************************************************/
/* Function: clockDisplay_reset                                                  */
/* Purpose: To reset the clock to a specified time based on the defines.         */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void clockDisplay_reset()
{
    // Update the display to get any "last minute values" out.
    clockDisplay_updateTimeDisplay(true);

    // Set the last time values to the current time values.
    clock_display_last_time_hour = clock_display_current_time_hour;
    clock_display_last_time_minute = clock_display_current_time_minute;
    clock_display_last_time_second = clock_display_current_time_second;

    // Set the current time to the default values.
    clock_display_current_time_hour = CLOCK_DISPLAY_DEFAULT_HOUR;
    clock_display_current_time_minute = CLOCK_DISPLAY_DEFAULT_MINUTE;
    clock_display_current_time_second = CLOCK_DISPLAY_DEFAULT_SECOND;
}

/*********************************************************************************/
/* Function: mod                                                                 */
/* Purpose: To get the modulus of two numbers and handling negative values.      */
/* Returns: An int16_t value representing the moduls value of two numbers.       */
/*********************************************************************************/
int16_t mod(int16_t x, int16_t y)
{
    // Create an initial moduls value of 0.
    int16_t modulus = 0;

    // If the the y operand is less than 0.
    if (y < 0)
    {
        // Call a recursive mod function negating the y value (making it positive handling a negative modulus).
        return mod(x, -y);
    }

    // Set the modulus of x and y into the modulus variable.
    modulus = x % y;

    // If our modulus is less than 0.
    if (modulus < 0)
    {
        // Add in the y value back into our modulus value again (this handles the negative modulus).
        modulus += y;
    }

    // Return the modulus value back.
    return modulus;
}

/*********************************************************************************/
/* Function: clockDisplay_inc_dec_second                                         */
/* Purpose: To increment or decrement the seconds value on the clock.            */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void clockDisplay_inc_dec_second(bool increment)
{
    // If the increment flag is true.
    if (increment)
    {
        // Increment the seconds on the clock, resetting back to 0 if we hit the max.
        clock_display_current_time_second = mod(++clock_display_current_time_second, CLOCK_DISPLAY_TIME_MAX_SECOND);
    }

    // Otherwise.
    else
    {
        // Decrement the seconds on the clock, going to the max value if we are at 0.
        clock_display_current_time_second = mod(--clock_display_current_time_second, CLOCK_DISPLAY_TIME_MAX_SECOND);
    }
}

/*********************************************************************************/
/* Function: clockDisplay_inc_dec_minute                                         */
/* Purpose: To increment or decrement the minutes value on the clock.            */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void clockDisplay_inc_dec_minute(bool increment)
{
    // If the increment flag is true.
    if (increment)
    {
        // Increment the minutes on the clock, resetting back to 0 if we hit the max.
        clock_display_current_time_minute = mod(++clock_display_current_time_minute, CLOCK_DISPLAY_TIME_MAX_MINUTE);
    }

    // Otherwise.
    else
    {
        // Decrement the minutes on the clock, going to the max value if we are at 0.
        clock_display_current_time_minute = mod(--clock_display_current_time_minute, CLOCK_DISPLAY_TIME_MAX_MINUTE);
    }
}

/*********************************************************************************/
/* Function: clockDisplay_inc_dec_hour                                           */
/* Purpose: To increment or decrement the hour value on the clock.               */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void clockDisplay_inc_dec_hour(bool increment)
{
    // If the increment flag is true.
    if (increment)
    {
        // Increment the hour of the clock.
        clock_display_current_time_hour = mod(++clock_display_current_time_hour, CLOCK_DISPLAY_TIME_MAX_HOUR);

        // If the clock rolled over to 0 (because of the max value).
        if (clock_display_current_time_hour == CLOCK_DISPLAY_ROLL_OVER)
        {
            // Set the clock to 1.
            clock_display_current_time_hour++;
        }
    }

    // Otherwise.
    else
    {
        // Decrement the hour of the clock.
        clock_display_current_time_hour = mod(--clock_display_current_time_hour, CLOCK_DISPLAY_TIME_MAX_HOUR);

        // If the current hour is now 0.
        if (clock_display_current_time_hour == CLOCK_DISPLAY_ROLL_OVER)
        {
            // Set the clock to it's max value - 1 (12).
            clock_display_current_time_hour = CLOCK_DISPLAY_TIME_MAX_HOUR - 1;
        }
    }
}

/*********************************************************************************/
/* Function: clockDisplay_init                                                   */
/* Purpose: To initialize the display for using it as a clock.                   */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void clockDisplay_init()
{
    // Create a character array containing an initial time string.
    char initial_time[CLOCK_DISPLAY_TIME_STRING_SIZE];

    // Init the display, fill the screen with black, set the text color, then set the text size.
    display_init();
    display_fillScreen(CLOCK_DISPLAY_BACKGROUND_COLOR);
    display_setTextColor(CLOCK_DISPLAY_COLOR);
    display_setTextSize(CLOCK_DISPLAY_TEXT_SIZE);

    // Prepare the initial time with the default time and then set that character array.
    sprintf(initial_time, "%2hd:%02hd:%02hd", clock_display_current_time_hour, clock_display_current_time_minute, clock_display_current_time_second);
    puts(initial_time);

    // Set the cursor to the position necessary to start writing the time, then print the time out to the screen.
    display_setCursor(CLOCK_DISPLAY_TRIANGLE_CENTER_X + CLOCK_DISPLAY_FIRST_TRIANGLE_OFFSET*(CLOCK_DISPLAY_TRIANGLE_WIDTH + CLOCK_DISPLAY_PADDING_X) - CLOCK_DISPLAY_TRIANGLE_HALF_WIDTH, (CLOCK_DISPLAY_HALF_DISPLAY - CLOCK_DISPLAY_CHARACTER_HALF_HEIGHT) + CLOCK_DISPLAY_CHARACTER_PADDING_Y);
    display_print(initial_time);

    // For loop used to iterate over the indexes of the triangles.
    for (int i = CLOCK_DISPLAY_FIRST_TRIANGLE_OFFSET; i <= CLOCK_DISPLAY_LAST_TRIANGLE_OFFSET; i++)
    {
        // Output the triangle to the display based on the current index in the for loop.
        display_fillTriangle(CLOCK_DISPLAY_TRIANGLE_CENTER_X + i*(CLOCK_DISPLAY_TRIANGLE_WIDTH + CLOCK_DISPLAY_PADDING_X), CLOCK_DISPLAY_TRIANGLE_CENTER_Y - CLOCK_DISPLAY_CHARACTER_HALF_HEIGHT - CLOCK_DISPLAY_PADDING_Y - CLOCK_DISPLAY_TRIANGLE_HEIGHT, CLOCK_DISPLAY_TRIANGLE_CENTER_X + i*(CLOCK_DISPLAY_TRIANGLE_WIDTH + CLOCK_DISPLAY_PADDING_X) - CLOCK_DISPLAY_TRIANGLE_HALF_WIDTH, CLOCK_DISPLAY_TRIANGLE_CENTER_Y - CLOCK_DISPLAY_CHARACTER_HALF_HEIGHT, CLOCK_DISPLAY_TRIANGLE_CENTER_X + i*(CLOCK_DISPLAY_TRIANGLE_WIDTH + CLOCK_DISPLAY_PADDING_X) + CLOCK_DISPLAY_TRIANGLE_HALF_WIDTH, CLOCK_DISPLAY_TRIANGLE_CENTER_Y - CLOCK_DISPLAY_CHARACTER_HALF_HEIGHT, CLOCK_DISPLAY_COLOR);
        display_fillTriangle(CLOCK_DISPLAY_TRIANGLE_CENTER_X + i*(CLOCK_DISPLAY_TRIANGLE_WIDTH + CLOCK_DISPLAY_PADDING_X), CLOCK_DISPLAY_TRIANGLE_CENTER_Y + CLOCK_DISPLAY_CHARACTER_HALF_HEIGHT + CLOCK_DISPLAY_PADDING_Y + CLOCK_DISPLAY_TRIANGLE_HEIGHT, CLOCK_DISPLAY_TRIANGLE_CENTER_X + i*(CLOCK_DISPLAY_TRIANGLE_WIDTH + CLOCK_DISPLAY_PADDING_X) - CLOCK_DISPLAY_TRIANGLE_HALF_WIDTH, CLOCK_DISPLAY_TRIANGLE_CENTER_Y + CLOCK_DISPLAY_CHARACTER_HALF_HEIGHT, CLOCK_DISPLAY_TRIANGLE_CENTER_X + i*(CLOCK_DISPLAY_TRIANGLE_WIDTH + CLOCK_DISPLAY_PADDING_X) + CLOCK_DISPLAY_TRIANGLE_HALF_WIDTH, CLOCK_DISPLAY_TRIANGLE_CENTER_Y + CLOCK_DISPLAY_CHARACTER_HALF_HEIGHT, CLOCK_DISPLAY_COLOR);
    }
}

/*********************************************************************************/
/* Function: clockDisplay_updateTimeDisplay                                      */
/* Purpose: To update the time only on the LCD display.                          */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void clockDisplay_updateTimeDisplay(bool forceUpdateAll)
{
    // If the forceUpdateAll flag has been set to true.
    if (forceUpdateAll)
    {
        // Do a full update on the hour (black then green).
        clockDisplay_print_time(CLOCK_DISPLAY_TIME_STRING_HOUR, clock_display_last_time_hour, CLOCK_DISPLAY_BACKGROUND_COLOR, true);
        clockDisplay_print_time(CLOCK_DISPLAY_TIME_STRING_HOUR, clock_display_current_time_hour, CLOCK_DISPLAY_COLOR, true);

        // Do a full update on the minute (black then green).
        clockDisplay_print_time(CLOCK_DISPLAY_TIME_STRING_MINUTE, clock_display_last_time_minute, CLOCK_DISPLAY_BACKGROUND_COLOR, true);
        clockDisplay_print_time(CLOCK_DISPLAY_TIME_STRING_MINUTE, clock_display_current_time_minute, CLOCK_DISPLAY_COLOR, true);

        // Do a full update on teh second (black then green).
        clockDisplay_print_time(CLOCK_DISPLAY_TIME_STRING_SECOND, clock_display_last_time_second, CLOCK_DISPLAY_BACKGROUND_COLOR, true);
        clockDisplay_print_time(CLOCK_DISPLAY_TIME_STRING_SECOND, clock_display_current_time_second, CLOCK_DISPLAY_COLOR, true);
    }

    // Otherwise, if any of the times are different than the previous times.
    else if ((clock_display_current_time_hour != clock_display_last_time_hour) || (clock_display_current_time_minute != clock_display_last_time_minute) || (clock_display_current_time_second != clock_display_last_time_second))
    {
        // If the current hour is not equal to the previous hour.
        if (clock_display_current_time_hour != clock_display_last_time_hour)
        {
            // If the first digit in the hour has changed.
            if (clock_display_current_time_hour / CLOCK_DISPLAY_TIME_FIRST_DIGIT != clock_display_last_time_hour / CLOCK_DISPLAY_TIME_FIRST_DIGIT)
            {
                // Do a full update on the hour to the display (black then green).
                clockDisplay_print_time(CLOCK_DISPLAY_TIME_STRING_HOUR, clock_display_last_time_hour, CLOCK_DISPLAY_BACKGROUND_COLOR, true);
                clockDisplay_print_time(CLOCK_DISPLAY_TIME_STRING_HOUR, clock_display_current_time_hour, CLOCK_DISPLAY_COLOR, true);
            }

            // Otherwise.
            else
            {
                // Only update the second character as it is the only one that has changed (black then green).
                clockDisplay_print_time(CLOCK_DISPLAY_TIME_STRING_HOUR, clock_display_last_time_hour, CLOCK_DISPLAY_BACKGROUND_COLOR, false);
                clockDisplay_print_time(CLOCK_DISPLAY_TIME_STRING_HOUR, clock_display_current_time_hour, CLOCK_DISPLAY_COLOR, false);
            }
        }

        // If the current minute is not equal to the previous minute.
        if (clock_display_current_time_minute != clock_display_last_time_minute)
        {
            // If the first digit in the minute has changed.
            if (clock_display_current_time_minute / CLOCK_DISPLAY_TIME_FIRST_DIGIT != clock_display_last_time_minute / CLOCK_DISPLAY_TIME_FIRST_DIGIT)
            {
                // Do a full update on the minute to the display (black then green).
                clockDisplay_print_time(CLOCK_DISPLAY_TIME_STRING_MINUTE, clock_display_last_time_minute, CLOCK_DISPLAY_BACKGROUND_COLOR, true);
                clockDisplay_print_time(CLOCK_DISPLAY_TIME_STRING_MINUTE, clock_display_current_time_minute, CLOCK_DISPLAY_COLOR, true);
            }

            // Otherwise.
            else
            {
                // Only update the second character as it is the only one that has changed (black then green).
                clockDisplay_print_time(CLOCK_DISPLAY_TIME_STRING_MINUTE, clock_display_last_time_minute, CLOCK_DISPLAY_BACKGROUND_COLOR, false);
                clockDisplay_print_time(CLOCK_DISPLAY_TIME_STRING_MINUTE, clock_display_current_time_minute, CLOCK_DISPLAY_COLOR, false);
            }
        }

        // If the current second is not equal to the previous second.
        if (clock_display_current_time_second != clock_display_last_time_second)
        {
            // If the first digit in the second has changed.
            if (clock_display_current_time_second / CLOCK_DISPLAY_TIME_FIRST_DIGIT != clock_display_last_time_second / CLOCK_DISPLAY_TIME_FIRST_DIGIT)
            {
                // Do a full update on the second to the display (black then green).
                clockDisplay_print_time(CLOCK_DISPLAY_TIME_STRING_SECOND, clock_display_last_time_second, CLOCK_DISPLAY_BACKGROUND_COLOR, true);
                clockDisplay_print_time(CLOCK_DISPLAY_TIME_STRING_SECOND, clock_display_current_time_second, CLOCK_DISPLAY_COLOR, true);
            }

            // Otherwise.
            else
            {
                // Only update the second character as it is the only one that has changed (black then green).
                clockDisplay_print_time(CLOCK_DISPLAY_TIME_STRING_SECOND, clock_display_last_time_second, CLOCK_DISPLAY_BACKGROUND_COLOR, false);
                clockDisplay_print_time(CLOCK_DISPLAY_TIME_STRING_SECOND, clock_display_current_time_second, CLOCK_DISPLAY_COLOR, false);
            }
        }
    }

    // Set the previous times to the current times.
    clock_display_last_time_second = clock_display_current_time_second;
    clock_display_last_time_minute = clock_display_current_time_minute;
    clock_display_last_time_hour = clock_display_current_time_hour;
}

/*********************************************************************************/
/* Function: clockDisplay_performIncDec                                          */
/* Purpose: Determines what to do based on the touch of the LCD display.         */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void clockDisplay_performIncDec()
{
    // Create and set the x, y, and intensity of the touch display to 0.
    int16_t display_touch_x = 0;
    int16_t display_touch_y = 0;
    uint8_t display_touch_intensity = 0;

    // Get the touched points from the display.
    display_getTouchedPoint(&display_touch_x, &display_touch_y, &display_touch_intensity);

    // If the y is less than sector 0 (we know we are on the top half).
    if (display_touch_y < CLOCK_DISPLAY_TOUCH_Y_0)
    {
        // If the x is less than sector 0 (far left).
        if (display_touch_x < CLOCK_DISPLAY_TOUCH_X_0)
        {
            // Increment the hours.
            clockDisplay_inc_dec_hour(true);
        }

        // Else, if the x is less than sector 1 (the middle).
        else if (display_touch_x < CLOCK_DISPLAY_TOUCH_X_1)
        {
            // Increment the minutes.
            clockDisplay_inc_dec_minute(true);
        }

        // Else, if the x is less than sector 2 (far right).
        else if (display_touch_x < CLOCK_DISPLAY_TOUCH_X_2)
        {
            // Increment the seconds.
            clockDisplay_inc_dec_second(true);
        }
    }

    // Otherwise (we are in the bottom half).
    else
    {
        // If the x is less than sector 0 (far left).
        if (display_touch_x < CLOCK_DISPLAY_TOUCH_X_0)
        {
            // Decrement hours.
            clockDisplay_inc_dec_hour(false);
        }

        // Else, if the x is less than sector 1 (the middle).
        else if (display_touch_x < CLOCK_DISPLAY_TOUCH_X_1)
        {
            // Decrement minutes.
            clockDisplay_inc_dec_minute(false);
        }

        // Else, if the x is less than sector 2 (far right).
        else if (display_touch_x < CLOCK_DISPLAY_TOUCH_X_2)
        {
            // Decrement seconds.
            clockDisplay_inc_dec_second(false);
        }
    }
}

/*********************************************************************************/
/* Function: clockDisplay_advanceTimeOneSecond                                   */
/* Purpose: Advances the time by one second for the LCD display.                 */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void clockDisplay_advanceTimeOneSecond()
{
    // Advance the second by 1.
    clock_display_current_time_second = ++clock_display_current_time_second % CLOCK_DISPLAY_TIME_MAX_SECOND;

    // If the second "rolled over."
    if (clock_display_current_time_second == CLOCK_DISPLAY_ROLL_OVER)
    {
        // Increment the minute by 1.
        clock_display_current_time_minute = ++clock_display_current_time_minute % CLOCK_DISPLAY_TIME_MAX_MINUTE;

        // If the minute "rolled over."
        if (clock_display_current_time_minute == CLOCK_DISPLAY_ROLL_OVER)
        {
            // Increment the hour by 1.
            clock_display_current_time_hour = ++clock_display_current_time_hour % CLOCK_DISPLAY_TIME_MAX_HOUR;

            // If the hour "rolled over."
            if (clock_display_current_time_hour == CLOCK_DISPLAY_ROLL_OVER)
            {
                // Set the hour, minute, and second to their "rolled over" values.
                clock_display_current_time_hour = CLOCK_DISPLAY_HOUR_ROLL_OVER;
                clock_display_current_time_minute = CLOCK_DISPLAY_ROLL_OVER;
                clock_display_current_time_second = CLOCK_DISPLAY_ROLL_OVER;
            }
        }
    }
}

/*********************************************************************************/
/* Function: clockDisplay_runTest                                                */
/* Purpose: Runs a test on the LCD display.                                      */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void clockDisplay_runTest()
{
    // A for loop used to increment the hour.
    for (int i = 0; i < CLOCK_DISPLAY_TEST_COUNT_HOUR; i++)
    {
        for (int j = 0; j < CLOCK_DISPLAY_TEST_PERIOD; j++);

        clockDisplay_updateTimeDisplay(false);
        clockDisplay_inc_dec_hour(true);
    }

    for (int i = CLOCK_DISPLAY_TEST_COUNT_DOUBLE_HOUR; i >= 0; i--)
    {
        for (int j = 0; j < CLOCK_DISPLAY_TEST_PERIOD; j++);

        clockDisplay_updateTimeDisplay(false);
        clockDisplay_inc_dec_hour(false);
    }

    // Test minute
    for (int i = 0; i < CLOCK_DISPLAY_TEST_COUNT_MIN_SEC; i++)
    {
        for (int j = 0; j < CLOCK_DISPLAY_TEST_PERIOD; j++);

        clockDisplay_updateTimeDisplay(false);
        clockDisplay_inc_dec_minute(true);
    }

    for (int i = CLOCK_DISPLAY_TEST_COUNT_DOUBLE_MIN_SEC; i >= 0; i--)
    {
        for (int j = 0; j < CLOCK_DISPLAY_TEST_PERIOD; j++);

        clockDisplay_updateTimeDisplay(false);
        clockDisplay_inc_dec_minute(false);
    }

    // Test second
    for (int i = 0; i < CLOCK_DISPLAY_TEST_COUNT_MIN_SEC; i++)
    {
        for (int j = 0; j < CLOCK_DISPLAY_TEST_PERIOD; j++);

        clockDisplay_updateTimeDisplay(false);
        clockDisplay_inc_dec_second(true);
    }

    // Reset the clock.
    clockDisplay_reset();
    clockDisplay_updateTimeDisplay(true);
}
