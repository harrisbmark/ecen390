#include "intervalTimer.h"
#include "xparameters.h"
#include "xil_io.h"
#include <stdio.h>

#define INTERVAL_TIMER_TEST_COUNT 10

#define INTERVAL_TIMER_TIMER_0_BASE_ADDR XPAR_AXI_TIMER_0_BASEADDR
#define INTERVAL_TIMER_TIMER_1_BASE_ADDR XPAR_AXI_TIMER_1_BASEADDR
#define INTERVAL_TIMER_TIMER_2_BASE_ADDR XPAR_AXI_TIMER_2_BASEADDR

#define INTERVAL_TIMER_0_CLOCK_FREQ XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ
#define INTERVAL_TIMER_1_CLOCK_FREQ XPAR_AXI_TIMER_1_CLOCK_FREQ_HZ
#define INTERVAL_TIMER_2_CLOCK_FREQ XPAR_AXI_TIMER_2_CLOCK_FREQ_HZ
#define INTERVAL_TIMER_0_CLOCK_PERIOD (double)(1 / INTERVAL_TIMER_0_CLOCK_FREQ)
#define INTERVAL_TIMER_1_CLOCK_PERIOD (double)(1 / INTERVAL_TIMER_1_CLOCK_FREQ)
#define INTERVAL_TIMER_2_CLOCK_PERIOD (double)(1 / INTERVAL_TIMER_2_CLOCK_FREQ)

#define INTERVAL_TIMER_TCSR0_OFFSET 0x00
#define INTERVAL_TIMER_TLR0_OFFSET 0x04
#define INTERVAL_TIMER_TCR0_OFFSET 0x08
#define INTERVAL_TIMER_TCSR1_OFFSET 0x10
#define INTERVAL_TIMER_TLR1_OFFSET 0x14
#define INTERVAL_TIMER_TCR1_OFFSET 0x18

#define INTERVAL_TIMER_TCSR_ZERO 0x00000000
#define INTERVAL_TIMER_TLR_ZERO 0x00000000

#define INTERVAL_TIMER_CASC_ON 0b00000000000000000000100000000000
#define INTERVAL_TIMER_UDT_OFF 0b11111111111111111111111111111101

#define INTERVAL_TIMER_LOAD_ON 0b00000000000000000000000000100000
#define INTERVAL_TIMER_LOAD_OFF 0b11111111111111111111111111011111

#define INTERVAL_TIMER_ENT_ON 0b00000000000000000000000010000000
#define INTERVAL_TIMER_ENT_OFF 0b11111111111111111111111101111111

/**********************************************************************************/
/* Function: intervalTimer_readTimerModule                                        */
/* Purpose: Allows the program to read from the timer module register.            */
/* Returns: The value of the time module register.                                */
/**********************************************************************************/
int32_t intervalTimer_readTimerModule(int32_t base_addr, int32_t offset)
{
    // Return the value of the timer module at the passed base address and passed offset.
    return Xil_In32(base_addr + offset);
}

/**********************************************************************************/
/* Function: intervalTimer_writeTimerModule                                       */
/* Purpose: Allows the program to write to the timer module register.             */
/* Returns: Void                                                                  */
/**********************************************************************************/
void intervalTimer_writeTimerModule(int32_t base_addr, int32_t offset, int32_t value)
{
    // Write to the timer module register.
    Xil_Out32(base_addr + offset, value);
}

/**********************************************************************************/
/* Function: intervalTimer_is_timerNumber                                         */
/* Purpose: Checks to see if the passed value is a timer number. If so, the       */
/*          function will return the base address of the timer number.            */
/* Returns: The base address of the timer number passed.                          */
/**********************************************************************************/
int32_t intervalTimer_is_timerNumber(uint32_t timerNumber)
{
    if (timerNumber < INTERVAL_TIMER_TIMER_0 || timerNumber > INTERVAL_TIMER_TIMER_2)
    {
        return false;
    }

    else
    {
        switch (timerNumber)
        {
            case INTERVAL_TIMER_TIMER_0:
                return INTERVAL_TIMER_TIMER_0_BASE_ADDR;

            case INTERVAL_TIMER_TIMER_1:
                return INTERVAL_TIMER_TIMER_1_BASE_ADDR;

            case INTERVAL_TIMER_TIMER_2:
                return INTERVAL_TIMER_TIMER_2_BASE_ADDR;

            default:
                return false;
        }
    }
}

