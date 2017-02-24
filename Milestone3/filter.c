#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "filter.h"
 
//#define FILTER_SAMPLE_FREQUENCY_IN_KHZ 100
//#define FILTER_FREQUENCY_COUNT 10
//#define FILTER_FIR_DECIMATION_FACTOR 10  // FIR-filter needs this many new inputs to compute a new output.
//#define FILTER_INPUT_PULSE_WIDTH 2000    // This is the width of the pulse you are looking for, in terms of decimated sample count.
// These are the tick counts that are used to generate the user frequencies.
// Not used in filter.h but are used to TEST the filter code.
// Placed here for general access as they are essentially constant throughout
// the code. The transmitter will also use these.
// Filtering routines for the laser-tag project.
// Filtering is performed by a two-stage filter, as described below.
// 1. First filter is a decimating FIR filter with a configurable number of taps and decimation factor.
// 2. The output from the decimating FIR filter is passed through a bank of 10 IIR filters. The
// characteristics of the IIR filter are fixed.
 
/*********************************************************************************************************
****************************************** Main Filter Functions *****************************************
**********************************************************************************************************/
#define DECIMATION_VALUE 10
#define X_QUEUE_SIZE 81
#define X_QUEUE_NAME "xQueue"
#define Y_QUEUE_SIZE 11
#define Y_QUEUE_NAME "yQueue"
#define Z_QUEUE_SIZE 10
#define Z_QUEUE_NAME "zQueue"
#define OUTPUT_QUEUE_SIZE 2000
#define OUTPUT_QUEUE_NAME "Output Queue"
#define IIR_A_COEFF_COUNT 11
#define IIR_B_COEFF_COUNT 11
#define FIR_A_COEFF_COUNT 81
#define FIR_B_COEFF_COUNT 81
#define NUMBER_OF_PLAYERS 10
#define FILTER_QUEUE_INIT_VALUE 0.0
#define QUEUE_STRING_SIZE 15

