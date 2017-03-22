/*********************************************************************************/
/* File: wamDisplay.c                                                            */
/* Purpose: To provide the code necessary to run the whack-a-mole game display.  */
/* Author: Mark Harris                                                           */
/* Date: 12-06-16                                                                */
/*********************************************************************************/
#include "wamDisplay.h"
#include "wamControl.h"
#include "supportFiles/display.h"
#include "supportFiles/utils.h"
#include "../Lab2/switches.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Necessary defines to set the reset values for the static global variables and mole information.
#define WAM_DISPLAY_HITS_RESET 0
#define WAM_DISPLAY_MISSES_RESET 0
#define WAM_DISPLAY_LEVEL_RESET 0
#define WAM_DISPLAY_MS_WAIT_RESET 0

// Necessary defines to adjust the difficulty of the whack-a-mole game.
#define WAM_DISPLAY_SPEED_INCREASE 2
#define WAM_DISPLAY_COUNT_INCREASE 1

// Necessary defines to determine when speed and levels change in the whack-a-mole-game.
#define WAM_DISPLAY_LEVEL_INCREASE_SPEED_EVERY 5
#define WAM_DISPLAY_LEVEL_INCREASE_COUNT_EVERY 10

// Necessary defines to set the board background color, padding, and special points.
#define WAM_DISPLAY_BOARD_BACKGROUND_COLOR DISPLAY_GREEN
#define WAM_DISPLAY_BOARD_PADDING 30
#define WAM_DISPLAY_BOARD_LEFT_X 0
#define WAM_DISPLAY_BOARD_RIGHT_X DISPLAY_WIDTH
#define WAM_DISPLAY_BOARD_TOP_Y 0
#define WAM_DISPLAY_BOARD_BOTTOM_Y DISPLAY_HEIGHT - WAM_DISPLAY_BOARD_PADDING

// Necessary defines to set the display background color, active mole color, text color, and text size.
#define WAM_DISPLAY_BACKGROUND_COLOR DISPLAY_BLACK
#define WAM_DISPLAY_ACTIVE_MOLE_COLOR DISPLAY_YELLOW
#define WAM_DISPLAY_TEXT_COLOR DISPLAY_WHITE
#define WAM_DISPLAY_TEXT_SIZE 2

// Necessary defines to set the splash screen main (h1) text, text size, and positioning.
#define WAM_DISPLAY_SPLASH_TEXT_H1 "Whack-a-Mole!"
#define WAM_DISPLAY_SPLASH_TEXT_H1_SIZE 3.5
#define WAM_DISPLAY_SPLASH_H1_X DISPLAY_WIDTH / 7
#define WAM_DISPLAY_SPLASH_H1_Y DISPLAY_HEIGHT / 2.5

// Necessary defines to set the splah screen sub (h2) text, text size, and positioning.
#define WAM_DISPLAY_SPLASH_TEXT_H2 "Touch to start"
#define WAM_DISPLAY_SPLASH_TEXT_H2_SZIE 2
#define WAM_DISPLAY_SPLASH_H2_X DISPLAY_WIDTH / 5
#define WAM_DISPLAY_SPLASH_H2_Y DISPLAY_HEIGHT / 1.75

// Necessary defines to set the game over screen text.
#define WAM_DISPLAY_GAME_OVER_TEXT_H1 "Game Over!"
#define WAM_DISPLAY_GAME_OVER_TEXT_H2_HITS "Hits:%d"
#define WAM_DISPLAY_GAME_OVER_TEXT_H2_MISSES "Misses:%d"
#define WAM_DISPLAY_GAME_OVER_TEXT_H2_current_level "Final Level:%d"
#define WAM_DISPLAY_GAME_OVER_TEXT_H3 "(Touch to Try Again)"

// Necessary defines to set the game over screen text size.
#define WAM_DISPLAY_GAME_OVER_TEXT_H1_SIZE 3.5
#define WAM_DISPLAY_GAME_OVER_H2_SIZE 2.5
#define WAM_DISPLAY_GAME_OVER_TEXT_H3_SIZE 2

// Necessary defines to set the game over screen text x positions.
#define WAM_DISPLAY_GAME_OVER_TEXT_H1_X DISPLAY_WIDTH / 5
#define WAM_DISPLAY_GAME_OVER_TEXT_H2_HITS_X DISPLAY_WIDTH / 2.75
#define WAM_DISPLAY_GAME_OVER_TEXT_H2_MISSES_X DISPLAY_WIDTH / 3.25
#define WAM_DISPLAY_GAME_OVER_TEXT_H2_current_level_X DISPLAY_WIDTH / 4.75
#define WAM_DISPLAY_GAME_OVER_TEXT_H3_X DISPLAY_WIDTH / 10

// Necessary defines to set the game over screen text y positions.
#define WAM_DISPLAY_GAME_OVER_TEXT_H1_Y DISPLAY_HEIGHT / 3.5
#define WAM_DISPLAY_GAME_OVER_TEXT_H2_HITS_Y DISPLAY_HEIGHT / 2.25
#define WAM_DISPLAY_GAME_OVER_TEXT_H2_MISSES_Y DISPLAY_HEIGHT / 1.95
#define WAM_DISPLAY_GAME_OVER_TEXT_H2_current_level_Y DISPLAY_HEIGHT / 1.70
#define WAM_DISPLAY_GAME_OVER_TEXT_H3_Y DISPLAY_HEIGHT / 1.25

// Necessary defines to set the score bar text below the mole board.
#define WAM_DISPLAY_SCORE_HIT_TEXT "Hit:%d"
#define WAM_DISPLAY_SCORE_MISS_TEXT "Miss:%d"
#define WAM_DISPLAY_SCORE_LEVEL_TEXT "Level:%d"
#define WAM_DISPLAY_SCORE_PADDING_TEXT "   "

