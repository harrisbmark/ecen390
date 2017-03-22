/**********************************************************************************/
/* File: buttons.c                                                                */
/* Purpose: This file contains the necessary components to use the buttons on     */
/*          Xilinx Zybo board using C code.                                       */
/* Author: Mark Harris                                                            */
/* Date: 09-12-2016                                                               */
/**********************************************************************************/
#include "buttons.h"
#include "xil_io.h"

// Required defines specific to the necessary button addresses on the Xilinx Zybo board.
#define BUTTONS_BASE_ADDR XPAR_PUSH_BUTTONS_BASEADDR
#define BUTTONS_GPIO_DATA_OFFSET 0x0
#define BUTTONS_GPIO_TRI_OFFSET 0x04
#define BUTTONS_GPIO_OFF_STATE 0b1111

// Required defines for the required exit sequence for the buttons.
#define BUTTONS_EXIT_SEQUENCE 0xF

// Required defines for the buttons shown on the display. Define size, height, and text styling.
#define DISPLAY_BTN_COUNT 4
#define DISPLAY_BTN_WIDTH (DISPLAY_WIDTH / DISPLAY_BTN_COUNT)
#define DISPLAY_BTN_HEIGHT (DISPLAY_HEIGHT / 2)
#define DISPLAY_BTN_TEXT_HEIGHT (DISPLAY_HEIGHT / 4)
#define DISPLAY_BTN_TEXT_SIZE 2.5
#define DISPLAY_BTN_TEXT_OFFSET_PADDING 15

// Required defines for the specifics with button 3. Exact text, color, and offset.
#define DISPLAY_BTN3_BACKGROUND_COLOR DISPLAY_BLUE
#define DISPLAY_BTN3_TEXT_COLOR DISPLAY_WHITE
#define DISPLAY_BTN3_TEXT "BTN3"
#define DISPLAY_BTN3_OFFSET ((DISPLAY_WIDTH / DISPLAY_BTN_COUNT) * 0)

// Required defines for the specifics with button 2. Exact text, color, and offset.
#define DISPLAY_BTN2_BACKGROUND_COLOR 0xFFA5
#define DISPLAY_BTN2_TEXT_COLOR DISPLAY_WHITE
#define DISPLAY_BTN2_TEXT "BTN2"
#define DISPLAY_BTN2_OFFSET ((DISPLAY_WIDTH / DISPLAY_BTN_COUNT) * 1)

// Required defines for the specifics with button 1. Exact text, color, and offset.
#define DISPLAY_BTN1_BACKGROUND_COLOR DISPLAY_CYAN
#define DISPLAY_BTN1_TEXT_COLOR DISPLAY_BLACK
#define DISPLAY_BTN1_TEXT "BTN1"
#define DISPLAY_BTN1_OFFSET ((DISPLAY_WIDTH / DISPLAY_BTN_COUNT) * 2)

// Required defines for the specifics with button 0. Exact text, color, and offset.
#define DISPLAY_BTN0_BACKGROUND_COLOR DISPLAY_YELLOW
#define DISPLAY_BTN0_TEXT_COLOR DISPLAY_BLACK
#define DISPLAY_BTN0_TEXT "BTN0"
#define DISPLAY_BTN0_OFFSET ((DISPLAY_WIDTH / DISPLAY_BTN_COUNT) * 3)

/**********************************************************************************/
/* Function: buttons_readGpioRegister                                             */
/* Purpose: Allows the program to read from the GPIO register .                   */
/* Returns: The value of the GPIO register.                                       */
/**********************************************************************************/
int32_t buttons_readGpioRegister(int32_t offset)
{
    // Return the value of the buttons at the GPIO address.
    return Xil_In32(BUTTONS_BASE_ADDR + offset);
}

/**********************************************************************************/
/* Function: buttons_writeGpioRegister                                            */
/* Purpose: Allows the program to write to the GPIO register.                     */
/* Returns: Void                                                                  */
/**********************************************************************************/
void buttons_writeGpioRegister(int32_t offset, int32_t value)
{
    // Write to the push button GPIOs.
    Xil_Out32(BUTTONS_BASE_ADDR + offset, value);
}

