/*********************************************************************************/
/* File: ticTacToeDisplay.c                                                      */
/* Purpose: To provide the functions necessary to show the tic-tac-toe game on   */
/*          the Zybo board LCD display.                                          */
/* Author: Mark Harris                                                           */
/* Date: 10-25-16                                                                */
/*********************************************************************************/
#include "ticTacToeDisplay.h"
#include "supportFiles/display.h"
#include "../lab2/buttons.h"
#include "../lab2/switches.h"
#include "supportFiles/utils.h"
#include <math.h>

// Necessary defines to set the background and text color for the tic-tac-toe game.
#define TIC_TAC_TOE_DISPLAY_BACKGROUND_COLOR DISPLAY_BLACK
#define TIC_TAC_TOE_DISPLAY_GAME_COLOR DISPLAY_GREEN

// Necessary defines to set the display width, height, and other special dimensions and locations.
#define TIC_TAC_TOE_DISPLAY_WIDTH DISPLAY_WIDTH
#define TIC_TAC_TOE_DISPLAY_HEIGHT DISPLAY_HEIGHT
#define TIC_TAC_TOE_DISPLAY_HEIGHT_HALF TIC_TAC_TOE_DISPLAY_HEIGHT / 2.25
#define TIC_TAC_TOE_DISPLAY_ABSOLUTE_TOP 0
#define TIC_TAC_TOE_DISPLAY_ABSOLUTE_LEFT 0

#define TIC_TAC_TOE_DISPLAY_NEXT_SQUARE 1
#define TIC_TAC_TOE_DISPLAY_DEFAULT_SQUARE 0

// Necessary defines to give the padding and width of the tic-tac-toe squares.
#define TIC_TAC_TOE_DISPLAY_PADDING 10
#define TIC_TAC_TOE_DISPLAY_SQUARE_WIDTH TIC_TAC_TOE_DISPLAY_WIDTH / TIC_TAC_TOE_DISPLAY_COLUMNS
#define TIC_TAC_TOE_DISPLAY_SQUARE_HEIGHT TIC_TAC_TOE_DISPLAY_HEIGHT / TIC_TAC_TOE_DISPLAY_ROWS

// Necessary defines to set the number of rows, columns, and locations of the mid points on the LCD screen.
#define TIC_TAC_TOE_DISPLAY_ROWS 3
#define TIC_TAC_TOE_DISPLAY_COLUMNS 3
#define TIC_TAC_TOE_DISPLAY_ROWS_HALF 2
#define TIC_TAC_TOE_DISPLAY_COLUMNS_HALF 2

// Necessary defines to set the size of the "characters" used on the display (X and O).
#define TIC_TAC_TOE_DISPLAY_CHARACTER_RADIUS TIC_TAC_TOE_DISPLAY_SQUARE_HEIGHT / 2 - TIC_TAC_TOE_DISPLAY_PADDING
#define TIC_TAC_TOE_DISPLAY_CHARACTER_WIDTH_HEIGHT ((TIC_TAC_TOE_DISPLAY_SQUARE_HEIGHT - 4 * TIC_TAC_TOE_DISPLAY_PADDING) / 2) * sqrt(2.0)

// Necessary defines to set the text size and padding.
#define TIC_TAC_TOE_DISPLAY_TEXT_SIZE 5
#define TIC_TAC_TOE_DISPLAY_TEXT_PADDING 15

// Necessary defines to special sequences and delays used with the display.
#define TIC_TAC_TOE_DISPLAY_EXIT_SEQUENCE 0b0010
#define TIC_TAC_TOE_DISPLAY_CLEAR_SEQUENCE 0b0001
#define TIC_TAC_TOE_DISPLAY_IS_O 0b0001
#define TIC_TAC_TOE_DISPLAY_TOUCH_DELAY 50

/*********************************************************************************/
/* Function: ticTacToeDisplay_init                                               */
/* Purpose: To initialize the LCD display in order to use it for the game.       */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void ticTacToeDisplay_init()
{
    // Initialize the display and fill the screen with the background color.
    display_init();
    display_fillScreen(TIC_TAC_TOE_DISPLAY_BACKGROUND_COLOR);

    // Set the text size and color and set the location of the cursor for text.
    display_setTextSize(TIC_TAC_TOE_DISPLAY_TEXT_SIZE);
    display_setTextColor(TIC_TAC_TOE_DISPLAY_GAME_COLOR);
    display_setCursor(TIC_TAC_TOE_DISPLAY_ABSOLUTE_LEFT + TIC_TAC_TOE_DISPLAY_TEXT_PADDING, TIC_TAC_TOE_DISPLAY_HEIGHT_HALF);
}

