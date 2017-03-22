/*********************************************************************************/
/* File: simonDisplay.c                                                          */
/* Purpose: To provide the code necessary to run the LCD display for the Simon   */
/*          game.                                                                */
/* Author: Mark Harris                                                           */
/* Date: 11-16-16                                                                */
/*********************************************************************************/
#include "simonDisplay.h"
#include "supportFiles/display.h"

// Necessary define to give the number of regions.
#define SIMON_DISPLAY_REGION_COUNT 4

// Necessary defines to set the touch delay and the text
#define SIMON_DISPLAY_TOUCH_DELAY 50
#define SIMON_DISPLAY_TEXT_COLOR DISPLAY_WHITE
#define SIMON_DISPLAY_TEXT_SIZE 2.5
#define SIMON_DISPLAY_TEXT_START_HEIGHT DISPLAY_HEIGHT / 2.5

// Necessary defines to set the background color and the color of the different regions.
#define SIMON_DISPLAY_BACKGROUND_COLOR DISPLAY_BLACK
#define SIMON_DISPLAY_REGION_0_COLOR DISPLAY_RED
#define SIMON_DISPLAY_REGION_1_COLOR DISPLAY_YELLOW
#define SIMON_DISPLAY_REGION_2_COLOR DISPLAY_BLUE
#define SIMON_DISPLAY_REGION_3_COLOR DISPLAY_GREEN

// Necessary defines for computing the region number.
#define SIMON_DISPLAY_TOP_COORDINATE DISPLAY_HEIGHT / 2
#define SIMON_DISPLAY_BOTTOM_COORDINATE DISPLAY_HEIGHT
#define SIMON_DISPLAY_LEFT_COORDINATE DISPLAY_WIDTH / 2
#define SIMON_DISPLAY_RIGHT_COORDINATE DISPLAY_WIDTH

// Necessary defines to display the button correctly on the x,y plane.
#define SIMON_DISPLAY_X_BUTTON_COORDINATE SIMON_DISPLAY_LEFT_COORDINATE / 2
#define SIMON_DISPLAY_Y_BUTTON_COORDINATE SIMON_DISPLAY_TOP_COORDINATE / 2

// Necessary defines to display the button correctly.
#define SIMON_DISPLAY_BUTTON_HALF_HEIGHT SIMON_DISPLAY_BUTTON_HEIGHT / 2
#define SIMON_DISPLAY_BUTTON_HALF_WIDTH SIMON_DISPLAY_BUTTON_WIDTH / 2

// Necessary defines to display the squares (touched buttons) correctly.
#define SIMON_DISPLAY_SQUARE_HALF_HEIGHT SIMON_DISPLAY_SQUARE_HEIGHT / 2
#define SIMON_DISPLAY_SQUARE_HALF_WIDTH SIMON_DISPLAY_SQUARE_WIDTH / 2

// Necessary constants containing lookup tables for the x region, y region, and colors.
const uint16_t SIMON_DISPLAY_REGION_X_LUT[SIMON_DISPLAY_REGION_COUNT] = {SIMON_DISPLAY_LEFT_COORDINATE, SIMON_DISPLAY_RIGHT_COORDINATE, SIMON_DISPLAY_LEFT_COORDINATE, SIMON_DISPLAY_RIGHT_COORDINATE};
const uint16_t SIMON_DISPLAY_REGION_Y_LUT[SIMON_DISPLAY_REGION_COUNT] = {SIMON_DISPLAY_TOP_COORDINATE, SIMON_DISPLAY_TOP_COORDINATE, SIMON_DISPLAY_BOTTOM_COORDINATE, SIMON_DISPLAY_BOTTOM_COORDINATE};
const uint32_t SIMON_DISPLAY_REGION_COLOR_LUT[SIMON_DISPLAY_REGION_COUNT] = {SIMON_DISPLAY_REGION_0_COLOR, SIMON_DISPLAY_REGION_1_COLOR, SIMON_DISPLAY_REGION_2_COLOR, SIMON_DISPLAY_REGION_3_COLOR};

