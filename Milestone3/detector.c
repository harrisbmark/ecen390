#include "detector.h"
#include "filter.h"
#include "isr.h"
#include "lockoutTimer.h"
#include "hitLedTimer.h"
#include "sort.h"
#include "../../supportFiles/interrupts.h"
#include <stdio.h>

#define DETECTOR_CLEAR_HIT_COUNT 0
#define DETECTOR_HALF_QUEUE 2
#define DETECTOR_MAX_ADC_VALUE 4095.0
#define DETECTOR_HALF_MAX_ADC_VALUE (DETECTOR_MAX_ADC_VALUE/2.0)
#define DETECTOR_NEW_INPUT_COUNT_CLEAR 0
#define DETECTOR_MAX_NEW_INPUT_COUNT 10
#define DETECTOR_HALF_MAX_NEW_INPUT_COUNT ((DETECTOR_MAX_NEW_INPUT_COUNT/2) - 1)
#define DETECTOR_FUDGE_FACTOR 5

volatile static bool hit_detected;
volatile static uint32_t adc_queue_elements_count;
volatile static detector_hitCount_t number_of_hits[FILTER_NUMBER_OF_PLAYERS];

double detector_scaled_adc_value(uint16_t value)
{
    return (value - DETECTOR_HALF_MAX_ADC_VALUE)/DETECTOR_HALF_MAX_ADC_VALUE;
}

void detector_hit_detection_algorithm()
{
    double filter_power_values[FILTER_NUMBER_OF_PLAYERS];
    double original_filter_power_values[FILTER_NUMBER_OF_PLAYERS];

    for (uint16_t i = 0; i < FILTER_NUMBER_OF_PLAYERS; i++)
    {
        filter_power_values[i] = filter_getCurrentPowerValue(i);
        original_filter_power_values[i] = filter_power_values[i];
    }

    quicksort(filter_power_values, FILTER_NUMBER_OF_PLAYERS);

    for (int8_t i = 0; i < FILTER_NUMBER_OF_PLAYERS; i++)
    {
        if (original_filter_power_values[i] > filter_power_values[DETECTOR_HALF_MAX_NEW_INPUT_COUNT] * DETECTOR_FUDGE_FACTOR)
        {
            hit_detected = true;
            number_of_hits[i]++;
            lockoutTimer_start();
            hitLedTimer_start();

            break;
        }
    }
}

void detector_init()
{
    lockoutTimer_init();

    for (uint8_t i = 0; i < FILTER_NUMBER_OF_PLAYERS; i++)
    {
        number_of_hits[i] = DETECTOR_CLEAR_HIT_COUNT;
    }
}

void detector(bool interruptsEnabled, bool ignoreSelf)
{
    uint8_t add_new_input_count = DETECTOR_NEW_INPUT_COUNT_CLEAR;
    adc_queue_elements_count = isr_adcBufferElementCount();
    uint16_t raw_value;

    for (uint32_t i = 0; i < adc_queue_elements_count; i++)
    {
        if (interruptsEnabled)
        {
            interrupts_disableArmInts();
            raw_value = isr_removeDataFromAdcBuffer();
            interrupts_enableArmInts();
        }

        else
        {
            raw_value = isr_removeDataFromAdcBuffer();
        }

        filter_addNewInput(detector_scaled_adc_value(raw_value));
        add_new_input_count++;

        printf("#\n\r");

        if (add_new_input_count >= DETECTOR_MAX_NEW_INPUT_COUNT)
        {
            add_new_input_count = DETECTOR_NEW_INPUT_COUNT_CLEAR;
            filter_firFilter();

            for (uint16_t j = 0; j < FILTER_NUMBER_OF_PLAYERS; j++)
            {
                filter_iirFilter(j);
                filter_computePower(j, false, false);
            }

            if (!lockoutTimer_running())
            {
                detector_hit_detection_algorithm();
            }
        }
    }
}

bool detector_hitDetected()
{
    return hit_detected;
}

void detector_clearHit()
{
    hit_detected = false;
}

void detector_getHitCounts(detector_hitCount_t hitArray[])
{
    for (uint8_t i = 0; i < FILTER_NUMBER_OF_PLAYERS; i++)
    {
        hitArray[i] = number_of_hits[i];
    }
}

void detector_runTest()
{

}