// Necessary defines to set the score screen text size and positioning.
#define WAM_DISPLAY_SCORE_TEXT_SIZE 2
#define WAM_DISPLAY_SCORE_HIT_X DISPLAY_WIDTH / 25
#define WAM_DISPLAY_SCORE_MISS_X DISPLAY_WIDTH / 3
#define WAM_DISPLAY_SCORE_LEVEL_X DISPLAY_WIDTH / 1.55
#define WAM_DISPLAY_SCORE_Y DISPLAY_HEIGHT - 25

// Necessary defines to set the string sizes used in the whack-a-mole game.
#define WAM_DISPLAY_HITS_STRING_SIZE 12
#define WAM_DISPLAY_MISSES_STRING_SIZE 14
#define WAM_DISPLAY_CURRENT_LEVEL_STRING_SIZE 19

// Necessary defines to set the score screen string sizes used in the whack-a-mole game.
#define WAM_DISPLAY_SCORE_HIT_STRING_SIZE 11
#define WAM_DISPLAY_SCORE_MISS_STRING_SIZE 12
#define WAM_DISPLAY_SCORE_LEVEL_STRING_SIZE 13

// Necessary defines used to set the maximum count of x's and y's.
#define WAM_DISPLAY_MOLE_MAX_X 3
#define WAM_DISPLAY_MOLE_MAX_Y 3

// Necessary defines used to set the minimum count of x's and y's.
#define WAM_DISPLAY_MOLE_START_X 0
#define WAM_DISPLAY_MOLE_START_Y 0
#define WAM_DISPLAY_MOLE_START 0

// Necessary defines to set the mole radius, error index, and modulus (used to draw the mole holes to the screen).
#define WAM_DISPLAY_MOLE_RADIUS 25
#define WAM_DISPLAY_MOLE_INDEX_ERROR -1
#define WAM_DISPLAY_MOLE_EVEN_MODULUS 2

// Necessary defines to set the increment value, interval band, and minimum interval for the moles.
#define WAM_DISPLAY_INCREMENT_VALUE 1
#define WAM_DISPLAY_TIME_INTERVAL_BAND 42
#define WAM_DISPLAY_TIME_INTERVAL_MINIMUM 10

// Necessary defines to set the origin of each hole in the mole board.
#define WAM_DISPLAY_MOLE_X_0 1 * DISPLAY_WIDTH / 6
#define WAM_DISPLAY_MOLE_X_1 3 * DISPLAY_WIDTH / 6
#define WAM_DISPLAY_MOLE_X_2 5 * DISPLAY_WIDTH / 6
#define WAM_DISPLAY_MOLE_Y_0 1 * (DISPLAY_HEIGHT - WAM_DISPLAY_BOARD_PADDING) / 6
#define WAM_DISPLAY_MOLE_Y_1 3 * (DISPLAY_HEIGHT - WAM_DISPLAY_BOARD_PADDING) / 6
#define WAM_DISPLAY_MOLE_Y_2 5 * (DISPLAY_HEIGHT - WAM_DISPLAY_BOARD_PADDING) / 6

// Necessary defines to set the test values used in the display test.
#define WAM_DISPLAY_TEST_ADC_WAIT_MS 50
#define WAM_DISPLAY_TEST_SPLASH_SCREEN 0
#define WAM_DISPLAY_TEST_BOARD_SCREEN 1
#define WAM_DISPLAY_TEST_GAME_OVER_SCREEN 2
#define WAM_DISPLAY_TEST_NUMBER_OF_SCREENS 3

// Necessary defines to get the switch values from the user.
#define  WAM_DISPLAY_TEST_SWITCH_VALUE_9 9
#define  WAM_DISPLAY_TEST_SWITCH_VALUE_6 6
#define  WAM_DISPLAY_TEST_SWITCH_VALUE_4 4
#define  WAM_DISPLAY_TEST_SWITCH_MASK 0xF

// Constants used for lookup tables to get each x and y location for each mole hole.
const wamDisplay_coord_t WAM_DISPLAY_MOLE_X_LUT[WAM_DISPLAY_MOLE_MAX_X] = {WAM_DISPLAY_MOLE_X_0, WAM_DISPLAY_MOLE_X_1, WAM_DISPLAY_MOLE_X_2};
const wamDisplay_coord_t WAM_DISPLAY_MOLE_Y_LUT[WAM_DISPLAY_MOLE_MAX_Y] = {WAM_DISPLAY_MOLE_Y_0, WAM_DISPLAY_MOLE_Y_1, WAM_DISPLAY_MOLE_Y_2};

// Struct used to store information for a "mole." This contains the origin, the number of ticks until the mole is awake, and the number of ticks until a mole is dormant.
typedef struct {
        wamDisplay_point_t origin;
        wamDisplay_moleTickCount_t ticksUntilAwake;
        wamDisplay_moleTickCount_t ticksUntilDormant;
} wamDisplay_moleInfo_t;

// Static global variable used to point to mole info structs.
static wamDisplay_moleInfo_t** wamDisplay_moleInfo;

// Static global variables used to set the number of moles (for the mole board), the time interval, and current number of hits, misses, and the current level.
static wamDisplay_moleCount_e mole_count;
static uint16_t time_interval_band;
static uint16_t current_hits;
static uint16_t current_misses;
static uint16_t current_level;