/*********************************************************************************/
/* Function: ticTacToeDisplay_reset                                              */
/* Purpose: To reset the display back to blank squares.                          */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void ticTacToeDisplay_reset()
{
    // For every column in the tic-tac-toe display.
    for (uint8_t column = TIC_TAC_TOE_DISPLAY_DEFAULT_SQUARE; column < TIC_TAC_TOE_DISPLAY_COLUMNS; column++)
    {
        // For every row in the tic-tac-toe display.
        for (uint8_t row = TIC_TAC_TOE_DISPLAY_DEFAULT_SQUARE; row < TIC_TAC_TOE_DISPLAY_ROWS; row++)
        {
            // Draw a "blank" X and O in order to "erase" any leftover characters.
            ticTacToeDisplay_drawX(row, column, true);
            ticTacToeDisplay_drawO(row, column, true);
        }
    }
}

/*********************************************************************************/
/* Function: ticTacToeDisplay_drawX                                              */
/* Purpose: To draw an X at the specified location on the LCD screen.            */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void ticTacToeDisplay_drawX(uint8_t row, uint8_t column, bool erase)
{
    // Set the text color.
    uint16_t color = TIC_TAC_TOE_DISPLAY_GAME_COLOR;

    // If the erase flag is set, then change the text color to the background color.
    if (erase)
    {
        color = TIC_TAC_TOE_DISPLAY_BACKGROUND_COLOR;
    }

    // Draw an X on the screen by drawing four lines centered at the middle of the row and column specified and spanning out at 45 degree angles.
    display_drawLine((column % TIC_TAC_TOE_DISPLAY_COLUMNS + TIC_TAC_TOE_DISPLAY_NEXT_SQUARE / TIC_TAC_TOE_DISPLAY_COLUMNS_HALF) * TIC_TAC_TOE_DISPLAY_SQUARE_WIDTH + TIC_TAC_TOE_DISPLAY_SQUARE_WIDTH / TIC_TAC_TOE_DISPLAY_COLUMNS_HALF, (row % TIC_TAC_TOE_DISPLAY_ROWS + TIC_TAC_TOE_DISPLAY_NEXT_SQUARE / TIC_TAC_TOE_DISPLAY_ROWS_HALF) * TIC_TAC_TOE_DISPLAY_SQUARE_HEIGHT + TIC_TAC_TOE_DISPLAY_SQUARE_HEIGHT / TIC_TAC_TOE_DISPLAY_ROWS_HALF, (column % TIC_TAC_TOE_DISPLAY_COLUMNS + TIC_TAC_TOE_DISPLAY_NEXT_SQUARE / TIC_TAC_TOE_DISPLAY_COLUMNS_HALF) * TIC_TAC_TOE_DISPLAY_SQUARE_WIDTH + TIC_TAC_TOE_DISPLAY_SQUARE_WIDTH / TIC_TAC_TOE_DISPLAY_COLUMNS_HALF - TIC_TAC_TOE_DISPLAY_CHARACTER_WIDTH_HEIGHT, (row % TIC_TAC_TOE_DISPLAY_ROWS + TIC_TAC_TOE_DISPLAY_NEXT_SQUARE / TIC_TAC_TOE_DISPLAY_ROWS_HALF) * TIC_TAC_TOE_DISPLAY_SQUARE_HEIGHT + TIC_TAC_TOE_DISPLAY_SQUARE_HEIGHT / TIC_TAC_TOE_DISPLAY_ROWS_HALF + TIC_TAC_TOE_DISPLAY_CHARACTER_WIDTH_HEIGHT, color);
    display_drawLine((column % TIC_TAC_TOE_DISPLAY_COLUMNS + TIC_TAC_TOE_DISPLAY_NEXT_SQUARE / TIC_TAC_TOE_DISPLAY_COLUMNS_HALF) * TIC_TAC_TOE_DISPLAY_SQUARE_WIDTH + TIC_TAC_TOE_DISPLAY_SQUARE_WIDTH / TIC_TAC_TOE_DISPLAY_COLUMNS_HALF, (row % TIC_TAC_TOE_DISPLAY_ROWS + TIC_TAC_TOE_DISPLAY_NEXT_SQUARE / TIC_TAC_TOE_DISPLAY_ROWS_HALF) * TIC_TAC_TOE_DISPLAY_SQUARE_HEIGHT + TIC_TAC_TOE_DISPLAY_SQUARE_HEIGHT / TIC_TAC_TOE_DISPLAY_ROWS_HALF, (column % TIC_TAC_TOE_DISPLAY_COLUMNS + TIC_TAC_TOE_DISPLAY_NEXT_SQUARE / TIC_TAC_TOE_DISPLAY_COLUMNS_HALF) * TIC_TAC_TOE_DISPLAY_SQUARE_WIDTH + TIC_TAC_TOE_DISPLAY_SQUARE_WIDTH / TIC_TAC_TOE_DISPLAY_COLUMNS_HALF - TIC_TAC_TOE_DISPLAY_CHARACTER_WIDTH_HEIGHT, (row % TIC_TAC_TOE_DISPLAY_ROWS + TIC_TAC_TOE_DISPLAY_NEXT_SQUARE / TIC_TAC_TOE_DISPLAY_ROWS_HALF) * TIC_TAC_TOE_DISPLAY_SQUARE_HEIGHT + TIC_TAC_TOE_DISPLAY_SQUARE_HEIGHT / TIC_TAC_TOE_DISPLAY_ROWS_HALF - TIC_TAC_TOE_DISPLAY_CHARACTER_WIDTH_HEIGHT, color);
    display_drawLine((column % TIC_TAC_TOE_DISPLAY_COLUMNS + TIC_TAC_TOE_DISPLAY_NEXT_SQUARE / TIC_TAC_TOE_DISPLAY_COLUMNS_HALF) * TIC_TAC_TOE_DISPLAY_SQUARE_WIDTH + TIC_TAC_TOE_DISPLAY_SQUARE_WIDTH / TIC_TAC_TOE_DISPLAY_COLUMNS_HALF, (row % TIC_TAC_TOE_DISPLAY_ROWS + TIC_TAC_TOE_DISPLAY_NEXT_SQUARE / TIC_TAC_TOE_DISPLAY_ROWS_HALF) * TIC_TAC_TOE_DISPLAY_SQUARE_HEIGHT + TIC_TAC_TOE_DISPLAY_SQUARE_HEIGHT / TIC_TAC_TOE_DISPLAY_ROWS_HALF, (column % TIC_TAC_TOE_DISPLAY_COLUMNS + TIC_TAC_TOE_DISPLAY_NEXT_SQUARE / TIC_TAC_TOE_DISPLAY_COLUMNS_HALF) * TIC_TAC_TOE_DISPLAY_SQUARE_WIDTH + TIC_TAC_TOE_DISPLAY_SQUARE_WIDTH / TIC_TAC_TOE_DISPLAY_COLUMNS_HALF + TIC_TAC_TOE_DISPLAY_CHARACTER_WIDTH_HEIGHT, (row % TIC_TAC_TOE_DISPLAY_ROWS + TIC_TAC_TOE_DISPLAY_NEXT_SQUARE / TIC_TAC_TOE_DISPLAY_ROWS_HALF) * TIC_TAC_TOE_DISPLAY_SQUARE_HEIGHT + TIC_TAC_TOE_DISPLAY_SQUARE_HEIGHT / TIC_TAC_TOE_DISPLAY_ROWS_HALF + TIC_TAC_TOE_DISPLAY_CHARACTER_WIDTH_HEIGHT, color);
    display_drawLine((column % TIC_TAC_TOE_DISPLAY_COLUMNS + TIC_TAC_TOE_DISPLAY_NEXT_SQUARE / TIC_TAC_TOE_DISPLAY_COLUMNS_HALF) * TIC_TAC_TOE_DISPLAY_SQUARE_WIDTH + TIC_TAC_TOE_DISPLAY_SQUARE_WIDTH / TIC_TAC_TOE_DISPLAY_COLUMNS_HALF, (row % TIC_TAC_TOE_DISPLAY_ROWS + TIC_TAC_TOE_DISPLAY_NEXT_SQUARE / TIC_TAC_TOE_DISPLAY_ROWS_HALF) * TIC_TAC_TOE_DISPLAY_SQUARE_HEIGHT + TIC_TAC_TOE_DISPLAY_SQUARE_HEIGHT / TIC_TAC_TOE_DISPLAY_ROWS_HALF, (column % TIC_TAC_TOE_DISPLAY_COLUMNS + TIC_TAC_TOE_DISPLAY_NEXT_SQUARE / TIC_TAC_TOE_DISPLAY_COLUMNS_HALF) * TIC_TAC_TOE_DISPLAY_SQUARE_WIDTH + TIC_TAC_TOE_DISPLAY_SQUARE_WIDTH / TIC_TAC_TOE_DISPLAY_COLUMNS_HALF + TIC_TAC_TOE_DISPLAY_CHARACTER_WIDTH_HEIGHT, (row % TIC_TAC_TOE_DISPLAY_ROWS + TIC_TAC_TOE_DISPLAY_NEXT_SQUARE / TIC_TAC_TOE_DISPLAY_ROWS_HALF) * TIC_TAC_TOE_DISPLAY_SQUARE_HEIGHT + TIC_TAC_TOE_DISPLAY_SQUARE_HEIGHT / TIC_TAC_TOE_DISPLAY_ROWS_HALF - TIC_TAC_TOE_DISPLAY_CHARACTER_WIDTH_HEIGHT, color);
}