/**********************************************************************************/
/* Function: buttons_init                                                         */
/* Purpose: Initializes the button driver software and hardware.                  */
/* Returns: A status value as defined above.                                      */
/**********************************************************************************/
int32_t buttons_init()
{
    // Write to the GPIO tri-state driver to turn the tri-state off.
    buttons_writeGpioRegister(BUTTONS_GPIO_TRI_OFFSET, BUTTONS_GPIO_OFF_STATE);

    // If the value of the GPIO's at that address is equal to the off state.
    if (buttons_readGpioRegister(BUTTONS_GPIO_TRI_OFFSET) == BUTTONS_GPIO_OFF_STATE)
    {
        // Then return an okay status.
        return BUTTONS_INIT_STATUS_OK;
    }

    // Otherwise.
    else
    {
        // Return a fail status.
        return BUTTONS_INIT_STATUS_FAIL;
    }
}

/**********************************************************************************/
/* Function: buttons_read                                                         */
/* Purpose: To read the value of the buttons on the Xilinx Zybo board.            */
/* Returns the current value of all 4 buttons as the lower 4 bits of the returned */
/*         value.                                                                 */
/**********************************************************************************/
int32_t buttons_read()
{
    // Return the value of the buttons.
    return buttons_readGpioRegister(BUTTONS_GPIO_DATA_OFFSET);
}