/*********************************************************************************/
/* Function: wamDisplay_selectMoleCountFromSwitches                              */
/* Purpose: To return the number of moles based on the position of the switches. */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void wamDisplay_selectMoleCountFromSwitches(uint16_t switchValue)
{
    // Switch on the passed value of the switches bitwise anded to our test switch mask.
    switch(switchValue & WAM_DISPLAY_TEST_SWITCH_MASK)
    {
        // If the value of the switches is the binary value for 9 moles, then set the mole count to that value.
        case WAM_DISPLAY_TEST_SWITCH_VALUE_9:
            wamDisplay_selectMoleCount(wamDisplay_moleCount_9);
            break;

        // Otherwise, if the value of the switches is the binary value for 6 moles, then set the mole count to that value.
        case WAM_DISPLAY_TEST_SWITCH_VALUE_6:
            wamDisplay_selectMoleCount(wamDisplay_moleCount_6);
            break;

        // Otherwise, if the value of the switches is the binary value for 4 moles, then set the mole count to that value.
        case WAM_DISPLAY_TEST_SWITCH_VALUE_4:
            wamDisplay_selectMoleCount(wamDisplay_moleCount_4);
            break;

        // Otherwise, if all else fails (really if the value of the switches is any other number), set the mole count to 9.
        default:
            wamDisplay_selectMoleCount(wamDisplay_moleCount_9);
            break;
    }
}

/*********************************************************************************/
/* Function: wamDisplay_get_mole_count                                           */
/* Purpose: To return the number of moles based on the mole count (a decimal     */
/*          value instead of an enum).                                           */
/* Returns: An integer value (16 bits).                                          */
/*********************************************************************************/
uint16_t wamDisplay_get_mole_count(wamDisplay_moleCount_e number_of_moles)
{
    // Switch on the (enum) number of moles.
    switch (number_of_moles)
    {
        // If we are in the 9 mole count, then return the decimal 9 to the user.
        case wamDisplay_moleCount_9:
            return WAM_DISPLAY_TEST_SWITCH_VALUE_9;

        // Otherwise, if we are in the 6 mole count, then return the decimal 6 to the user.
        case wamDisplay_moleCount_6:
            return WAM_DISPLAY_TEST_SWITCH_VALUE_6;

        // Otherwise, we must be in the 4 mole count so return the decimal 4 to the user.
        default:
            return WAM_DISPLAY_TEST_SWITCH_VALUE_4;
    }
}

/*********************************************************************************/
/* Function: wamDisplay_draw_mole_holes                                          */
/* Purpose: To draw the mole board.                                              */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void wamDisplay_draw_mole_holes(wamDisplay_moleCount_e number_of_moles)
{
    // Switch on the number of moles.
    switch (number_of_moles)
    {
        // If we are in the 9 mole count.
        case wamDisplay_moleCount_9:
            // For every x position in the mole board.
            for (wamDisplay_moleIndex_t x = WAM_DISPLAY_MOLE_START_X; x < WAM_DISPLAY_MOLE_MAX_X; x++)
            {
                // For every y position in the mole board.
                for (wamDisplay_moleIndex_t y = WAM_DISPLAY_MOLE_START_Y; y < WAM_DISPLAY_MOLE_MAX_Y; y++)
                {
                    // Draw a mole hole in every spot of the mole board.
                    display_fillCircle(WAM_DISPLAY_MOLE_X_LUT[x], WAM_DISPLAY_MOLE_Y_LUT[y], WAM_DISPLAY_MOLE_RADIUS, WAM_DISPLAY_BACKGROUND_COLOR);
                }
            }

            break;

        // Otherwise, if we are in the 6 mole count.
        case wamDisplay_moleCount_6:
            // For every x position in the mole board.
            for (wamDisplay_moleIndex_t x = WAM_DISPLAY_MOLE_START_X; x < WAM_DISPLAY_MOLE_MAX_X; x++)
            {
                // For every y position in the mole board.
                for (wamDisplay_moleIndex_t y = WAM_DISPLAY_MOLE_START_Y; y < WAM_DISPLAY_MOLE_MAX_Y; y++)
                {
                    // As long as we are in the first and last row in the mole board.
                    if (!(y % WAM_DISPLAY_MOLE_EVEN_MODULUS))
                    {
                        // Draw a mole hole.
                        display_fillCircle(WAM_DISPLAY_MOLE_X_LUT[x], WAM_DISPLAY_MOLE_Y_LUT[y], WAM_DISPLAY_MOLE_RADIUS, WAM_DISPLAY_BACKGROUND_COLOR);
                    }
                }
            }

            break;

        // Otherwise, we are in the four mole whole count.
        default:
            // For every x position in the mole board.
            for (wamDisplay_moleIndex_t x = WAM_DISPLAY_MOLE_START_X; x < WAM_DISPLAY_MOLE_MAX_X; x++)
            {
                // For every y position in the mole board.
                for (wamDisplay_moleIndex_t y = WAM_DISPLAY_MOLE_START_Y; y < WAM_DISPLAY_MOLE_MAX_Y; y++)
                {
                    // As long as we are in the first and last column and the first and last row.
                    if (!(y % WAM_DISPLAY_MOLE_EVEN_MODULUS) && !(x % WAM_DISPLAY_MOLE_EVEN_MODULUS))
                    {
                        // Draw a mole hole.
                        display_fillCircle(WAM_DISPLAY_MOLE_X_LUT[x], WAM_DISPLAY_MOLE_Y_LUT[y], WAM_DISPLAY_MOLE_RADIUS, WAM_DISPLAY_BACKGROUND_COLOR);
                    }
                }
            }

            break;
    }
}

