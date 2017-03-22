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
#define DETECTOR_MAX_ADC_VALUE 4095
#define DETECTOR_HALF_MAX_ADC_VALUE (DETECTOR_MAX_ADC_VALUE/2)
#define DETECTOR_NEW_INPUT_COUNT_CLEAR 0
#define DETECTOR_MAX_NEW_INPUT_COUNT 10
#define DETECTOR_HALF_MAX_NEW_INPUT_COUNT DETECTOR_MAX_NEW_INPUT_COUNT/2
#define DETECTOR_FUDGE_FACTOR 2000

static volatile bool hit_detected;
static volatile uint32_t adc_queue_elements_count;
static volatile uint32_t original_filter_power_value[FILTER_NUMBER_OF_PLAYERS];
static volatile detector_hitCount_t number_of_hits[FILTER_NUMBER_OF_PLAYERS];

double detector_scaled_adc_value(uint16_t value)
{
    return (value - DETECTOR_HALF_MAX_ADC_VALUE)/DETECTOR_HALF_MAX_ADC_VALUE;
}

void detector_hit_detection_algorithm()
{
    uint32_t filter_power_value[FILTER_NUMBER_OF_PLAYERS];

    for (uint16_t i = 0; i < FILTER_NUMBER_OF_PLAYERS; i++)
    {
        original_filter_power_value[i] = filter_getCurrentPowerValue(i);
        filter_power_value[i] = original_filter_power_value[i];
    }

    quicksort(filter_power_value, FILTER_NUMBER_OF_PLAYERS);

    for (uint16_t i = FILTER_NUMBER_OF_PLAYERS - 1; i >= 0; i--)
    {
        if (filter_power_value[i] >= filter_power_value[DETECTOR_HALF_MAX_NEW_INPUT_COUNT] * DETECTOR_FUDGE_FACTOR)
        {
            hit_detected = true;
            number_of_hits[i]++;

            break;
        }
    }
}

void detector_init()
{
    //lockoutTimer_init();

    for (uint8_t i = 0; i < FILTER_NUMBER_OF_PLAYERS; i++)
    {
        number_of_hits[i] = DETECTOR_CLEAR_HIT_COUNT;
    }
}

void detector(bool interruptsEnabled, bool ignoreSelf)
{
    uint8_t add_new_input_count = DETECTOR_NEW_INPUT_COUNT_CLEAR;
    adc_queue_elements_count = isr_adcBufferElementCount();

    for (uint32_t i = 0; i < adc_queue_elements_count; i++)
    {
        if (interruptsEnabled)
        {
            interrupts_disableArmInts();
            isr_removeDataFromAdcBuffer();
            interrupts_enableArmInts();
        }

        else
        {
            isr_removeDataFromAdcBuffer();
        }

        filter_addNewInput(detector_scaled_adc_value(isr_removeDataFromAdcBuffer()));
        add_new_input_count++;

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

                if (hit_detected)
                {
                    lockoutTimer_start();
                    hitLedTimer_start();
                }
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
