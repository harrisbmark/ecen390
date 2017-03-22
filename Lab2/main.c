/**********************************************************************************/
/* File: main.c                                                                   */
/* Purpose: To run the driver for the button and switches.                        */
/* Author: Mark Harris                                                            */
/* Date: 09-12-2016                                                               */
/**********************************************************************************/
#include <stdio.h>
#include "buttons.h"
#include "switches.h"

/**********************************************************************************/
/* Function: main                                                                 */
/* Purpose: To run the test driver for the buttons and switches.                  */
/* Returns: An integer value back to the calling program.                         */
/**********************************************************************************/
int main()
{
    // Run the test for the buttons and switches.
    buttons_runTest();
    switches_runTest();

    // Return back to the calling program.
    return 0;
}

void isr_function()
{
    // Empty.
}