/**********************************************************************************/
/* Function: buttons_runTest                                                      */
/* Purpose: As you push the buttons, graphics and messages will be written to     */
/*          the LCD panel. The test will until all 4 pushbuttons are              */
/*          simultaneously pressed.                                               */
/* Returns: Void                                                                  */
/**********************************************************************************/
void buttons_runTest()
{
    // Create a current and last state to use in the test. These determine the current and last position of the buttons.
    int32_t current_state = buttons_read();
    int32_t last_state = current_state;

    // Initialize the display and set the background to black. Also set the text size and position of the cursor.
    display_init();
    display_fillScreen(DISPLAY_BLACK);
    display_setTextSize(DISPLAY_BTN_TEXT_SIZE);
    display_setCursor(0, 0);

    // While the current state is not equal to the exit button sequence.
    while (current_state != BUTTONS_EXIT_SEQUENCE)
    {
        // If the current state is not equal to the last state.
        if (current_state != last_state)
        {
            // If the current state of the buttons includes button 3 and button 3 wasn't turned on in the last state.
            if ((current_state & BUTTONS_BTN3_MASK) == BUTTONS_BTN3_MASK && (last_state & BUTTONS_BTN3_MASK) != BUTTONS_BTN3_MASK)
            {
                // Then show the button on the display with its corresponding color and text with text style.
                display_fillRect(DISPLAY_BTN3_OFFSET, 0, DISPLAY_BTN_WIDTH, DISPLAY_BTN_HEIGHT, DISPLAY_BTN3_BACKGROUND_COLOR);
                display_setCursor((DISPLAY_BTN3_OFFSET + DISPLAY_BTN_TEXT_OFFSET_PADDING), DISPLAY_BTN_TEXT_HEIGHT);
                display_setTextColor(DISPLAY_BTN3_TEXT_COLOR);
                display_print(DISPLAY_BTN3_TEXT);
            }

            // Otherwise.
            else
            {
                // If the current state does not include button 3 anymore.
                if ((current_state & BUTTONS_BTN3_MASK) != BUTTONS_BTN3_MASK)
                {
                    // Replace the button on the display with black.
                    display_fillRect(DISPLAY_BTN3_OFFSET, 0, DISPLAY_BTN_WIDTH, DISPLAY_BTN_HEIGHT, DISPLAY_BLACK);
                }
            }

            // If the current state of the buttons includes button 2 and button 2 wasn't turned on in the last state.
            if ((current_state & BUTTONS_BTN2_MASK) == BUTTONS_BTN2_MASK && (last_state & BUTTONS_BTN2_MASK) != BUTTONS_BTN2_MASK)
            {
                // Then show the button on the display with its corresponding color and text with text style.
                display_fillRect(DISPLAY_BTN2_OFFSET, 0, DISPLAY_BTN_WIDTH, DISPLAY_BTN_HEIGHT, DISPLAY_BTN2_BACKGROUND_COLOR);
                display_setCursor((DISPLAY_BTN2_OFFSET + DISPLAY_BTN_TEXT_OFFSET_PADDING), DISPLAY_BTN_TEXT_HEIGHT);
                display_setTextColor(DISPLAY_BTN2_TEXT_COLOR);
                display_print(DISPLAY_BTN2_TEXT);
            }

            // Otherwise.
            else
            {
                // If the current state does not include button 2 anymore.
                if ((current_state & BUTTONS_BTN2_MASK) != BUTTONS_BTN2_MASK)
                {
                    // Replace the button on the display with black.
                    display_fillRect(DISPLAY_BTN2_OFFSET, 0, DISPLAY_BTN_WIDTH, DISPLAY_BTN_HEIGHT, DISPLAY_BLACK);
                }
            }

            // If the current state of the buttons includes button 1 and button 1 wasn't turned on in the last state.
            if ((current_state & BUTTONS_BTN1_MASK) == BUTTONS_BTN1_MASK && (last_state & BUTTONS_BTN1_MASK) != BUTTONS_BTN1_MASK)
            {
                // Then show the button on the display with its corresponding color and text with text style.
                display_fillRect(DISPLAY_BTN1_OFFSET, 0, DISPLAY_BTN_WIDTH, DISPLAY_BTN_HEIGHT, DISPLAY_BTN1_BACKGROUND_COLOR);
                display_setCursor((DISPLAY_BTN1_OFFSET + DISPLAY_BTN_TEXT_OFFSET_PADDING), DISPLAY_BTN_TEXT_HEIGHT);
                display_setTextColor(DISPLAY_BTN1_TEXT_COLOR);
                display_print(DISPLAY_BTN1_TEXT);
            }

            // Otherwise
            else
            {
                // If the current state does not include button 1 anymore.
                if ((current_state & BUTTONS_BTN1_MASK) != BUTTONS_BTN1_MASK)
                {
                    // Replace the button on the display with black.
                    display_fillRect(DISPLAY_BTN1_OFFSET, 0, DISPLAY_BTN_WIDTH, DISPLAY_BTN_HEIGHT, DISPLAY_BLACK);
                }
            }

            // If the current state of the buttons includes button 0 and button 0 wasn't turned on in the last state.
            if ((current_state & BUTTONS_BTN0_MASK) == BUTTONS_BTN0_MASK && (last_state & BUTTONS_BTN0_MASK) != BUTTONS_BTN0_MASK)
            {
                // Then show the button on the display with its corresponding color and text with text style.
                display_fillRect(DISPLAY_BTN0_OFFSET, 0, DISPLAY_BTN_WIDTH, DISPLAY_BTN_HEIGHT, DISPLAY_BTN0_BACKGROUND_COLOR);
                display_setCursor((DISPLAY_BTN0_OFFSET + DISPLAY_BTN_TEXT_OFFSET_PADDING), DISPLAY_BTN_TEXT_HEIGHT);
                display_setTextColor(DISPLAY_BTN0_TEXT_COLOR);
                display_print(DISPLAY_BTN0_TEXT);
            }

            // Otherwise.
            else
            {
                // If the current state does not include button 0 anymore.
                if ((current_state & BUTTONS_BTN0_MASK) != BUTTONS_BTN0_MASK)
                {
                    // Replace the button on the display with black.
                    display_fillRect(DISPLAY_BTN0_OFFSET, 0, DISPLAY_BTN_WIDTH, DISPLAY_BTN_HEIGHT, DISPLAY_BLACK);
                }
            }

            // Set the last state equal to the current state.
            last_state = current_state;
        }

        // Set the current state equal to the value of the buttons.
        current_state = buttons_read();
    }

    // Fill the enter screen with black when the exit sequence is pressed.
    display_fillScreen(DISPLAY_BLACK);
}
