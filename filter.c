#include <stdint.h>
#include "filter.h"
 

 
 
//#define FILTER_SAMPLE_FREQUENCY_IN_KHZ 100
//#define FILTER_FREQUENCY_COUNT 10
//#define FILTER_FIR_DECIMATION_FACTOR 10  // FIR-filter needs this many new inputs to compute a new output.
//#define FILTER_INPUT_PULSE_WIDTH 2000    // This is the width of the pulse you are looking for, in terms of decimated sample count.
// These are the tick counts that are used to generate the user frequencies.
// Not used in filter.h but are used to TEST the filter code.
// Placed here for general access as they are essentially constant throughout
// the code. The transmitter will also use these.
const uint16_t filter_frequencyTickTable[FILTER_FREQUENCY_COUNT] = {68, 58, 50, 44, 38, 34, 30, 28, 26, 24};
 
// Filtering routines for the laser-tag project.
// Filtering is performed by a two-stage filter, as described below.
 
// 1. First filter is a decimating FIR filter with a configurable number of taps and decimation factor.
// 2. The output from the decimating FIR filter is passed through a bank of 10 IIR filters. The
// characteristics of the IIR filter are fixed.
 
/*********************************************************************************************************
****************************************** Main Filter Functions *****************************************
**********************************************************************************************************/
 
 

 #define X_QUEUE_SIZE 81
 #define Y_QUEUE_SIZE 11
 #define Z_QUEUE_SIZE 10
 #define OUTPUT_QUEUE_SIZE 2000
 #define IIR_A_COEFFICIENT_COUNT 11
 #define IIR_B_COEFFICIENT_COUNT 11
 #define FIR_A_COEFFICIENT_COUNT 81
 #define FIR_B_COEFFICIENT_COUNT 81
 #define FILTER_IIR_FILTER_COUNT
 #define FILTER_FIR_FILTER_COUNT
 #define NUMBER_OF_PLAYERS 10
 
 
 static queue_t zQueue[FILTER_IIR_FILTER_COUNT];
 static queue_t xQueue[];
 static queue_t yQueue[];
 static queue_t outputQueue[]; 
 
 void initXQueue(){
	 //We are filling the x queues with zeros
	 for(uint i = 0; i < ; i++){
		 queue_init(&(xQueue[i]), X_QUEUE_SIZE);
		 for(uint j = 0; j<X_QUEUE_SIZE; j++){
			 queue_overwritePush(&(xQueue[i]), 0.0);//not sure if this should be i or j
		 }
	 }
 }
 void initYQueue(){
 	 //We are filling the x queues with zeros
	 for(uint i = 0; i < ; i++){
		 queue_init(&(yQueue[i]), Y_QUEUE_SIZE);
		 for(uint j = 0; j<Y_QUEUE_SIZE; j++){
			 queue_overwritePush(&(yQueue[i]), 0.0);//not sure if this should be i or j
		 }
	 }
 }
 void initZQueue(){
	 //We are filling the x queues with zeros
	 for(uint i = 0; i < FILTER_IIR_FILTER_COUNT; i++){
		 queue_init(&(zQueue[i]), Z_QUEUE_SIZE);
		 for(uint j = 0; j<Z_QUEUE_SIZE; j++){
			 queue_overwritePush(&(zQueue[i]), 0.0);//not sure if this should be i or j
		 }
	 }	 
 }
 void initOutputQueue(){
 	 //We are filling the output queues with zeros
	 for(uint i = 0; i < NUMBER_OF_PLAYERS; i++){
		 queue_init(&(outputQueue[i]), OUTPUT_QUEUE_SIZE);
		 for(uint j = 0; j<OUTPUT_QUEUE_SIZE; j++){
			 queue_overwritePush(&(outputQueue[i]), 0.0);//not sure if this should be i or j
		 }
	 }	 
 }
 
 
 
 
// Must call this prior to using any filter functions.
void filter_init(){
	initXQueue();
	initYQueue();
	initZQueue();
	initOutputQueue();
}
 
