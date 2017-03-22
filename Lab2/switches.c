/**********************************************************************************/
/* File: switches.c                                                               */
/* Purpose: This file contains the necessary components to use the switches on    */
/*          Xilinx Zybo board using C code.                                       */
/* Author: Mark Harris                                                            */
/* Date: 09-12-2016                                                               */
/**********************************************************************************/
#include "switches.h"
#include "xil_io.h"

// Required defines specific to the necessary switch addresses on the Xilinx Zybo board.
#define SWITCHES_BASE_ADDR XPAR_SLIDE_SWITCHES_BASEADDR
#define SWITCHES_GPIO_DATA_OFFSET 0x0
#define SWITCHES_GPIO_TRI_OFFSET 0x04
#define SWITCHES_GPIO_OFF_STATE 0b1111

// Required defines for the required exit sequence for the switches.
#define SWITCHES_EXIT_SEQUENCE 0xF

/**********************************************************************************/
/* Function: switches_readGpioRegister                                            */
/* Purpose: Allows the program to read from the GPIO register .                   */
/* Returns: The value of the GPIO register.                                       */
/**********************************************************************************/
int32_t switches_readGpioRegister(int32_t offset)
{
    // Return the value from the GPIO register using the Xil_In32 function.
    return Xil_In32(SWITCHES_BASE_ADDR + offset);
}

/**********************************************************************************/
/* Function: switches_writeGpioRegister                                           */
/* Purpose: Allows the program to write to the GPIO register.                     */
/* Returns: Void                                                                  */
/**********************************************************************************/
void switches_writeGpioRegister(int32_t offset, int32_t value)
{
    // Call the Xil_Out32 function to write to the GPIOs.
    Xil_Out32(SWITCHES_BASE_ADDR + offset, value);
}

/**********************************************************************************/
/* Function: switches_init                                                        */
/* Purpose: Initializes the switch driver software and hardware.                  */
/* Returns: A status value as defined above.                                      */
/**********************************************************************************/
int32_t switches_init()
{
    // Write all 1s to the GPIO register to turn the tri-state drivers off.
    switches_writeGpioRegister(SWITCHES_GPIO_TRI_OFFSET, SWITCHES_GPIO_OFF_STATE);

    // If the value of the GPIO's at that address is equal to the off state.
    if (switches_readGpioRegister(SWITCHES_GPIO_TRI_OFFSET) == SWITCHES_GPIO_OFF_STATE)
    {
        // If so, then return an okay status.
        return SWITCHES_INIT_STATUS_OK;
    }

    // Otherwise.
    else
    {
        // Return a fail status.
        return SWITCHES_INIT_STATUS_FAIL;
    }
}

/**********************************************************************************/
/* Function: switches_read                                                        */
/* Purpose: To read the value of the switches on the Xilinx Zybo board.           */
/* Returns: The current value of all 4 switches as the lower 4 bits of the        */
/*          returned value.                                                       */
/**********************************************************************************/
int32_t switches_read()
{
    // Return the value of the GPIO switches.
    return switches_readGpioRegister(SWITCHES_GPIO_DATA_OFFSET);
}

/**********************************************************************************/
/* Function: switches_runTest                                                     */
/* Purpose: Runs a test of the switches. As you slide the switches, LEDs directly */
/*          above the switches will illuminate. The test will run until all       */
/*          switches are slid upwards. When all 4 slide switches are slid upward, */
/*          this function will exit.                                              */
/* Returns: Void                                                                  */
/**********************************************************************************/
void switches_runTest()
{
    // Create a current and last state to use in the test. These determine the current and last position of the switches.
    int32_t current_state = switches_read();
    int32_t last_state = current_state;

    // Initialize the LEDs.
    leds_init(true);

    // While the current state is not equal to the exit sequence.
    while (current_state != SWITCHES_EXIT_SEQUENCE)
    {
        // If the current is different from the last state.
        if (current_state != last_state)
        {
            // Write the current state to the LEDs and set the last state equal to the current state.
            leds_write(current_state);
            last_state = current_state;
        }

        // Set the current state to the values of the switches.
        current_state = switches_read();
    }

    // Turn all the LEDs off when the exit sequence is set.
    leds_write(SWITCHES_ZERO);
}
