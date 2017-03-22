/**********************************************************************************/
/* File: buttons.h                                                                */
/* Purpose: This file contains the necessary components to use the buttons on     */
/*          Xilinx Zybo board using C code.                                       */
/* Author: Mark Harris                                                            */
/* Date: 09-12-2016                                                               */
/**********************************************************************************/
#ifndef SRC_LAB2_BUTTONS_H_
#define SRC_LAB2_BUTTONS_H_

// Necessary includes.
#include <stdint.h>
#include "supportFiles/display.h"
#include "xparameters.h"

// Required defines specific to button states and masks.
#define BUTTONS_INIT_STATUS_OK 1
#define BUTTONS_INIT_STATUS_FAIL 0

// Required defines used to mask the buttons (used to determine what button is being pressed).
#define BUTTONS_BTN0_MASK 0x1
#define BUTTONS_BTN1_MASK 0x2
#define BUTTONS_BTN2_MASK 0x4
#define BUTTONS_BTN3_MASK 0x8

// Required define to set the buttons to zero.
#define BUTTONS_ZERO 0x0

/**********************************************************************************/
/* Function: buttons_init                                                         */
/* Purpose: Initializes the button driver software and hardware.                  */
/* Returns: A status value as defined above.                                      */
/**********************************************************************************/
int32_t buttons_init();

/**********************************************************************************/
/* Function: buttons_read                                                         */
/* Purpose: To read the value of the buttons on the Xilinx Zybo board.            */
/* Returns the current value of all 4 buttons as the lower 4 bits of the returned */
/*         value.                                                                 */
/**********************************************************************************/
int32_t buttons_read();

/**********************************************************************************/
/* Function: buttons_runTest                                                      */
/* Purpose: As you push the buttons, graphics and messages will be written to     */
/*          the LCD panel. The test will until all 4 pushbuttons are              */
/*          simultaneously pressed.                                               */
/* Returns: Void                                                                  */
/**********************************************************************************/
void buttons_runTest();

#endif