intervalTimer_status_t intervalTimer_init(uint32_t timerNumber)
{
    int32_t timer_addr = intervalTimer_is_timerNumber(timerNumber);

    if (timer_addr)
    {
        intervalTimer_writeTimerModule(timer_addr, INTERVAL_TIMER_TCSR0_OFFSET, INTERVAL_TIMER_TCSR_ZERO);
        intervalTimer_writeTimerModule(timer_addr, INTERVAL_TIMER_TCSR1_OFFSET, INTERVAL_TIMER_TCSR_ZERO);
        intervalTimer_writeTimerModule(timer_addr, INTERVAL_TIMER_TCSR0_OFFSET, (INTERVAL_TIMER_CASC_ON | intervalTimer_readTimerModule(timer_addr, INTERVAL_TIMER_TCSR0_OFFSET)));
        intervalTimer_writeTimerModule(timer_addr, INTERVAL_TIMER_TCSR0_OFFSET, (INTERVAL_TIMER_UDT_OFF & intervalTimer_readTimerModule(timer_addr, INTERVAL_TIMER_TCSR0_OFFSET)));

        if (intervalTimer_readTimerModule(timer_addr, INTERVAL_TIMER_TCSR1_OFFSET) == INTERVAL_TIMER_TCSR_ZERO && (intervalTimer_readTimerModule(timer_addr, INTERVAL_TIMER_TCSR0_OFFSET) | INTERVAL_TIMER_CASC_ON) == INTERVAL_TIMER_CASC_ON && (intervalTimer_readTimerModule(timer_addr, INTERVAL_TIMER_TCSR0_OFFSET) & INTERVAL_TIMER_UDT_OFF) == INTERVAL_TIMER_UDT_OFF)
        {
            return INTERVAL_TIMER_STATUS_OK;
        }

        else
        {
            return INTERVAL_TIMER_STATUS_FAIL;
        }
    }

    else
    {
        return INTERVAL_TIMER_STATUS_FAIL;
    }
}

intervalTimer_status_t intervalTimer_initAll()
{
    for (int i = INTERVAL_TIMER_TIMER_0; i <= INTERVAL_TIMER_TIMER_2; i++)
    {
        if (intervalTimer_init(i) != INTERVAL_TIMER_STATUS_OK)
        {
            return INTERVAL_TIMER_STATUS_FAIL;
        }
    }

    return INTERVAL_TIMER_STATUS_OK;
}

void intervalTimer_start(uint32_t timerNumber)
{
    int32_t timer_addr = intervalTimer_is_timerNumber(timerNumber);

    if (timer_addr)
    {
        // Set the counter to start counting.
        intervalTimer_writeTimerModule(timer_addr, INTERVAL_TIMER_TCSR0_OFFSET, (INTERVAL_TIMER_ENT_ON | intervalTimer_readTimerModule(timer_addr, INTERVAL_TIMER_TCSR0_OFFSET)));
    }

}

void intervalTimer_stop(uint32_t timerNumber)
{
    int32_t timer_addr = intervalTimer_is_timerNumber(timerNumber);

    if (timer_addr)
    {
        // Set the counter to stop.
        intervalTimer_writeTimerModule(timer_addr, INTERVAL_TIMER_TCSR0_OFFSET, (INTERVAL_TIMER_ENT_OFF & intervalTimer_readTimerModule(timer_addr, INTERVAL_TIMER_TCSR0_OFFSET)));
    }

}

