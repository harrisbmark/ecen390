#include <stdio.h>
#include "transmitter.h"
#include "hitLedTimer.h"
#include "lockoutTimer.h"
#include "trigger.h"
#include "isr.h"

int main()
{
    isr_init();

    transmitter_runTest();
    hitLedTimer_runTest();
    lockoutTimer_runTest();
    trigger_runTest();

    return 0;
}