/*********************************************************************************/
/* Function: wamDisplay_get_mole_x                                               */
/* Purpose: To return the x origin of the mole based on the mole number.         */
/* Returns: An integer value (typedef as moleIndex_t, 16 bits).                  */
/*********************************************************************************/
wamDisplay_moleIndex_t wamDisplay_get_mole_x(uint8_t mole_number)
{
    // Switch on the mole count.
    switch (mole_count)
    {
        // If we are in the 9 mole count, then return back the x position of the mole by using the max x value as a modulus.
        case wamDisplay_moleCount_9:
            return WAM_DISPLAY_MOLE_X_LUT[mole_number % WAM_DISPLAY_MOLE_MAX_X];

            // Otherwise, if we are in the 6 mole count, then return back the x position of the mole by using the max x value as a modulus.
        case wamDisplay_moleCount_6:
            return WAM_DISPLAY_MOLE_X_LUT[mole_number % WAM_DISPLAY_MOLE_MAX_X];

            // Otherwise, we are in the 4 mole count, so return back the x position by using the even modulus and multiplying by it.
        default:
            return WAM_DISPLAY_MOLE_X_LUT[(mole_number % WAM_DISPLAY_MOLE_EVEN_MODULUS) * WAM_DISPLAY_MOLE_EVEN_MODULUS];
    }
}

/*********************************************************************************/
/* Function: wamDisplay_get_mole_y                                               */
/* Purpose: To return the y origin of the mole based on the mole number.         */
/* Returns: An integer value (typedef as moleIndex_t, 16 bits).                  */
/*********************************************************************************/
wamDisplay_moleIndex_t wamDisplay_get_mole_y(uint8_t mole_number)
{
    // Switch on the mole count.
    switch (mole_count)
    {
        // If we are in the 9 mole count, then return back the y position of the mole by dividing by the max y and then using the max y as a modulus.
        case wamDisplay_moleCount_9:
            return WAM_DISPLAY_MOLE_Y_LUT[(mole_number / WAM_DISPLAY_MOLE_MAX_Y) % WAM_DISPLAY_MOLE_MAX_Y];

        // Otherwise, if we are in the 6 mole count, then return back the y position of the mole by diving by the max y, using the max y as a modulus, then multiplying by the even modulus.
        case wamDisplay_moleCount_6:
            return WAM_DISPLAY_MOLE_Y_LUT[((mole_number / WAM_DISPLAY_MOLE_MAX_Y) % WAM_DISPLAY_MOLE_MAX_Y) * WAM_DISPLAY_MOLE_EVEN_MODULUS];

            // Otherwise, we are in the 4 mole count, so return back the y position by diving by the even modulus, using the even modulus, and then by multiplying by the even modulus.
        default:
            return WAM_DISPLAY_MOLE_Y_LUT[((mole_number / WAM_DISPLAY_MOLE_EVEN_MODULUS) % WAM_DISPLAY_MOLE_EVEN_MODULUS) * WAM_DISPLAY_MOLE_EVEN_MODULUS];
    }
}

/*********************************************************************************/
/* Function: wamDisplay_draw_active_mole_hole                                    */
/* Purpose: To draw (or erase) an active mole hole.                              */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void wamDisplay_draw_active_mole_hole(uint8_t mole_number, bool erase)
{
    // Draw (or erase) the active mole hole based on the passed mole number.
    display_fillCircle(wamDisplay_get_mole_x(mole_number), wamDisplay_get_mole_y(mole_number), WAM_DISPLAY_MOLE_RADIUS, (erase ? WAM_DISPLAY_BACKGROUND_COLOR : WAM_DISPLAY_ACTIVE_MOLE_COLOR));
}

/*********************************************************************************/
/* Function: wamDisplay_free_mole_memory                                         */
/* Purpose: To return the y origin of the mole based on the mole number.         */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void wamDisplay_free_mole_memory()
{
    // For every mole (based on the mole count).
    for (uint16_t i = WAM_DISPLAY_MOLE_START; i < wamDisplay_get_mole_count(mole_count); i++)
    {
        // Free all of the mole info structs and set them to NULL (not necessary but removes any idea of the memory still being allocated).
        free(wamDisplay_moleInfo[i]);
        wamDisplay_moleInfo[i] = NULL;
    }

    // Now free the entire struct and set it to NULL.
    free(wamDisplay_moleInfo);
    wamDisplay_moleInfo = NULL;
}

/*********************************************************************************/
/* Function: wamDisplay_computeMoleInfo                                          */
/* Purpose: To allocate memory for the moles.                                    */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void wamDisplay_computeMoleInfo()
{
    // Allocate the memory size needed for the number of moles.
    wamDisplay_moleInfo = (wamDisplay_moleInfo_t**) malloc(wamDisplay_get_mole_count(mole_count) * sizeof(wamDisplay_moleInfo_t*));

    // For every mole allocated.
    for (wamDisplay_moleIndex_t i = WAM_DISPLAY_MOLE_START; i < wamDisplay_get_mole_count(mole_count); i++)
    {
        // Allocate the memory size needed for the information a mole holds.
        wamDisplay_moleInfo[i] = (wamDisplay_moleInfo_t*) malloc(sizeof(wamDisplay_moleInfo_t));
    }

    // For ever mole allocated.
    for (wamDisplay_moleIndex_t i = WAM_DISPLAY_MOLE_START; i < wamDisplay_get_mole_count(mole_count); i++)
    {
        // Set the origin and ticks to their default values.
        wamDisplay_moleInfo[i]->origin.x = wamDisplay_get_mole_x(i);
        wamDisplay_moleInfo[i]->origin.y = wamDisplay_get_mole_y(i);
        wamDisplay_moleInfo[i]->ticksUntilAwake = WAM_DISPLAY_MS_WAIT_RESET;
        wamDisplay_moleInfo[i]->ticksUntilDormant = WAM_DISPLAY_MS_WAIT_RESET;
    }
}

