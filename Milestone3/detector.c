/*
 * detector.c
 *
 *  Created on: Mar 23, 2017
 *      Author: kellym7
 */

#include <stdio.h>
#include "supportFiles/leds.h"
#include "supportFiles/globalTimer.h"
#include "supportFiles/interrupts.h"
#include "intervalTimer.h"
#include <stdbool.h>
#include "queue.h"
#include "xparameters.h"
#include "histogram.h"
#include "transmitter.h"
#include <stdlib.h>
#include "buttons.h"
#include "switches.h"
#include "supportFiles/mio.h"
#include <string.h>
#include "supportFiles/display.h"
#include "trigger.h"
#include <stdint.h>
#include "supportFiles/utils.h"
#include "detector.h"
#include "filter.h"
#include "isr.h"
#include "lockoutTimer.h"
#include "hitLedTimer.h"
#include "sort.h"
#include "../../supportFiles/interrupts.h"
#include <stdio.h>
#define MAX_HIT_COUNT 10
#define DETECTOR_HIT_ARRAY_SIZE 10
#define ISR_CUMULATIVE_TIMER INTERVAL_TIMER_TIMER_0
//Used by the ISR.
#define TOTAL_RUNTIME_TIMER INTERVAL_TIMER_TIMER_1
//Used to compute total run-time.
#define MAIN_CUMULATIVE_TIMER INTERVAL_TIMER_TIMER_2
//Used to compute cumulative run-time in main.
#define DETECTOR_CLEAR_HIT_COUNT 0
//Clear count to 0
#define DETECTOR_HALF_QUEUE 2
//Half of a queue is 2
#define DETECTOR_MAX_ADC_VALUE 4095.0
//Maximum value for ADC
#define DETECTOR_HALF_MAX_ADC_VALUE (DETECTOR_MAX_ADC_VALUE/2.0)
//Half of the Maximum ADC value
#define DETECTOR_NEW_INPUT_COUNT_CLEAR 0
//New input clear count to 0
#define DETECTOR_MAX_NEW_INPUT_COUNT 10
//Maximum number of elements to keep track of 10
#define DETECTOR_HALF_MAX_NEW_INPUT_COUNT ((DETECTOR_MAX_NEW_INPUT_COUNT/2) - 1)
//New maximum input count for detector
#define DETECTOR_FUDGE_FACTOR 3000
//Fudge factor

volatile static bool hit_detected;
//Keep track of whether there was a hit
volatile static uint32_t adc_queue_elements_count;
//Keep track of the number of elements in the adc queue
volatile static detector_hitCount_t number_of_hits[FILTER_NUMBER_OF_PLAYERS];
//Keep track of the number of hit per player

double detector_scaled_adc_value(uint16_t value)
{
    return (value - DETECTOR_HALF_MAX_ADC_VALUE)/DETECTOR_HALF_MAX_ADC_VALUE;
    //Scaled adc value
}

//Detection algorithm to determine hits
void detector_hit_detection_algorithm()
{
    double filter_power_values[FILTER_NUMBER_OF_PLAYERS];
    //Create a vector for power values for all players
    double original_filter_power_values[FILTER_NUMBER_OF_PLAYERS];
    //Keep track of the power values for each of the players in a different vector

    for (uint16_t i = 0; i < FILTER_NUMBER_OF_PLAYERS; i++)
    {
        filter_power_values[i] = filter_getCurrentPowerValue(i);
        //Fill power value vector with current power values
        original_filter_power_values[i] = filter_power_values[i];
        //Set original vector with current power value
    }

    quicksort(filter_power_values, FILTER_NUMBER_OF_PLAYERS);
    //Call quicksort function for each power value for all players

    for (int8_t i = 0; i < FILTER_NUMBER_OF_PLAYERS; i++)
    {
        //if original power value is greater than sorted power value multiplied by our fudge factor
        if (original_filter_power_values[i] > filter_power_values[DETECTOR_HALF_MAX_NEW_INPUT_COUNT] * DETECTOR_FUDGE_FACTOR)
        {
            //Set hit detected to true
            hit_detected = true;
            //Increment number of hits for that player
            number_of_hits[i]++;
            //Start lockout timer
            lockoutTimer_start();
            //Start hit led timer
            hitLedTimer_start();

            break;
        }
    }
}

// Always have to init things.
void detector_init()
{
    for (uint8_t i = 0; i < FILTER_NUMBER_OF_PLAYERS; i++)
    {
        number_of_hits[i] = DETECTOR_CLEAR_HIT_COUNT;
        //Set player hit count to 0 for each player
    }
}