void intervalTimer_reset(uint32_t timerNumber)
{
    int32_t timer_addr = intervalTimer_is_timerNumber(timerNumber);

    if (timer_addr)
    {
        // Set both TLR0 and TLR1 to all zeroes. We are settings the counters to zero here.
        intervalTimer_writeTimerModule(timer_addr, INTERVAL_TIMER_TLR0_OFFSET, INTERVAL_TIMER_TLR_ZERO);
        intervalTimer_writeTimerModule(timer_addr, INTERVAL_TIMER_TLR1_OFFSET, INTERVAL_TIMER_TLR_ZERO);

        // Load the zeroes into the registers.
        intervalTimer_writeTimerModule(timer_addr, INTERVAL_TIMER_TCSR0_OFFSET, (INTERVAL_TIMER_LOAD_ON | intervalTimer_readTimerModule(timer_addr, INTERVAL_TIMER_TCSR0_OFFSET)));
        intervalTimer_writeTimerModule(timer_addr, INTERVAL_TIMER_TCSR1_OFFSET, (INTERVAL_TIMER_LOAD_ON | intervalTimer_readTimerModule(timer_addr, INTERVAL_TIMER_TCSR1_OFFSET)));

        // Disable the load bit so that we can use the counters.
        intervalTimer_writeTimerModule(timer_addr, INTERVAL_TIMER_TCSR0_OFFSET, (INTERVAL_TIMER_LOAD_OFF & intervalTimer_readTimerModule(timer_addr, INTERVAL_TIMER_TCSR0_OFFSET)));
        intervalTimer_writeTimerModule(timer_addr, INTERVAL_TIMER_TCSR1_OFFSET, (INTERVAL_TIMER_LOAD_OFF & intervalTimer_readTimerModule(timer_addr, INTERVAL_TIMER_TCSR1_OFFSET)));
    }

}

void intervalTimer_resetAll()
{
    for (int i = INTERVAL_TIMER_TIMER_0; i <= INTERVAL_TIMER_TIMER_2; i++)
    {
        intervalTimer_reset(i);
    }
}

intervalTimer_status_t intervalTimer_test(uint32_t timerNumber)
{
    int32_t timer_addr = intervalTimer_is_timerNumber(timerNumber);
    int32_t current_time = 0;
    int32_t last_time = 0;

    if (timer_addr)
    {
        intervalTimer_reset(timerNumber);

        if (intervalTimer_readTimerModule(timer_addr, INTERVAL_TIMER_TCR0_OFFSET) == INTERVAL_TIMER_TLR_ZERO && intervalTimer_readTimerModule(timer_addr, INTERVAL_TIMER_TCR1_OFFSET) == INTERVAL_TIMER_TLR_ZERO)
        {
            intervalTimer_start(timerNumber);

            for (int i =  0; i < INTERVAL_TIMER_TEST_COUNT; i++)
            {
                current_time = intervalTimer_readTimerModule(timer_addr, INTERVAL_TIMER_TCR0_OFFSET);

                if (current_time == last_time)
                {
                    return INTERVAL_TIMER_STATUS_FAIL;
                }

                last_time = current_time;
            }

            intervalTimer_stop(timerNumber);

            for (int i =  0; i < INTERVAL_TIMER_TEST_COUNT; i++)
            {
                current_time = intervalTimer_readTimerModule(timer_addr, INTERVAL_TIMER_TCR0_OFFSET);

                if (current_time != last_time)
                {
                    return INTERVAL_TIMER_STATUS_FAIL;
                }

                last_time = current_time;
            }

            return INTERVAL_TIMER_STATUS_OK;
        }

        else
        {
            return INTERVAL_TIMER_STATUS_FAIL;
        }
    }

    else
    {
        return INTERVAL_TIMER_STATUS_FAIL;
    }
}

intervalTimer_status_t intervalTimer_testAll()
{
    for (int i = INTERVAL_TIMER_TIMER_0; i <= INTERVAL_TIMER_TIMER_2; i++)
    {
        if (intervalTimer_test(i) == INTERVAL_TIMER_STATUS_FAIL)
        {
            return INTERVAL_TIMER_STATUS_FAIL;
        }
    }

    return INTERVAL_TIMER_STATUS_OK;
}

double intervalTimer_getTotalDurationInSeconds(uint32_t timerNumber)
{
    int32_t timer_addr = intervalTimer_is_timerNumber(timerNumber);
    int32_t upper_32 = 0;
    int32_t lower_32 = 0;

    if (timer_addr)
    {
        do
        {
            upper_32 = intervalTimer_readTimerModule(timer_addr, INTERVAL_TIMER_TCR1_OFFSET);
            lower_32 = intervalTimer_readTimerModule(timer_addr, INTERVAL_TIMER_TCR0_OFFSET);
        }
        while (upper_32 != intervalTimer_readTimerModule(timer_addr, INTERVAL_TIMER_TCR1_OFFSET));

        return (double)lower_32 * (double).00000001;
    }

    return false;
}