const double FILTER_FIR_A_COEFF[FIR_A_COEFF_COUNT] = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
const double FILTER_FIR_B_COEFF[FIR_B_COEFF_COUNT] = {0.000605461, 0.000525071, 0.000384491, 0.000173987, -0.000113605, -0.000474881, -0.000888139, -0.00130826, -0.00166636, -0.00187557, -0.00184324, -0.00148843, -0.000762255, 0.000332452, 0.00172625, 0.00327684, 0.00477448, 0.00596063, 0.00655915, 0.00631729, 0.00505164, 0.00269264, -0.000679508, -0.00481411, -0.00928992, -0.0135386, -0.0168916, -0.018647, -0.0181497, -0.0148759, -0.00851106, 0.000988489, 0.0133604, 0.0280333, 0.0441587, 0.0606765, 0.0764081, 0.0901668, 0.100875, 0.107671, 0.11, 0.107671, 0.100875, 0.0901668, 0.0764081, 0.0606765, 0.0441587, 0.0280333, 0.0133604, 0.000988489, -0.00851106, -0.0148759, -0.0181497, -0.018647, -0.0168916, -0.0135386, -0.00928992, -0.00481411, -0.000679508, 0.00269264, 0.00505164, 0.00631729, 0.00655915, 0.00596063, 0.00477448, 0.00327684, 0.00172625, 0.000332452, -0.000762255, -0.00148843, -0.00184324, -0.00187557, -0.00166636, -0.00130826, -0.000888139, -0.000474881, -0.000113605, 0.000173987, 0.000384491, 0.000525071, 0.000605461};
const double FILTER_IIR_A_COEFF[NUMBER_OF_PLAYERS][IIR_A_COEFF_COUNT] = {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, -5.96377}, {-4.63779, -3.05913, -1.40717, 0.820109, 2.70809, 4.94798, 6.17019, 7.40929, 8.57431, 19.1253, 13.5022}, {8.64175, 5.69041, 5.16738, 7.83191, 14.6916, 20.1272, 26.8579, 34.3066, -40.3415, -26.156, -14.2788}, {-5.73747, 3.25804, 12.2016, 29.0824, 42.9742, 61.5788, 84.0353, 61.5375, 38.5897, 21.3023, 11.958}, {10.3929, 18.6515, 43.1798, 65.958, 98.2583, 139.285, -70.0197, -43.039,-22.1939, -8.54353, 4.81018}, {18.7582, 48.4408, 75.2304, 113.595, 163.051, 60.2988, 37.8129, 20.8735, 11.7173, 10.1837, 18.2761}, {42.3107, 64.6304, 96.2805, 136.481, -38.7338, -25.1136, -13.7098, -5.50883, 3.1282, 11.7154, 27.9234}, {41.2616, 59.1247, 80.6863, 17.9935, 12.7032, 8.13036, 5.35368, 4.86159, 7.36844, 13.8222, 18.9361}, {25.2685, 32.2764, -5.49791, -4.27551, -2.82016, -1.29725, 0.756045, 2.49654, 4.56147, 5.6882, 6.83051}, {7.90451, 0.903328, 0.903328, 0.903328, 0.903328, 0.903328, 0.903328, 0.903328, 0.903328, 0.903328, 0.903328}};
const double FILTER_IIR_B_COEFF[NUMBER_OF_PLAYERS][IIR_B_COEFF_COUNT] = {{0.000000000909285, 0.000000000909286, 0.000000000909286, 0.000000000909287, 0.000000000909287, 0.000000000909287, 0.000000000909283, 0.000000000909296, 0.000000000909264, 0.000000000909062, -0.0}, {0.0, 0.0, 0.0, 0.0, -0.0, -0.0, 0.0, 0.0, 0.0, -0.00000000454642, -0.00000000454643}, {-0.00000000454643, -0.00000000454644, -0.00000000454643, -0.00000000454643, -0.00000000454642, -0.00000000454648, -0.00000000454632, -0.00000000454531, -0.0, 0.0, 0.0}, {0.0, 0.0, -0.0, -0.0, 0.0, 0.0, 0.0, 0.00000000909285, 0.00000000909286, 0.00000000909286, 0.00000000909287}, {0.00000000909287, 0.00000000909287, 0.00000000909283, 0.00000000909296, 0.00000000909264, 0.00000000909062, -0.0, 0.0, 0.0, 0.0, 0.0}, {-0.0, -0.0, 0.0, 0.0, 0.0, -0.00000000909285, -0.00000000909286, -0.00000000909286, -0.00000000909287, -0.00000000909287, -0.00000000909287}, {-0.00000000909283, -0.00000000909296, -0.00000000909264, -0.00000000909062, -0.0, 0.0, 0.0, 0.0, 0.0, -0.0, -0.0}, {0.0, 0.0, 0.0, 0.00000000454642, 0.00000000454643, 0.00000000454643, 0.00000000454644, 0.00000000454643, 0.00000000454643, 0.00000000454642, 0.00000000454648}, {0.00000000454632, 0.00000000454531, -0.0, 0.0, 0.0, 0.0, 0.0, -0.0, -0.0, 0.0, 0.0}, {0.0, -0.000000000909285, -0.000000000909286, -0.000000000909286, -0.000000000909287, -0.000000000909287, -0.000000000909287, -0.000000000909283, -0.000000000909296, -0.000000000909264, -0.000000000909062}};

static queue_t xQueue;
static queue_t yQueue;
static queue_t zQueue[NUMBER_OF_PLAYERS];
static queue_t outputQueue[NUMBER_OF_PLAYERS];

static double last_power_computed = 0.0;

double square(double value)
{
    return value * value;
}
 
void initXQueue()
{
	queue_init(&xQueue, X_QUEUE_SIZE, X_QUEUE_NAME);
	filter_fillQueue(&xQueue, FILTER_QUEUE_INIT_VALUE);
}

void initYQueue()
{
	queue_init(&yQueue, Y_QUEUE_SIZE, Y_QUEUE_NAME);
	filter_fillQueue(&yQueue, FILTER_QUEUE_INIT_VALUE);
}
 
 void initZQueue()
{
    char temp_string[QUEUE_STRING_SIZE];

    for(uint8_t i = 0; i < NUMBER_OF_PLAYERS; i++)
    {
        sprintf(temp_string, "%s #%d", Z_QUEUE_NAME, i);
        queue_init(&zQueue[i], Z_QUEUE_SIZE, temp_string);
        filter_fillQueue(&zQueue[i], FILTER_QUEUE_INIT_VALUE);
    }
}
 