// Runs the entire detector: decimating fir-filter, iir-filters, power-computation, hit-detection.
// if interruptsEnabled = false, interrupts are not running. If interruptsEnabled = false
// you can pop values from the ADC queue without disabling interrupts.
// If interruptsEnabled = true, do the following:
// 1. disable interrupts.
// 2. pop the value from the ADC queue.
// 3. re-enable interrupts.
// Use this to determine whether you should disable and re-enable interrrupts when accessing the adc queue.
// if ignoreSelf == true, ignore hits that are detected on your frequency.
// Your frequency is simply the frequency indicated by the slide switches.
void detector(bool interruptsEnabled, bool ignoreSelf)
{
    static uint8_t add_new_input_count = DETECTOR_NEW_INPUT_COUNT_CLEAR;
    //We made this value static so it keeps it's value.
    adc_queue_elements_count = isr_adcBufferElementCount();
    //We have a variable that keeps track of the elements.
    uint16_t raw_value;
    //This variable keeps track of the value that we are getting rid of.

    for (uint32_t i = 0; i < adc_queue_elements_count; i++)
    {
        if (interruptsEnabled) //Interrupts are enabled
        {
            interrupts_disableArmInts(); //Disable interrupts briefly
            raw_value = isr_removeDataFromAdcBuffer(); //Remove value from buffer
            interrupts_enableArmInts(); //Re-enable interrupts
        }

        else
        {
            raw_value = isr_removeDataFromAdcBuffer();
            //Set raw value to remove from buffer
        }

        filter_addNewInput(detector_scaled_adc_value(raw_value));
        //Add new scaled input to filter
        add_new_input_count++;
        //Increment count

        if (add_new_input_count >= DETECTOR_MAX_NEW_INPUT_COUNT) //Input count greater than 10
        {
            add_new_input_count = DETECTOR_NEW_INPUT_COUNT_CLEAR;
            //Clear input count
            filter_firFilter();
            //Call FIR filter
            for (uint16_t j = 0; j < FILTER_NUMBER_OF_PLAYERS; j++)
            {
                filter_iirFilter(j);
                //Call iir filter for every player
                filter_computePower(j, false, false);
                //Compute power for each player
            }

            if (!lockoutTimer_running())
            {
                detector_hit_detection_algorithm();
                //Call hit detection algorithm
            }
        }
    }
}

// Returns true if a hit was detected.
bool detector_hitDetected()
{
    return hit_detected;
    //Return if there was a hit or not
}

// Clear the detected hit once you have accounted for it.
void detector_clearHit()
{
    hit_detected = false;
    //Set hit detected to false
}

// Get the current hit counts.
// Copy the current hit counts into the user-provided hitArray
// using a for-loop.
void detector_getHitCounts(detector_hitCount_t hitArray[])
{
    for (uint8_t i = 0; i < FILTER_NUMBER_OF_PLAYERS; i++)
    {
        hitArray[i] = number_of_hits[i];
        //Fill array given with the number of hits array that we've been keeping track of
    }
}

void detector_runTest()
{
    uint16_t hit_count = DETECTOR_CLEAR_HIT_COUNT;
    //Have a hit count variable initialized to 0
    buttons_init();
    //Initialize buttons
    switches_init();
    //Initialize switches
    mio_init(false);
    //Initialize mio pin
    intervalTimer_initAll();
    //Initialize interval timers
    leds_init(true);
    //Initialize leds
    transmitter_init();
    //Initialize transmitter
    detector_init();
    //Initialize detector
    filter_init();
    //Initialize filter
    isr_init();
    //Initialize isr
    lockoutTimer_init();
    //Initialize lockout timer
    hitLedTimer_init();
    //Initialize hit led timer
    trigger_init();
    //Initialize trigger
    trigger_enable();
    //Makes the trigger state machine responsive to the trigger.
    interrupts_enableTimerGlobalInts();       //Allows the timer to generate interrupts.
    interrupts_startArmPrivateTimer();        //Start the private ARM timer running.
    intervalTimer_reset(ISR_CUMULATIVE_TIMER);  //Used to measure ISR execution time.
    intervalTimer_reset(TOTAL_RUNTIME_TIMER);   //Used to measure total program execution time.
    intervalTimer_reset(MAIN_CUMULATIVE_TIMER); //Used to measure main-loop execution time.
    intervalTimer_start(TOTAL_RUNTIME_TIMER);   //Start measuring total execution time.
    interrupts_enableArmInts();       //The ARM will start seeing interrupts after this.
    lockoutTimer_start();                 //Ignore erroneous hits at startup (when all power values are essentially 0).

    while ((!(buttons_read() & BUTTONS_BTN3_MASK)) && hit_count < MAX_HIT_COUNT) { //Run until you detect btn3 pressed.
        if (interrupts_isrFlagGlobal) {  //Only do something if an interrupt has occurred.
            detector(true, false);    //true, false means: interrupts are enabled, do not ignore your set frequency.
            if (detector_hitDetected()) {  //Hit detected
                hit_count++;  //Increment the hit count.
                detector_clearHit();  //Clear the hit.
                detector_hitCount_t hitCounts[DETECTOR_HIT_ARRAY_SIZE]; //Store the hit-counts here.
                detector_getHitCounts(hitCounts);  //Get the current hit counts.
            }
            uint16_t switchValue = switches_read();   //Read the switches and switch frequency as required.
            transmitter_setFrequencyNumber(switchValue);
        }
        intervalTimer_stop(MAIN_CUMULATIVE_TIMER);  //All done with actual processing.
    }
    interrupts_disableArmInts();  //Done with loop, disable the interrupts.
}
