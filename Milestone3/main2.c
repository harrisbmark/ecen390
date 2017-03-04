#include "supportFiles/interrupts.h"

int main()
{
	trigger_init();
    interrupts_initAll(true);               // main interrupt init function.
    interrupts_enableTimerGlobalInts();     // enable global interrupts.
    interrupts_startArmPrivateTimer();      // start the main timer.
    interrupts_enableArmInts();             // now the ARM processor can see interrupts.
    while (1);                              // Just a forever-while loop for now. Modify as you see fit.
}