/*********************************************************************************/
/* Function: wamDisplay_selectMoleCount                                          */
/* Purpose: To set the number of moles selected.                                 */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void wamDisplay_selectMoleCount(wamDisplay_moleCount_e moleCount)
{
    // Set the mole count to the passed mole count.
    mole_count = moleCount;
}

/*********************************************************************************/
/* Function: wamDisplay_init                                                     */
/* Purpose: To initialize the whack-a-mole display.                              */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void wamDisplay_init()
{
    // Initialize the display and fill the screen with the background color.
    display_init();
    display_fillScreen(WAM_DISPLAY_BACKGROUND_COLOR);

    // Set the default interval value.
    time_interval_band = WAM_DISPLAY_TIME_INTERVAL_BAND;
}

/*********************************************************************************/
/* Function: wamDisplay_drawMoleBoard                                            */
/* Purpose: To draw (or erase) the mole board on the display.                    */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void wamDisplay_drawMoleBoard(bool erase)
{
    // Draw the mole board by drawing the board background, getting the number of moles, drawing the mole holes, and then allocating the memory for the moles.
    display_fillRect(WAM_DISPLAY_BOARD_LEFT_X, WAM_DISPLAY_BOARD_TOP_Y, WAM_DISPLAY_BOARD_RIGHT_X, WAM_DISPLAY_BOARD_BOTTOM_Y, (erase ? WAM_DISPLAY_BACKGROUND_COLOR : WAM_DISPLAY_BOARD_BACKGROUND_COLOR));
    wamDisplay_selectMoleCountFromSwitches(switches_read());
    wamDisplay_draw_mole_holes(mole_count);
    wamDisplay_computeMoleInfo();
}

/*********************************************************************************/
/* Function: wamDisplay_drawSplashScreen                                         */
/* Purpose: To draw (or erase) the splash screen on the display.                 */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void wamDisplay_drawSplashScreen(bool erase)
{
    // Set the text color for the splash screen message.
    display_setTextColor((erase ? WAM_DISPLAY_BACKGROUND_COLOR : WAM_DISPLAY_TEXT_COLOR));

    // Set the cursor, text size, then print the message for the splash screen title.
    display_setCursor(WAM_DISPLAY_SPLASH_H1_X, WAM_DISPLAY_SPLASH_H1_Y);
    display_setTextSize(WAM_DISPLAY_SPLASH_TEXT_H1_SIZE);
    display_println(WAM_DISPLAY_SPLASH_TEXT_H1);

    // Set the cursor, text size, then print the message for the splash screen subtitle.
    display_setCursor(WAM_DISPLAY_SPLASH_H2_X, WAM_DISPLAY_SPLASH_H2_Y);
    display_setTextSize(WAM_DISPLAY_SPLASH_TEXT_H2_SZIE);
    display_println(WAM_DISPLAY_SPLASH_TEXT_H2);
}

/*********************************************************************************/
/* Function: wamDisplay_drawGameOverScreen                                       */
/* Purpose: To draw (or erase) the game over screen on the display.              */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void wamDisplay_drawGameOverScreen(bool erase)
{
    // Set the hit, miss, and current level strings (used temporarily).
    char hit_string[WAM_DISPLAY_HITS_STRING_SIZE];
    char misses_string[WAM_DISPLAY_MISSES_STRING_SIZE];
    char current_level_string[WAM_DISPLAY_CURRENT_LEVEL_STRING_SIZE];

    // Set the hit, miss, and current level strings with the necessary data.
    sprintf(hit_string, WAM_DISPLAY_GAME_OVER_TEXT_H2_HITS, current_hits);
    sprintf(misses_string, WAM_DISPLAY_GAME_OVER_TEXT_H2_MISSES, current_misses);
    sprintf(current_level_string, WAM_DISPLAY_GAME_OVER_TEXT_H2_current_level, current_level);

    // Set the text color for the game over screen.
    display_setTextColor((erase ? WAM_DISPLAY_BACKGROUND_COLOR : WAM_DISPLAY_TEXT_COLOR));

    // Set the cursor position, text size, and then print the main title (h1).
    display_setCursor(WAM_DISPLAY_GAME_OVER_TEXT_H1_X, WAM_DISPLAY_GAME_OVER_TEXT_H1_Y);
    display_setTextSize(WAM_DISPLAY_GAME_OVER_TEXT_H1_SIZE);
    display_println(WAM_DISPLAY_GAME_OVER_TEXT_H1);

    // Set the text size, cursor, and then print the hits, misses, and current level strings.
    display_setTextSize(WAM_DISPLAY_GAME_OVER_H2_SIZE);
    display_setCursor(WAM_DISPLAY_GAME_OVER_TEXT_H2_HITS_X, WAM_DISPLAY_GAME_OVER_TEXT_H2_HITS_Y);
    display_println(hit_string);
    display_setCursor(WAM_DISPLAY_GAME_OVER_TEXT_H2_MISSES_X, WAM_DISPLAY_GAME_OVER_TEXT_H2_MISSES_Y);
    display_println(misses_string);
    display_setCursor(WAM_DISPLAY_GAME_OVER_TEXT_H2_current_level_X, WAM_DISPLAY_GAME_OVER_TEXT_H2_current_level_Y);
    display_println(current_level_string);

    // Set the text size, cursor position, and then print the "touch to continue" message.
    display_setTextSize(WAM_DISPLAY_GAME_OVER_TEXT_H3_SIZE);
    display_setCursor(WAM_DISPLAY_GAME_OVER_TEXT_H3_X, WAM_DISPLAY_GAME_OVER_TEXT_H3_Y);
    display_println(WAM_DISPLAY_GAME_OVER_TEXT_H3);
}

