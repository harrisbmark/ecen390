#include "trigger.h"


#define BUTTONS_BTN0_MASK
// The trigger state machine debounces both the press and release of gun trigger.
// Ultimately, it will activate the transmitter when a debounced press is detected.

bool ignoreGunInput;
bool TriggerEnable_g; //Create a global variable to enable the state

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

//Trigger can be activated by either btn0 or the external gun that is attached to the TRIGGER_GUN_TRIGGER_MIO_PIN
//Gun input is ignred if the gun-input is high when the init() function is invoked.
bool triggerPressed(){
	return((!ignoreGunInput & (mio_readPin(TRIGGER_GUN_TRIGGER_MIO_PIN) == GUN_TRIGGER_PRESSED)) || (buttonsread() & BUTTONS_BTN0_MASK));
	
}

enum triggerControl_st { //Create a state machine
    triggerControl_init_st, //We want an initial state
    triggerControl_start_st, //We want a start state
    triggerControl_waitTilTouched_st, //We want a state where we wait til the board has been touched
    triggerControl_adcCounter_st, //We want a state for the adc Counter
    triggerControl_waitTilReleased_st, //We want a state where we wait til it's not touched
    triggerControl_final_st //We want a final state   
} triggerControl_currentState = triggerControl_init_st; //Set current state to initial state

//Only print the message if:
// 1. This the first pass and the value for previousState is unknown.
// 2. previousState != currentState - this prevents reprinting the same state
// name over and over.
void triggerControl_stateDebugPrint() {
static buttonHandlerControl_st previousState; //Create a previous state
static bool firstPass = true; //Create a variable called first pass

    if (previousState != triggerControl_currentState || firstPass) {
        //If the previous state is not the current state and it's not the
        // first pass
        firstPass = false; //Set first pass to true
        previousState = triggerControl_currentState;
        //Set previous state to current state
		switch (triggerControl_currentState) { //Make a switch statement
        case triggerControl_init_st: //Init state case
            printf("triggerControl_init_st\n\r"); //Print init state
            break; //Break
		case triggerControl_start_st: //Start state case
            printf("triggerControl_start_st\n\r"); //Print start state
            break; //Break
        case triggerControl_waitTilTouched_st:
            //Wait til touched state case
            printf("triggerControl_waitTilTouched_st\n\r");
            //Print wit til touched state
            break; //Break
        case triggerControl_adcCounter_st: //adc counter state case
            printf("triggerControl_adcCounter_st\n\r");
            //Print the adc count state
            break; //Break
        case triggerControl_waitTilReleased_st:
            //Wait til released state case
            printf("triggerControl_waitTilReleased_st\n\r");
            //Print wait til released state
            break; //Break
        case triggerControl_final_st: //Final state case
            printf("triggerControl_final_st\n\r"); //Print final state
            break; //Break        	
		default: //Default case
            break; //Break
        }
    }
}

// Standard tick function.
void trigger_tick(){
    triggerControl_stateDebugPrint(); //Call the state debug print
    //---------------------------------------->State actions
    switch (triggerControl_currentState) { //Create a switch statement
    case triggerControl_init_st: //Init state case
        break; //Break
    
    default: //Default case
        break; //Break
    }

    //---------------------------------------->Transitions
    switch (buttonHandlerControl_currentState) { //Create a switch statement
    case triggerControl_init_st: //Init state case
        //adc_count_g = INITIAL; //Initialize adc count
        triggerControl_currentState = triggerControl_start_st;
        //Set current state to start state
        break; //Break
    
    default: //Default state
        break; //Break
    }	
	
}