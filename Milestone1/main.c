#include <stdio.h>
#include "queue.h"

/**********************************************************************************/
/* Function: main                                                                 */
/* Purpose: To run the "Milestone 1" code.                                        */
/* Returns: An integer value (back to the calling function).                      */
/**********************************************************************************/
int main()
{
    // Run the queue test and return 0.
    queue_runTest();
    return 0;
}

// This function must be defined but can be left empty for now.
// You will use this function in a later lab.
// It is called in the timer interrupt service routine (see interrupts.c in supportFiles).
void isr_function() {}
