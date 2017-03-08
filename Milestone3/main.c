#include <stdio.h>
#include "transmitter.h"
#include "hitLedTimer.h"
#include "lockoutTimer.h"
#include "trigger.h"

int main()
{
    //transmitter_runTest();
    //hitLedTimer_runTest();
    //lockoutTimer_runTest();
    trigger_runTest();

    return 0;
}

void isr_function() {}