/*********************************************************************************/
/* Function: wamDisplay_activateRandomMole                                       */
/* Purpose: To activate a random mole on the mole board.                         */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void wamDisplay_activateRandomMole()
{
    // Get a random number and modulus it by the current number of moles.
    uint8_t random_index = wamControl_get_random_number() % wamDisplay_get_mole_count(mole_count);

    // Set the number of ticks until awake and dormant for the randomly chosen mole.
    wamDisplay_moleInfo[random_index]->ticksUntilAwake = wamControl_getRandomMoleAwakeInterval() % time_interval_band + WAM_DISPLAY_TIME_INTERVAL_MINIMUM;
    wamDisplay_moleInfo[random_index]->ticksUntilDormant = wamControl_getRandomMoleAsleepInterval() % time_interval_band + WAM_DISPLAY_TIME_INTERVAL_MINIMUM;
}

/*********************************************************************************/
/* Function: wamDisplay_whackMole                                                */
/* Purpose: To attempt to "whack" a mole on the mole board.                      */
/* Returns: An integer value (typedef as moleIndex_t, 16 bits).                  */
/*********************************************************************************/
wamDisplay_moleIndex_t wamDisplay_whackMole(wamDisplay_point_t* whackOrigin)
{
    // For every mole hole on the mole board.
    for (uint16_t i = WAM_DISPLAY_MOLE_START; i < wamDisplay_get_mole_count(mole_count); i++)
    {
        // If the current mole has ticks until dormant set, that means it is active.
        if (wamDisplay_moleInfo[i]->ticksUntilDormant)
        {
            // If the touch point is within the radius of the origin of the mole.
            if ((whackOrigin->x >= wamDisplay_moleInfo[i]->origin.x - WAM_DISPLAY_MOLE_RADIUS && whackOrigin->x <= wamDisplay_moleInfo[i]->origin.x + WAM_DISPLAY_MOLE_RADIUS) && (whackOrigin->y >= wamDisplay_moleInfo[i]->origin.y - WAM_DISPLAY_MOLE_RADIUS && whackOrigin->y <= wamDisplay_moleInfo[i]->origin.y + WAM_DISPLAY_MOLE_RADIUS))
            {
                // Set the ticks until dormant to their default value, erase the active mole hole, and then increment the hit score.
                wamDisplay_moleInfo[i]->ticksUntilDormant = WAM_DISPLAY_MS_WAIT_RESET;
                wamDisplay_draw_active_mole_hole(i, true);
                wamDisplay_setHitScore(current_hits + WAM_DISPLAY_INCREMENT_VALUE);

                // If it has been the value needed to increment the speed (which also increases the level).
                if (!(wamDisplay_getHitScore() % WAM_DISPLAY_LEVEL_INCREASE_SPEED_EVERY))
                {
                    // Increment the level and increase the speed.
                    wamDisplay_incrementLevel();
                    time_interval_band /= WAM_DISPLAY_SPEED_INCREASE;
                }

                // If it has been the value needed to increment the number of moles.
                if (!(wamDisplay_getHitScore() % WAM_DISPLAY_LEVEL_INCREASE_COUNT_EVERY) && wamControl_getMaxActiveMoles() < wamDisplay_get_mole_count(mole_count))
                {
                    // Increment the number of moles that are allowed to be active.
                    wamControl_setMaxActiveMoles(wamControl_getMaxActiveMoles() + WAM_DISPLAY_COUNT_INCREASE);
                }

                // Return the index of the changed mole.
                return i;
            }
        }
    }

    // Otherwise, if we get to this point, increment the miss score.
    wamDisplay_incrementMissScore();

    return WAM_DISPLAY_MOLE_INDEX_ERROR;
}

/*********************************************************************************/
/* Function: wamDisplay_updateAllMoleTickCounts                                  */
/* Purpose: To update all of the moles tick values (if they are set).            */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void wamDisplay_updateAllMoleTickCounts()
{
    // For ever mole in the mole board.
    for (uint16_t i = WAM_DISPLAY_MOLE_START; i < wamDisplay_get_mole_count(mole_count); i++)
    {
        // If the number of ticks until awake are set, then decrement (subtract the increment) from the tick value.
        if (wamDisplay_moleInfo[i]->ticksUntilAwake)
        {
            wamDisplay_moleInfo[i]->ticksUntilAwake = wamDisplay_moleInfo[i]->ticksUntilAwake - WAM_DISPLAY_INCREMENT_VALUE;
        }

        // Otherwise.
        else
        {
            // If the number of ticks until dormant are set.
            if (wamDisplay_moleInfo[i]->ticksUntilDormant)
            {
                // Draw the active mole hole and decrement the ticks until dormant.
                wamDisplay_draw_active_mole_hole(i, false);
                wamDisplay_moleInfo[i]->ticksUntilDormant = wamDisplay_moleInfo[i]->ticksUntilDormant - WAM_DISPLAY_INCREMENT_VALUE;

                // If the number of ticks until dormant are no longer set.
                if (!wamDisplay_moleInfo[i]->ticksUntilDormant)
                {
                    // Erase the mole hole and increment the miss score.
                    wamDisplay_draw_active_mole_hole(i, true);
                    wamDisplay_incrementMissScore();
                }
            }
        }
    }
}