// Use this to copy an input into the input queue of the FIR-filter (xQueue).
void filter_addNewInput(double x){
	
}
 
// Fills a queue with the given fillValue.
void filter_fillQueue(queue_t* q, double fillValue){
	
}
 
// Invokes the FIR-filter. Input is contents of xQueue.
// Output is returned and is also pushed on to yQueue.
double filter_firFilter(){
	
}
 
// Use this to invoke a single iir filter. Input comes from yQueue.
// Output is returned and is also pushed onto zQueue[filterNumber].
double filter_iirFilter(uint16_t filterNumber){
	
}
 
// Use this to compute the power for values contained in an outputQueue.
// If force == true, then recompute power by using all values in the outputQueue.
// This option is necessary so that you can correctly compute power values the first time.
// After that, you can incrementally compute power values by:
// 1. Keeping track of the power computed in a previous run, call this prev-power.
// 2. Keeping track of the oldest outputQueue value used in a previous run, call this oldest-value.
// 3. Get the newest value from the power queue, call this newest-value.
// 4. Compute new power as: prev-power - (oldest-value * oldest-value) + (newest-value * newest-value).
// Note that this function will probably need an array to keep track of these values for each
// of the 10 output queues.
double filter_computePower(uint16_t filterNumber, bool forceComputeFromScratch, bool debugPrint){
	
}
 
// Returns the last-computed output power value for the IIR filter [filterNumber].
double filter_getCurrentPowerValue(uint16_t filterNumber){
	
}
 
// Get a copy of the current power values.
// This function copies the already computed values into a previously-declared array
// so that they can be accessed from outside the filter software by the detector.
// Remember that when you pass an array into a C function, changes to the array within
// that function are reflected in the returned array.
void filter_getCurrentPowerValues(double powerValues[]){
	
}
 
// Using the previously-computed power values that are current stored in currentPowerValue[] array,
// Copy these values into the normalizedArray[] argument and then normalize them by dividing
// all of the values in normalizedArray by the maximum power value contained in currentPowerValue[].
void filter_getNormalizedPowerValues(double normalizedArray[], uint16_t* indexOfMaxValue){
	
}
 
/*********************************************************************************************************
********************************** Verification-assisting functions. *************************************
********* Test functions access the internal data structures of the filter.c via these functions. ********
*********************** These functions are not used by the main filter functions. ***********************
**********************************************************************************************************/
 
// Returns the array of FIR coefficients.
const double* filter_getFirCoefficientArray();{
	
} 
// Returns the number of FIR coefficients.
uint32_t filter_getFirCoefficientCount();{
	
} 
// Returns the array of coefficients for a particular filter number.
const double* filter_getIirACoefficientArray(uint16_t filterNumber){
	
}
 
// Returns the number of A coefficients.
uint32_t filter_getIirACoefficientCount(){
	
}
 
// Returns the array of coefficients for a particular filter number.
const double* filter_getIirBCoefficientArray(uint16_t filterNumber){
	
}
 
// Returns the number of B coefficients.
uint32_t filter_getIirBCoefficientCount(){
	
}
 
// Returns the size of the yQueue.
uint32_t filter_getYQueueSize(){
	
}
 
// Returns the decimation value.
uint16_t filter_getDecimationValue(){
	
}
 
// Returns the address of xQueue.
queue_t* filter_getXQueue(){
	
}
 
// Returns the address of yQueue.
queue_t* filter_getYQueue(){
	
}
 
// Returns the address of zQueue for a specific filter number.
queue_t* filter_getZQueue(uint16_t filterNumber){
	
}
 
// Returns the address of the IIR output-queue for a specific filter-number.
queue_t* filter_getIirOutputQueue(uint16_t filterNumber){
	
}
 
// Returns the address of the firOutputDebugQueue.
//queue_t* filter_getFirOutputDebugQueue();
 
//void filter_runTest();
 