void initOutputQueue()
{
    char temp_string[QUEUE_STRING_SIZE];

	for(uint8_t i = 0; i < NUMBER_OF_PLAYERS; i++)
	{
	    sprintf(temp_string, "%s #%d", OUTPUT_QUEUE_NAME, i);
		queue_init(&outputQueue[i], OUTPUT_QUEUE_SIZE, temp_string);
		filter_fillQueue(&outputQueue[i], FILTER_QUEUE_INIT_VALUE);
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
	queue_overwritePush(&xQueue, x);
}
 
// Fills a queue with the given fillValue.
void filter_fillQueue(queue_t* q, double fillValue)
{
	for(uint i = 0; i < q->size; i++)
	{
		queue_push(q, fillValue);
	}
}
 
// Invokes the FIR-filter. Input is contents of xQueue.
// Output is returned and is also pushed on to yQueue.
double filter_firFilter(){
    queue_data_t temp_y = FILTER_QUEUE_INIT_VALUE;

    for (uint8_t i = 0; i < FIR_B_COEFF_COUNT; i++)
    {
        temp_y += queue_readElementAt(&xQueue, FIR_B_COEFF_COUNT - 1 - i) * FILTER_FIR_B_COEFF[i];
    }

    queue_overwritePush(&yQueue, temp_y);

    return temp_y;
}
 
// Use this to invoke a single iir filter. Input comes from yQueue.
// Output is returned and is also pushed onto zQueue[filterNumber].
double filter_iirFilter(uint16_t filterNumber){
    queue_data_t temp_z1 = FILTER_QUEUE_INIT_VALUE;
    queue_data_t temp_z2 = FILTER_QUEUE_INIT_VALUE;

    for (uint8_t i = 0; i < IIR_B_COEFF_COUNT; i++)
    {
        temp_z1 += queue_readElementAt(&yQueue, IIR_B_COEFF_COUNT - 1 - i) * FILTER_IIR_B_COEFF[filterNumber][i];
    }

    for (uint8_t i = 0; i < IIR_A_COEFF_COUNT; i++)
    {
        temp_z2 += queue_readElementAt(&zQueue[filterNumber], IIR_A_COEFF_COUNT - 1 - i) * FILTER_IIR_A_COEFF[filterNumber][i];
    }

    queue_overwritePush(&zQueue[filterNumber], (temp_z1 - temp_z2));
    queue_overwritePush(&outputQueue[filterNumber], (temp_z1 - temp_z2));

    return (temp_z1 - temp_z2);
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
	double temp_power = 0.0;

	if (forceComputeFromScratch)
	{
        for (uint8_t i = 0; i < outputQueue->size; i++)
        {
            temp_power += (square(outputQueue->data[i]));
        }
	}
	
	else
	{
	    temp_power -= last_power_computed;
	    temp_power += square(outputQueue->data[outputQueue->indexIn - 1]);
	}

	last_power_computed = square(outputQueue->data[outputQueue->indexOut]);

	return temp_power;
}
 
// Returns the last-computed output power value for the IIR filter [filterNumber].
double filter_getCurrentPowerValue(uint16_t filterNumber){
	return filter_computePower(filterNumber, false, false);
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
const double* filter_getFirCoefficientArray(){
	return FILTER_FIR_B_COEFF;
} 

// Returns the number of FIR coefficients.
uint32_t filter_getFirCoefficientCount(){
    return FIR_B_COEFF_COUNT;
}

// Returns the array of coefficients for a particular filter number.
const double* filter_getIirACoefficientArray(uint16_t filterNumber){
	return FILTER_IIR_A_COEFF[filterNumber];
}
 
// Returns the number of A coefficients.
uint32_t filter_getIirACoefficientCount(){
	return IIR_A_COEFF_COUNT;
}
 
// Returns the array of coefficients for a particular filter number.
const double* filter_getIirBCoefficientArray(uint16_t filterNumber){
	return FILTER_IIR_B_COEFF[filterNumber];
}
 
// Returns the number of B coefficients.
uint32_t filter_getIirBCoefficientCount(){
	return IIR_B_COEFF_COUNT;
}
 
// Returns the size of the yQueue.
uint32_t filter_getYQueueSize(){
	return yQueue.size;
}
 
// Returns the decimation value.
uint16_t filter_getDecimationValue(){
	return DECIMATION_VALUE;
}
 
// Returns the address of xQueue.
queue_t* filter_getXQueue(){
	return &xQueue;
}
 
// Returns the address of yQueue.
queue_t* filter_getYQueue(){
	return &yQueue;
}
 
// Returns the address of zQueue for a specific filter number.
queue_t* filter_getZQueue(uint16_t filterNumber){
	return &zQueue[filterNumber];
}
 
// Returns the address of the IIR output-queue for a specific filter-number.
queue_t* filter_getIirOutputQueue(uint16_t filterNumber){
	return &outputQueue[filterNumber];
}
 
// Returns the address of the firOutputDebugQueue.
//queue_t* filter_getFirOutputDebugQueue();
 
//void filter_runTest();