/*********************************************************************************/
/* Function: wamDisplay_getActiveMoleCount                                       */
/* Purpose: To get the number of active moles.                                   */
/* Returns: An integer value (16 bits).                                          */
/*********************************************************************************/
uint16_t wamDisplay_getActiveMoleCount()
{
    // Create a temporary value to store the number of active moles.
    uint16_t count = WAM_DISPLAY_MOLE_START;

    // For ever mole in the mole board.
    for (wamDisplay_moleIndex_t i = WAM_DISPLAY_MOLE_START; i < wamDisplay_get_mole_count(mole_count); i++)
    {
        // If the number of ticks until awake or dormant are set, then increment the count.
        if (wamDisplay_moleInfo[i]->ticksUntilAwake || wamDisplay_moleInfo[i]->ticksUntilDormant)
        {
            count++;
        }
    }

    // Return the count.
    return count;
}

/*********************************************************************************/
/* Function: wamDisplay_setHitScore                                              */
/* Purpose: To set the hit score on the score board below the mole board.        */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void wamDisplay_setHitScore(uint16_t hits)
{
    // Set the hit string.
    char hit_string[WAM_DISPLAY_SCORE_HIT_STRING_SIZE];

    // Put the necessary values in the hit string (the current hit value before being changed).
    sprintf(hit_string, WAM_DISPLAY_SCORE_HIT_TEXT, current_hits);

    // Set the text color, size, cursor, and then erase the hit string on the display.
    display_setTextColor(WAM_DISPLAY_BACKGROUND_COLOR);
    display_setTextSize(WAM_DISPLAY_SCORE_TEXT_SIZE);
    display_setCursor(WAM_DISPLAY_SCORE_HIT_X, WAM_DISPLAY_SCORE_Y);
    display_print(hit_string);

    // Set the current hits to the passed hits value and put the necessary values in the hit string.
    current_hits = hits;
    sprintf(hit_string, WAM_DISPLAY_SCORE_HIT_TEXT, current_hits);

    // Set the text color, size, cursor, and then print the hit string to the display.
    display_setTextColor(WAM_DISPLAY_TEXT_COLOR);
    display_setTextSize(WAM_DISPLAY_SCORE_TEXT_SIZE);
    display_setCursor(WAM_DISPLAY_SCORE_HIT_X, WAM_DISPLAY_SCORE_Y);
    display_print(hit_string);
}

/*********************************************************************************/
/* Function: wamDisplay_getHitScore                                              */
/* Purpose: To get the hit score.                                                */
/* Returns: An integer value (16 bits).                                          */
/*********************************************************************************/
uint16_t wamDisplay_getHitScore()
{
    // Return the current number of hits.
    return current_hits;
}

/*********************************************************************************/
/* Function: wamDisplay_setMissScore                                             */
/* Purpose: To set the miss score on the score board below the mole board.       */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void wamDisplay_setMissScore(uint16_t misses)
{
    // Set the miss string.
    char miss_string[WAM_DISPLAY_SCORE_MISS_STRING_SIZE];

    // Set the text size and put the current value of misses in the miss string.
    display_setTextSize(WAM_DISPLAY_SCORE_TEXT_SIZE);
    sprintf(miss_string, WAM_DISPLAY_SCORE_MISS_TEXT, current_misses);

    // Set the text color, cursor position, and erase the old miss string on the display.
    display_setTextColor(WAM_DISPLAY_BACKGROUND_COLOR);
    display_setCursor(WAM_DISPLAY_SCORE_MISS_X, WAM_DISPLAY_SCORE_Y);
    display_print(miss_string);

    // Set the value of current misses to the passed misses value then set the missing string with the right values.
    current_misses = misses;
    sprintf(miss_string, WAM_DISPLAY_SCORE_MISS_TEXT, current_misses);

    // Set the text color, cursor position, and then print the new string to the display.
    display_setTextColor(WAM_DISPLAY_TEXT_COLOR);
    display_setCursor(WAM_DISPLAY_SCORE_MISS_X, WAM_DISPLAY_SCORE_Y);
    display_print(miss_string);
}

/*********************************************************************************/
/* Function: wamDisplay_getMissScore                                             */
/* Purpose: To get the miss score on the score board below the mole board.       */
/* Returns: An integer value (16 bits).                                          */
/*********************************************************************************/
uint16_t wamDisplay_getMissScore()
{
    // Return the number of misses.
    return current_misses;
}

/*********************************************************************************/
/* Function: wamDisplay_incrementLevel                                           */
/* Purpose: To increment the level.                                              */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void wamDisplay_incrementLevel()
{
    // Set the level string.
    char level_string[WAM_DISPLAY_SCORE_LEVEL_STRING_SIZE];

    // Put the appropriate values into the level string.
    sprintf(level_string, WAM_DISPLAY_SCORE_LEVEL_TEXT, current_level);

    // Set the text color, size, cursor position, and erase the old level string.
    display_setTextColor(WAM_DISPLAY_BACKGROUND_COLOR);
    display_setTextSize(WAM_DISPLAY_SCORE_TEXT_SIZE);
    display_setCursor(WAM_DISPLAY_SCORE_LEVEL_X, WAM_DISPLAY_SCORE_Y);
    display_print(level_string);

    // Increment the current level and put the appropriate values in the level string.
    current_level++;
    sprintf(level_string, WAM_DISPLAY_SCORE_LEVEL_TEXT, current_level);

    // Set the text color, size, cursor position, and print the level string to the display.
    display_setTextColor(WAM_DISPLAY_TEXT_COLOR);
    display_setTextSize(WAM_DISPLAY_SCORE_TEXT_SIZE);
    display_setCursor(WAM_DISPLAY_SCORE_LEVEL_X, WAM_DISPLAY_SCORE_Y);
    display_print(level_string);
}

