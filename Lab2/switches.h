/**********************************************************************************/
/* File: switches.h                                                               */
/* Purpose: This file contains the necessary components to use the switches on    */
/*          Xilinx Zybo board using C code.                                       */
/* Author: Mark Harris                                                            */
/* Date: 09-12-2016                                                               */
/**********************************************************************************/
#ifndef SRC_LAB2_SWITCHES_H_
#define SRC_LAB2_SWITCHES_H_

// Necessary includes.
#include <stdint.h>
#include "supportFiles/leds.h"
#include "xparameters.h"

// Required defines specific to switch states and masks.
#define SWITCHES_INIT_STATUS_OK 1
#define SWITCHES_INIT_STATUS_FAIL 0

// Required defines for the switch masks (used to determine what switch is being used).
#define SWITCHES_SW0_MASK 0x1
#define SWITCHES_SW1_MASK 0x2
#define SWITCHES_SW2_MASK 0x4
#define SWITCHES_SW3_MASK 0x8

// Required define to set the switches to zero.
#define SWITCHES_ZERO 0x0

/**********************************************************************************/
/* Function: switches_init                                                        */
/* Purpose: Initializes the switch driver software and hardware.                  */
/* Returns: A status value as defined above.                                      */
/**********************************************************************************/
int32_t switches_init();

/**********************************************************************************/
/* Function: switches_read                                                        */
/* Purpose: To read the value of the switches on the Xilinx Zybo board.           */
/* Returns: The current value of all 4 switches as the lower 4 bits of the        */
/*          returned value.                                                       */
/**********************************************************************************/
int32_t switches_read();

/**********************************************************************************/
/* Function: switches_runTest                                                     */
/* Purpose: Runs a test of the switches. As you slide the switches, LEDs directly */
/*          above the switches will illuminate. The test will run until all       */
/*          switches are slid upwards. When all 4 slide switches are slid upward, */
/*          this function will exit.                                              */
/* Returns: Void                                                                  */
/**********************************************************************************/
void switches_runTest();

#endif
