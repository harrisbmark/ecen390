/*********************************************************************************************************/
/* File: trigger.c                                                                                       */
/* Date: 3-8-17                                                                                          */
/* Authors: Kelly Martin, Mark Harris                                                                    */
/*********************************************************************************************************/
#include <stdio.h>
#include "trigger.h"
#include "../Lab2/buttons.h"
#include "supportFiles/utils.h"
#include "supportFiles/mio.h"

// Required defines for defining the fire time and debouncing the timers.
#define FIRE_TIME_UP 50000
#define PULL_TIME_UP 5000
#define RELEASE_TIME_UP 5000

// Required defines for initial value, MIO pin, and MIO pin output when the trigger is pressed.
#define INITIAL 0
#define TRIGGER_GUN_TRIGGER_MIO_PIN 13
#define GUN_TRIGGER_PRESSED 1

// Required define used to debounce the buttons in testing.
#define TRIGGER_TEST_MS_DEBOUNCE_DELAY 300

// Variables used in the program to know when to ignore the gun input, when to enable the trigger, and when the trigger is touched.
volatile static bool ignoreGunInput;
volatile static bool triggerEnable_g;
volatile static bool touched_g;

// Variables used for the fire and release debounce timers and a count for releases.
volatile static uint16_t pullCount;
volatile static uint16_t fireCount;
volatile uint16_t releaseCount;

// Enum used to control the states of the trigger state machine.
enum triggerControl_st {
    triggerControl_idle_st, //We want an idle state
    triggerControl_debouncePullTimer_st, //We want a state where we wait until the board has been touched
    triggerControl_fire_st, //We want a state for the ADC Counter
    triggerControl_waitTilReleased_st, //We want a state where we wait until it's not touched
    triggerControl_debounceReleaseTimer_st //We want a final state
} triggerControl_currentState = triggerControl_idle_st; //Set current state to initial state

/*********************************************************************************************************/
/* Function: triggerPressed                                                                              */
/* Purpose: To provide the state of the trigger (either btn0 or the trigger MIO pin).                    */
/* Returns: A boolean value specifying if the trigger is pulled.                                         */
/*********************************************************************************************************/
bool triggerPressed() {
    // Return if the trigger is pulled or not.
    return((!ignoreGunInput & (mio_readPin(TRIGGER_GUN_TRIGGER_MIO_PIN) == GUN_TRIGGER_PRESSED)) || (buttons_read() & BUTTONS_BTN0_MASK));
}

/*********************************************************************************************************/
/* Function: trigger_init                                                                                */
/* Purpose: To initialize the trigger data used in the state machine. Also checks if the gun is          */
/*          connected.                                                                                   */
/* Returns: VOID                                                                                         */
/*********************************************************************************************************/
void trigger_init() {
    // Set the MIO input pin for the trigger.
    mio_setPinAsInput(TRIGGER_GUN_TRIGGER_MIO_PIN);

    // If the trigger is pressed, ignore other gun input.
    if(triggerPressed()) {
        ignoreGunInput = true;
    }
}

/*********************************************************************************************************/
/* Function: trigger_enable                                                                              */
/* Purpose: To enable the trigger state machine.                                                         */
/* Returns: VOID                                                                                         */
/*********************************************************************************************************/
void trigger_enable() {
    // Set the trigger enable variable to true.
    triggerEnable_g = true;
}

/*********************************************************************************************************/
/* Function: trigger_releaseDetected                                                                     */
/* Purpose: To detect if the trigger has been release.                                                   */
/* Returns: A boolean value specifying if the trigger has been release.                                  */
/*********************************************************************************************************/
bool trigger_releaseDetected() {
    // Return the value of the trigger being touched.
    return touched_g;
}

/*********************************************************************************************************/
/* Function: triggerControl_stateDebugPrint                                                              */
/* Purpose: To print the current state of the state machine.                                             */
/* Returns: VOID                                                                                         */
/*********************************************************************************************************/
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

/*********************************************************************************************************/
/* Function: trigger_tick                                                                                */
/* Purpose: To "tick" the state machine. This is the heart of the state machine, controlling actions.    */
/* Returns: VOID                                                                                         */
/*********************************************************************************************************/
void trigger_tick() {
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
            if(triggerPressed()) {
                triggerControl_currentState = triggerControl_debouncePullTimer_st;
                //Set current state to debounce pull timer state
            }
            else {
                triggerControl_currentState = triggerControl_idle_st;
                //Set current state to idle state
            }

            break;

        case triggerControl_debouncePullTimer_st: //Wait debounce pull timer state case
            if(pullCount >= PULL_TIME_UP) {
                triggerControl_currentState = triggerControl_fire_st;
                //Set current state to fire state
            }
            else {
                triggerControl_currentState = triggerControl_debouncePullTimer_st;
                //Set current state to debounce pull timer state
            }

            break;

        case triggerControl_fire_st: //Fire state case
            if((fireCount >= FIRE_TIME_UP) && triggerPressed()) {
                triggerControl_currentState = triggerControl_waitTilReleased_st;
                //Set current state to wait til released state
            }
            else if((fireCount == FIRE_TIME_UP) && !triggerPressed()) {
                triggerControl_currentState = triggerControl_debounceReleaseTimer_st;
                //Set current state to debounce release timer state
            }
            else {
                triggerControl_currentState = triggerControl_fire_st;
                //Set current state to fire state
            }

            break;

        case triggerControl_waitTilReleased_st: //Wait til released state case
            if(!triggerPressed()) {
                triggerControl_currentState = triggerControl_debounceReleaseTimer_st;
                //Set current state to debounce release timer state
            }
            else {
                triggerControl_currentState = triggerControl_waitTilReleased_st;
                //Set current state to wait til released state
            }

            break;

        case triggerControl_debounceReleaseTimer_st: //Debounce release timer state case
            if(releaseCount >= RELEASE_TIME_UP) {
                triggerControl_currentState = triggerControl_idle_st;
                //Set current state to idle state
            }
            else {
                triggerControl_currentState = triggerControl_debounceReleaseTimer_st;
                //Set current state to debounce release state
            }

            break;

        default: //Default state
            triggerControl_currentState = triggerControl_idle_st; // Go to the idle state.
            break; //Break
    }
}

/*********************************************************************************************************/
/* Function: trigger_runTest                                                                             */
/* Purpose: To test the trigger state machine.                                                           */
/* Returns: VOID
/*********************************************************************************************************/
void trigger_runTest()
{
    // Inform the user that the trigger test is running.
    printf("Started trigger run test...\n\r");

    // Initialize the trigger.
    trigger_init();

    // Let the ISR function run the ticks (and keep going until button 1 is pushed).
    while (!(buttons_read() & BUTTONS_BTN1_MASK)) {}

    // If button 1 is pushed (the kill button).
    if ((buttons_read() & BUTTONS_BTN1_MASK))
    {
        // While the button is held down.
        while ((buttons_read() & BUTTONS_BTN1_MASK))
        {
            // Keep delaying reading the button to make sure it is debounced.
            utils_msDelay(TRIGGER_TEST_MS_DEBOUNCE_DELAY);
        };
    }

    // Inform the user that the trigger test is ending.
    printf("Ended trigger run test.\n\r");
}
