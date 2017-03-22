/*********************************************************************************/
/* File: globals.c                                                               */
/* Purpose: To provide the code necessary to share a Simon sequence across all   */
/*          state machines used in the Simon game.                               */
/* Author: Mark Harris                                                           */
/* Date: 11-16-16                                                                */
/*********************************************************************************/
#include "globals.h"

// Global variables used for the sequence.
static uint16_t globals_sequenceLength = 0;
static uint16_t globals_sequenceIterationLength = 0;
static uint8_t globals_sequence[GLOBALS_MAX_FLASH_SEQUENCE];

/*********************************************************************************/
/* Function: globals_setSequence                                                 */
/* Purpose: To set the sequence to be used for the Simon game.                   */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void globals_setSequence(const uint8_t sequence[], uint16_t length)
{
    // For loop to iterate through the length of the sequence.
    for (uint8_t i = 0; i < length; i++)
    {
        // Set the current index of the globals sequence to the current index of the passed sequence.
        globals_sequence[i] = sequence[i];
    }

    // Set the globals sequence length to the passed length.
    globals_sequenceLength = length;
}

/*********************************************************************************/
/* Function: globals_getSequenceValue                                            */
/* Purpose: To get the value of the globals sequence at a passed index.          */
/* Returns: An 8 bit integer value.                                              */
/*********************************************************************************/
uint8_t globals_getSequenceValue(uint16_t index)
{
    // Return the value of the globals sequence at the passed index.
    return globals_sequence[index];
}

/*********************************************************************************/
/* Function: globals_setSequenceLength                                           */
/* Purpose: To get the length of the globals sequence.                           */
/* Returns: A 16 bit integer value.                                              */
/*********************************************************************************/
uint16_t globals_getSequenceLength()
{
    // Return the globals sequence length.
    return globals_sequenceLength;
}

/*********************************************************************************/
/* Function: globals_setSequenceIterationLength                                  */
/* Purpose: To set the length of the current globals iteration to a passed       */
/*          length.                                                              */
/* Returns: VOID                                                                 */
/*********************************************************************************/
void globals_setSequenceIterationLength(uint16_t length)
{
    // Set the globals sequence iteration length to the passed length.
    globals_sequenceIterationLength = length;
}

/*********************************************************************************/
/* Function: globals_getSequenceIterationLength                                  */
/* Purpose: To get the length of the current globals sequence iteration.         */
/* Returns: A 16 bit integer value.                                              */
/*********************************************************************************/
uint16_t globals_getSequenceIterationLength()
{
    // Return the globals sequence iteration length.
    return globals_sequenceIterationLength;
}