/*********************************************************************************/
/* Function: ticTacToeDisplay_drawO                                              */
/* Purpose: To draw an O at the specified location on the LCD screen.            */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void ticTacToeDisplay_drawO(uint8_t row, uint8_t column, bool erase)
{
    // Set the text color.
    uint16_t color = TIC_TAC_TOE_DISPLAY_GAME_COLOR;

    // If the erase flag is set, then change the text color to the background color.
    if (erase)
    {
        color = TIC_TAC_TOE_DISPLAY_BACKGROUND_COLOR;
    }

    // Draw an O on the screen by drawing a circle that radiates out from the center of the speicified square at the row and column passed.
    display_drawCircle((column % TIC_TAC_TOE_DISPLAY_COLUMNS + TIC_TAC_TOE_DISPLAY_NEXT_SQUARE / TIC_TAC_TOE_DISPLAY_COLUMNS_HALF) * TIC_TAC_TOE_DISPLAY_SQUARE_WIDTH + TIC_TAC_TOE_DISPLAY_SQUARE_WIDTH / TIC_TAC_TOE_DISPLAY_COLUMNS_HALF, (row % TIC_TAC_TOE_DISPLAY_ROWS + TIC_TAC_TOE_DISPLAY_NEXT_SQUARE / TIC_TAC_TOE_DISPLAY_ROWS_HALF) * TIC_TAC_TOE_DISPLAY_SQUARE_HEIGHT + TIC_TAC_TOE_DISPLAY_SQUARE_HEIGHT / TIC_TAC_TOE_DISPLAY_ROWS_HALF, TIC_TAC_TOE_DISPLAY_CHARACTER_RADIUS, color);
}