/*********************************************************************************/
/* Function: wamDisplay_getLevel                                                 */
/* Purpose: To get the current level value.                                      */
/* Returns: An integer value (16 bits).                                          */
/*********************************************************************************/
uint16_t wamDisplay_getLevel()
{
    // Return the value of the current level.
    return current_level;
}

/*********************************************************************************/
/* Function: wamDisplay_drawScoreScreen                                          */
/* Purpose: To draw (or erase) the score screen on the display.                  */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void wamDisplay_drawScoreScreen(bool erase)
{
    // Set the hit, miss, and level strings.
    char hit_string[WAM_DISPLAY_SCORE_HIT_STRING_SIZE];
    char miss_string[WAM_DISPLAY_SCORE_MISS_STRING_SIZE];
    char level_string[WAM_DISPLAY_SCORE_LEVEL_STRING_SIZE];

    // Put the necessary values into the strings.
    sprintf(hit_string, WAM_DISPLAY_SCORE_HIT_TEXT, current_hits);
    sprintf(miss_string, WAM_DISPLAY_SCORE_MISS_TEXT, current_misses);
    sprintf(level_string, WAM_DISPLAY_SCORE_LEVEL_TEXT, current_level);

    // Set the text color and size.
    display_setTextColor((erase ? WAM_DISPLAY_BACKGROUND_COLOR : WAM_DISPLAY_TEXT_COLOR));
    display_setTextSize(WAM_DISPLAY_SCORE_TEXT_SIZE);

    // Set the cursor position and print the hit string.
    display_setCursor(WAM_DISPLAY_SCORE_HIT_X, WAM_DISPLAY_SCORE_Y);
    display_print(hit_string);

    // Set the cursor position and print the miss string.
    display_setCursor(WAM_DISPLAY_SCORE_MISS_X, WAM_DISPLAY_SCORE_Y);
    display_print(miss_string);

    // Set the cursor position and print the level string.
    display_setCursor(WAM_DISPLAY_SCORE_LEVEL_X, WAM_DISPLAY_SCORE_Y);
    display_print(level_string);
}

/*********************************************************************************/
/* Function: wamDisplay_incrementMissScore                                       */
/* Purpose: To increment the miss score.                                         */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void wamDisplay_incrementMissScore()
{
    // Pass the incremented value of the current misses to the set miss score function.
    wamDisplay_setMissScore(current_misses + WAM_DISPLAY_INCREMENT_VALUE);
}

/*********************************************************************************/
/* Function: wamDisplay_resetAllScoresAndLevel                                   */
/* Purpose: To reset all of the scores, levels, and free the memory used.        */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void wamDisplay_resetAllScoresAndLevel()
{
    // Set the hits, misses, and levels variables to the default values.
    current_hits = WAM_DISPLAY_HITS_RESET;
    current_misses = WAM_DISPLAY_MISSES_RESET;
    current_level = WAM_DISPLAY_LEVEL_RESET;

    // Free the memory used by the moles.
    wamDisplay_free_mole_memory();
}

/*********************************************************************************/
/* Function: wamDisplay_runMilestone1_test                                       */
/* Purpose: To test the display screens and positioning.                         */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void wamDisplay_runMilestone1_test()
{
    // Create temporary values to keep track of the current and last screen.
    uint8_t current_screen = WAM_DISPLAY_TEST_SPLASH_SCREEN;
    uint8_t last_screen = current_screen;

    // Initialize the display and print the splash screen.
    wamDisplay_init();
    wamDisplay_drawSplashScreen(false);

    // Initialize the switches.
    switches_init();

    // While true (in other words, forever).
    while (true)
    {
        // If the display is touched.
        if (display_isTouched())
        {
            // Wait for the ADC settle time for the touch screen.
            utils_msDelay(WAM_DISPLAY_TEST_ADC_WAIT_MS);

            // If the display is still touched.
            if (display_isTouched())
            {
                // While the display is still touched, wait, then increment the current screen.
                while (display_isTouched());
                current_screen++;
            }
        }

        // If the last screen is not equal to the current screen.
        if (last_screen != current_screen)
        {
            // Switch the current screen modulus the number of screens tested.
            switch (current_screen % WAM_DISPLAY_TEST_NUMBER_OF_SCREENS)
            {
                // If we are on the splash screen.
                case WAM_DISPLAY_TEST_SPLASH_SCREEN:
                    // Erase the game over screen then draw the splash screen.
                    wamDisplay_drawGameOverScreen(true);
                    wamDisplay_drawSplashScreen(false);
                    break;

                // Otherwise, if we are on the board screen.
                case WAM_DISPLAY_TEST_BOARD_SCREEN:
                    // Erase the splash screen and draw the mole board.
                    wamDisplay_drawSplashScreen(true);
                    wamDisplay_drawMoleBoard(false);

                    // Get the number of moles from the switches and draw the mole holes.
                    wamDisplay_selectMoleCountFromSwitches(switches_read());
                    wamDisplay_draw_mole_holes(mole_count);

                    // Draw the score bar at the bottom of the screen.
                    wamDisplay_drawScoreScreen(false);
                    break;

                    // Otherwise, if we are on the game over screen.
                case WAM_DISPLAY_TEST_GAME_OVER_SCREEN:
                    // Erase the mole board and the score screen. Draw the game over screen.
                    wamDisplay_drawMoleBoard(true);
                    wamDisplay_drawScoreScreen(true);
                    wamDisplay_drawGameOverScreen(false);
                    break;
            }
        }

        // Set the last screen to the current screen.
        last_screen = current_screen;
    }
}
