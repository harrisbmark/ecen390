#include "isr.h"
#include "transmitter.h"
#include "hitLedTimer.h"
#include "lockoutTimer.h"
#include "trigger.h"

// isr provides the isr_function() where you will place functions that require accurate timing.
// A buffer for storing values from the Analog to Digital Converter (ADC) is implemented in isr.c

// Performs inits for anything in isr.c
void isr_init()
{
    transmitter_init();
    hitLedTimer_init();
    lockoutTimer_init();
    trigger_init();
}

// This function is invoked by the timer interrupt at 100 kHz.
void isr_function()
{
}

/*******************************************************************
* The functions below will be implemented in the next milestone.   *
* You can ignore them for now.                                     *
*******************************************************************/

// This is only used for testing purposes.
// Makes it possible to replace ADC data with synthetic test data.
// This is used to test the frequency responses for the filters.
void isr_addDataToAdcBuffer(uint32_t adcData){}

// This removes a value from the ADC buffer.
uint32_t isr_removeDataFromAdcBuffer(){}

// This returns the number of values in the ADC buffer.
uint32_t isr_adcBufferElementCount(){}

// Provides read access to all elements in the ADC buffer.
uint32_t isr_adcBufferReadElementAt(uint32_t index){}