/*********************************************************************************/
/* Function: ticTacToeDisplay_touchScreenComputeBoardRowColumn                   */
/* Purpose: To get the touch point on the LCD screen.                            */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void ticTacToeDisplay_touchScreenComputeBoardRowColumn(uint8_t* row, uint8_t* column)
{
    // Create and set the x, y, and intensity of the touch display to 0.
    int16_t display_touch_x = 0;
    int16_t display_touch_y = 0;
    uint8_t display_touch_intensity = 0;

    // Get the touched points from the display.
    display_getTouchedPoint(&display_touch_x, &display_touch_y, &display_touch_intensity);

    // For every row (y) in the tic-tac-toe display.
    for (uint8_t y = TIC_TAC_TOE_DISPLAY_DEFAULT_SQUARE; y < TIC_TAC_TOE_DISPLAY_ROWS; y++)
    {
        // If the touched point is less than the specified row (starting at the next square), set the row to that value and break the loop.
        if (display_touch_y < ((y + TIC_TAC_TOE_DISPLAY_NEXT_SQUARE) * TIC_TAC_TOE_DISPLAY_SQUARE_HEIGHT))
        {
            *row = y;
            break;
        }
    }

    // For every column (x) in the tic-tac-toe display.
    for (uint8_t x = TIC_TAC_TOE_DISPLAY_DEFAULT_SQUARE; x < TIC_TAC_TOE_DISPLAY_COLUMNS; x++)
    {
        // If the touched point is less than the specified column (starting at the next square), set the column to that value and break the loop.
        if (display_touch_x < ((x + TIC_TAC_TOE_DISPLAY_NEXT_SQUARE) * TIC_TAC_TOE_DISPLAY_SQUARE_WIDTH))
        {
            *column = x;
            break;
        }
    }
}

