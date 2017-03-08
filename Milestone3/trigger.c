#include <stdio.h>
#include "trigger.h"
#include "../Lab2/buttons.h"
#include "supportFiles/mio.h"

#define FIRE_TIME_UP 10 //Half a second
#define PULL_TIME_UP 3 //Debounce timers
#define RELEASE_TIME_UP 3 //Debounce timers
#define INITIAL 0
#define TRIGGER_GUN_TRIGGER_MIO_PIN 13
#define GUN_TRIGGER_PRESSED 1
// The trigger state machine debounces both the press and release of gun trigger.
// Ultimately, it will activate the transmitter when a debounced press is detected.

volatile static bool ignoreGunInput;
volatile static bool TriggerEnable_g; //Create a global variable to enable the state

//Trigger can be activated by either btn0 or the external gun that is attached to the TRIGGER_GUN_TRIGGER_MIO_PIN
//Gun input is ignred if the gun-input is high when the init() function is invoked.
bool triggerPressed(){
    return((!ignoreGunInput & (mio_readPin(TRIGGER_GUN_TRIGGER_MIO_PIN) == GUN_TRIGGER_PRESSED)) || (buttons_read() & BUTTONS_BTN0_MASK));

}

// Init trigger data-structures.
// Determines whether the trigger switch of the gun is connected (see discussion in lab web pages).
// Initializes the mio subsystem.
void trigger_init(){
    mio_setPinAsInput(TRIGGER_GUN_TRIGGER_MIO_PIN);
    if(triggerPressed()){
        ignoreGunInput = true;
    }
    //not sure what else to include in this function
}

// Enable the trigger state machine. The trigger state-machine is inactive until this function is called.
// This allows you to ignore the trigger when helpful (mostly useful for testing).
// I don't have an associated trigger_disable() function because I don't need to disable the trigger.
void trigger_enable(){
    TriggerEnable_g = true;
}

bool touched_g; //Create a global variable for whether it has been touched
bool trigger_releaseDetected() {
    return touched_g; //Return whether the board has been touched
}

enum triggerControl_st { //Create a state machine
    triggerControl_idle_st, //We want a start state
    triggerControl_debouncePullTimer_st, //We want a state where we wait til the board has been touched
    triggerControl_fire_st, //We want a state for the adc Counter
    triggerControl_waitTilReleased_st, //We want a state where we wait til it's not touched
    triggerControl_debounceReleaseTimer_st //We want a final state
} triggerControl_currentState = triggerControl_idle_st; //Set current state to initial state

//Only print the message if:
// 1. This the first pass and the value for previousState is unknown.
// 2. previousState != currentState - this prevents reprinting the same state
// name over and over.
void triggerControl_stateDebugPrint() {
static triggerControl_st previousState; //Create a previous state
static bool firstPass = true; //Create a variable called first pass

    if (previousState != triggerControl_currentState || firstPass) {
        //If the previous state is not the current state and it's not the
        // first pass
        firstPass = false; //Set first pass to true
        previousState = triggerControl_currentState;
        //Set previous state to current state
        switch (triggerControl_currentState) { //Make a switch statement
        case triggerControl_idle_st: //Idle state case
            printf("triggerControl_idle_st\n\r"); //Print idle state
            break; //Break
        case triggerControl_debouncePullTimer_st:
            //Wait debounce pull timer state case
            printf("triggerControl_debouncePullTimer_st\n\r");
            //Print debounce timer state
            break; //Break
        case triggerControl_fire_st: //Fire state case
            printf("triggerControl_fire_st\n\r");
            printf("D\n\r");
            //Print the fire state
            break; //Break
        case triggerControl_waitTilReleased_st:
            //Wait til released state case
            printf("triggerControl_waitTilReleased_st\n\r");
            //Print wait til released state
            break; //Break
        case triggerControl_debounceReleaseTimer_st: //Debounce release timer state case
            printf("triggerControl_debounceReleaseTimer_st\n\r"); //Print final state
            printf("U\n\r");
            break; //Break
        default: //Default case
            break; //Break
        }
    }
}

uint16_t pullCount;
uint16_t fireCount;
uint16_t releaseCount;
// Standard tick function.
void trigger_tick(){
    triggerControl_stateDebugPrint(); //Call the state debug print
    //---------------------------------------->State actions
    switch (triggerControl_currentState) { //Create a switch statement
    case triggerControl_idle_st: //Idle state case
        pullCount = INITIAL; //Initialize this pull count
        fireCount = INITIAL; //Initialize this fire count
        releaseCount = INITIAL; //Initialize this release count
        break;
    case triggerControl_debouncePullTimer_st: //Wait debounce pull timer state case
        pullCount++; //Increment pull count by 1
        break;
    case triggerControl_fire_st: //Fire state case

    //Fire the gun!!!!

        fireCount++; //Increment fire count by 1
        break;
    case triggerControl_waitTilReleased_st: //Wait til released state case
        break;
    case triggerControl_debounceReleaseTimer_st: //Debounce release timer state case
        releaseCount++; //Increment release count by 1
        break;
    default: //Default case
        break; //Break
    }

    //---------------------------------------->Transitions
    switch (triggerControl_currentState) { //Create a switch statement
    case triggerControl_idle_st: //Idle state case
        if(triggerPressed()){
            triggerControl_currentState = triggerControl_debouncePullTimer_st;
            //Set current state to debounce pull timer state
        }
        else{
            triggerControl_currentState = triggerControl_idle_st;
            //Set current state to idle state
        }
        break;
    case triggerControl_debouncePullTimer_st: //Wait debounce pull timer state case
        if(pullCount >= PULL_TIME_UP){
            triggerControl_currentState = triggerControl_fire_st;
            //Set current state to fire state
        }
        else{
            triggerControl_currentState = triggerControl_debouncePullTimer_st;
            //Set current state to debounce pull timer state
        }
        break;
    case triggerControl_fire_st: //Fire state case
        if((fireCount >= FIRE_TIME_UP) && triggerPressed()){
            triggerControl_currentState = triggerControl_waitTilReleased_st;
            //Set current state to wait til released state
        }
        else if((fireCount == FIRE_TIME_UP) && !triggerPressed()){
            triggerControl_currentState = triggerControl_debounceReleaseTimer_st;
            //Set current state to debounce release timer state
        }
        else{
            triggerControl_currentState = triggerControl_fire_st;
            //Set current state to fire state
        }
        break;
    case triggerControl_waitTilReleased_st: //Wait til released state case
        if(!triggerPressed()){
            triggerControl_currentState = triggerControl_debounceReleaseTimer_st;
            //Set current state to debounce release timer state
        }
        else{
            triggerControl_currentState = triggerControl_waitTilReleased_st;
            //Set current state to wait til released state
        }
        break;
    case triggerControl_debounceReleaseTimer_st: //Debounce release timer state case
        if(releaseCount >= RELEASE_TIME_UP){
            triggerControl_currentState = triggerControl_idle_st;
            //Set current state to idle state
        }
        else{
            triggerControl_currentState = triggerControl_debounceReleaseTimer_st;
            //Set current state to debounce release state
        }
        break;
    default: //Default state
        break; //Break
    }
}

void trigger_runTest()
{
    printf("Started trigger run test...\n\r");

    trigger_init();

    while (!(buttons_read() & BUTTONS_BTN1_MASK))
    {
        trigger_tick();
    }

    printf("Ended trigger run test.\n\r");
}