/*********************************************************************************/
/* Function: simonDisplay_computeRegionNumber                                    */
/* Purpose: To compute the button region based on a passed x and y coordinate.   */
/* Returns: An 8 bit integer value.                                              */
/*********************************************************************************/
int8_t simonDisplay_computeRegionNumber(int16_t x, int16_t y)
{
    // If the passed x coordinate is less than the left half of the display.
    if (x < SIMON_DISPLAY_LEFT_COORDINATE)
    {
        // If the passed y coordinate is less than the top half of the display.
        if (y < SIMON_DISPLAY_TOP_COORDINATE)
        {
            // Return region 0.
            return SIMON_DISPLAY_REGION_0;
        }

        // Otherwise.
        else
        {
            // Return region 2.
            return SIMON_DISPLAY_REGION_2;
        }
    }

    // Otherwise.
    else
    {
        // If the passed y coordinate is less than the top half of the display.
        if (y < SIMON_DISPLAY_TOP_COORDINATE)
        {
            // Return region 1.
            return SIMON_DISPLAY_REGION_1;
        }

        // Otherwise.
        else
        {
            // Return region 3.
            return SIMON_DISPLAY_REGION_3;
        }
    }
}

/*********************************************************************************/
/* Function: simonDisplay_drawButton                                             */
/* Purpose: To draw a button on the LCD screen based on the region number.       */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void simonDisplay_drawButton(uint8_t regionNumber, bool erase)
{
    // Set what the color should be (whether or not we are erasing).
    uint32_t color = erase ? SIMON_DISPLAY_BACKGROUND_COLOR : SIMON_DISPLAY_REGION_COLOR_LUT[regionNumber %= SIMON_DISPLAY_REGION_COUNT];

    // Draw or erase the button in the specified region (based on lookup tables).
    display_fillRect(SIMON_DISPLAY_REGION_X_LUT[regionNumber] - SIMON_DISPLAY_X_BUTTON_COORDINATE - SIMON_DISPLAY_BUTTON_HALF_WIDTH, SIMON_DISPLAY_REGION_Y_LUT[regionNumber] - SIMON_DISPLAY_Y_BUTTON_COORDINATE - SIMON_DISPLAY_BUTTON_HALF_HEIGHT, SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT, color);
}

/*********************************************************************************/
/* Function: simonDisplay_drawAllButtons                                         */
/* Purpose: To draw all the button on the LCD screen.                            */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void simonDisplay_drawAllButtons()
{
    // For loop to iterate through all of the regions on the LCD.
    for (int region = SIMON_DISPLAY_REGION_0; region < SIMON_DISPLAY_REGION_COUNT; region++)
    {
        // Draw the button in the current region.
        simonDisplay_drawButton(region, false);
    }
}

/*********************************************************************************/
/* Function: simonDisplay_eraseAllButtons                                        */
/* Purpose: To erase all the button on the LCD screen.                           */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void simonDisplay_eraseAllButtons()
{
    // For loop to iterate through all of the regions on the LCD.
    for (int region = SIMON_DISPLAY_REGION_0; region < SIMON_DISPLAY_REGION_COUNT; region++)
    {
        // Erase the button in the current region.
        simonDisplay_drawButton(region, true);
    }
}

/*********************************************************************************/
/* Function: simonDisplay_drawSquare                                             */
/* Purpose: To draw a square (touched button) based on the passed region number. */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void simonDisplay_drawSquare(uint8_t regionNumber, bool erase)
{
    // Set the color based on whether or not we are erasing.
    uint32_t color = erase ? SIMON_DISPLAY_BACKGROUND_COLOR : SIMON_DISPLAY_REGION_COLOR_LUT[regionNumber %= SIMON_DISPLAY_REGION_COUNT];

    // Draw or erase the square in the passed region location (based on lookup tables).
    display_fillRect(SIMON_DISPLAY_REGION_X_LUT[regionNumber] - SIMON_DISPLAY_X_BUTTON_COORDINATE - SIMON_DISPLAY_SQUARE_HALF_WIDTH, SIMON_DISPLAY_REGION_Y_LUT[regionNumber] - SIMON_DISPLAY_Y_BUTTON_COORDINATE - SIMON_DISPLAY_SQUARE_HALF_HEIGHT, SIMON_DISPLAY_SQUARE_WIDTH, SIMON_DISPLAY_SQUARE_HEIGHT, color);
}
