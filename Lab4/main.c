/**********************************************************************************/
/* File: main.c                                                                   */
/* Purpose: To run the driver for the interval timer.                             */
/* Author: Mark Harris                                                            */
/* Date: 09-20-2016                                                               */
/**********************************************************************************/

#include <stdio.h>
#include "clockControl.h"
#include "clockDisplay.h"
#include "supportFiles/utils.h"

/**********************************************************************************/
/* Function: main                                                                 */
/* Purpose: To run the test driver for the buttons and switches.                  */
/* Returns: An integer value back to the calling program.                         */
/**********************************************************************************/
int main()
{
    clockDisplay_init();
    clockDisplay_runTest();
    /*while (1)
    {
        clockControl_tick();
        utils_msDelay(1);
    }

    return 0;*/
}

void isr_function(){}