/*********************************************************************************/
/* Function: ticTacToeDisplay_runTest                                            */
/* Purpose: To get the touch point on the LCD screen.                            */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void ticTacToeDisplay_runTest()
{
    // Create a row and column variable to be used for the test.
    uint8_t row;
    uint8_t column;

    // Initialize the buttons, the switches, and set up the tic-tac-toe display.
    buttons_init();
    switches_init();
    ticTacToeDisplay_init();
    ticTacToeDisplay_drawBoardLines();

    // While we don't get the exit button sequence.
    while ((buttons_read() & TIC_TAC_TOE_DISPLAY_EXIT_SEQUENCE) != TIC_TAC_TOE_DISPLAY_EXIT_SEQUENCE)
    {
        // If the reset button sequence is detected, reset the display.
        if ((buttons_read() & TIC_TAC_TOE_DISPLAY_CLEAR_SEQUENCE) == TIC_TAC_TOE_DISPLAY_CLEAR_SEQUENCE)
        {
            ticTacToeDisplay_reset();
        }

        // Otherwise.
        else
        {
            // If the display is touched.
            if (display_isTouched())
            {
                // Clear the old touch data and wait for a specified delay.
                display_clearOldTouchData();
                utils_msDelay(TIC_TAC_TOE_DISPLAY_TOUCH_DELAY);

                // If the display is still being touched.
                if (display_isTouched())
                {
                    // Get the touched point from the LCD.
                    ticTacToeDisplay_touchScreenComputeBoardRowColumn(&row, &column);

                    // If the switches indicate to draw an O, go ahead and do so.
                    if ((switches_read() & TIC_TAC_TOE_DISPLAY_IS_O) == TIC_TAC_TOE_DISPLAY_IS_O)
                    {
                        ticTacToeDisplay_drawO(row, column, false);
                    }

                    // Otherwise, draw an X.
                    else
                    {
                        ticTacToeDisplay_drawX(row, column, false);
                    }

                    // While the display is still being touched, wait for a specified delay.
                    while (display_isTouched())
                    {
                        utils_msDelay(TIC_TAC_TOE_DISPLAY_TOUCH_DELAY);
                    }
                }
            }
        }
    }

    // When the exit sequence is pressed, show "GAME OVER" to the user.
    display_print("GAME OVER!");
}

/*********************************************************************************/
/* Function: ticTacToeDisplay_drawBoardLines                                     */
/* Purpose: To draw the tic-tac-toe board on the LCD display.                    */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void ticTacToeDisplay_drawBoardLines()
{
    // For ever column in the tic-tac-toe board, draw a vertical line at increments specified in the above defines.
    for (uint8_t column = TIC_TAC_TOE_DISPLAY_NEXT_SQUARE; column < TIC_TAC_TOE_DISPLAY_COLUMNS; column++)
    {
        display_drawFastVLine(column * TIC_TAC_TOE_DISPLAY_SQUARE_WIDTH, TIC_TAC_TOE_DISPLAY_ABSOLUTE_TOP, TIC_TAC_TOE_DISPLAY_HEIGHT, TIC_TAC_TOE_DISPLAY_GAME_COLOR);
    }

    // For ever row in the tic-tac-toe board, draw a horizontal line at increments specified in the above defines.
    for (uint8_t row = TIC_TAC_TOE_DISPLAY_NEXT_SQUARE; row < TIC_TAC_TOE_DISPLAY_ROWS; row++)
    {
        display_drawFastHLine(TIC_TAC_TOE_DISPLAY_ABSOLUTE_LEFT, row * TIC_TAC_TOE_DISPLAY_SQUARE_HEIGHT, TIC_TAC_TOE_DISPLAY_WIDTH, TIC_TAC_TOE_DISPLAY_GAME_COLOR);
    }
}
