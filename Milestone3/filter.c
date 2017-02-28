/*********************************************************************************************************/
/* File: filter.c                                                                                        */
/* Date: Feb 24, 2017                                                                                    */
/* Authors: Kelly Martin, Mark Harris                                                                     */
/*********************************************************************************************************/
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
// Define used to control the number of players for the game.
#define FILTER_NUMBER_OF_PLAYERS 10

// Define used to control the decimation value.
#define FILTER_DECIMATION_VALUE 10

// Defines used to control the xQueue queue size and name.
#define FILTER_X_QUEUE_SIZE 81
#define FILTER_X_QUEUE_NAME "xQueue"

// Defines used to control the yQueue queue size and name.
#define FILTER_Y_QUEUE_SIZE 11
#define FILTER_Y_QUEUE_NAME "yQueue"

// Defines used to control the zQueue queue size and name.
#define FILTER_Z_QUEUE_SIZE 10
#define FILTER_FILTER_Z_QUEUE_NAME "zQueue"

// Defines used to control the output queue size and name.
#define FILTER_OUTPUT_QUEUE_SIZE 2000
#define FILTER_OUTPUT_QUEUE_NAME "Output Queue"

// Defines used for the size of the IIR A and B coefficient arrays.
#define FILTER_IIR_A_COEFF_COUNT 10
#define FILTER_IIR_B_COEFF_COUNT 11

// Defines used for the size of the FIR A and B coefficient arrays.
#define FILTER_FIR_A_COEFF_COUNT 81
#define FILTER_FIR_B_COEFF_COUNT 81

// Defines used to give a queue init value and default string size.
#define FILTER_QUEUE_INIT_VALUE 0.0
#define QUEUE_STRING_SIZE 20

// Constant defined to hold all of the FIR A coefficient values (1x81).
const double FILTER_FIR_A_COEFF[FILTER_FIR_A_COEFF_COUNT] = {
	1.0000000000000000e+00, 0.0000000000000000e+00, 0.0000000000000000e+00, 0.0000000000000000e+00, 0.0000000000000000e+00,
	0.0000000000000000e+00, 0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,
	0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,
	0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,
	0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,
	0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,
	0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,
	0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,
	0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,
	0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,
	0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,
	0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,
	0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,
	0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,
	0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,
	0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,   0.0000000000000000e+00,
	0.0000000000000000e+00
};

// Constant used to hold all of the FIR B coefficient values (1x81).
const double FILTER_FIR_B_COEFF[FILTER_FIR_B_COEFF_COUNT] = {
	6.0546138291252597e-04,   5.2507143315267811e-04,   3.8449091272701525e-04,   1.7398667197948182e-04,  -1.1360489934931548e-04,
	-4.7488111478632532e-04,  -8.8813878356223768e-04,  -1.3082618178394971e-03,  -1.6663618496969908e-03,  -1.8755700366336781e-03,
	-1.8432363328817916e-03,  -1.4884258721727399e-03,  -7.6225514924622853e-04,   3.3245249132384837e-04,   1.7262548802593762e-03,
	3.2768418720744217e-03,   4.7744814146589041e-03,   5.9606317814670249e-03,   6.5591485566565593e-03,   6.3172870282586493e-03,
	5.0516421324586546e-03,   2.6926388909554420e-03,  -6.7950808883015244e-04,  -4.8141100026888716e-03,  -9.2899200683230643e-03,
	-1.3538595939086505e-02,  -1.6891587875325020e-02,  -1.8646984919441702e-02,  -1.8149697899123560e-02,  -1.4875876924586697e-02,
	-8.5110608557150517e-03,   9.8848931927316319e-04,   1.3360421141947857e-02,   2.8033301291042201e-02,   4.4158668590312596e-02,
	6.0676486642862550e-02,   7.6408062643700314e-02,   9.0166807112971648e-02,   1.0087463525509034e-01,   1.0767073207825099e-01,
	1.1000000000000000e-01,   1.0767073207825099e-01,   1.0087463525509034e-01,   9.0166807112971648e-02,   7.6408062643700314e-02,
	6.0676486642862550e-02,   4.4158668590312596e-02,   2.8033301291042201e-02,   1.3360421141947857e-02,   9.8848931927316319e-04,
	-8.5110608557150517e-03,  -1.4875876924586697e-02,  -1.8149697899123560e-02,  -1.8646984919441702e-02,  -1.6891587875325020e-02,
	-1.3538595939086505e-02,  -9.2899200683230643e-03,  -4.8141100026888716e-03,  -6.7950808883015244e-04,   2.6926388909554420e-03,
	5.0516421324586546e-03,   6.3172870282586493e-03,   6.5591485566565593e-03,   5.9606317814670249e-03,   4.7744814146589041e-03,
	3.2768418720744217e-03,   1.7262548802593762e-03,   3.3245249132384837e-04,  -7.6225514924622853e-04,  -1.4884258721727399e-03,
	-1.8432363328817916e-03,  -1.8755700366336781e-03,  -1.6663618496969908e-03,  -1.3082618178394971e-03,  -8.8813878356223768e-04,
	-4.7488111478632532e-04,  -1.1360489934931548e-04,   1.7398667197948182e-04,   3.8449091272701525e-04,   5.2507143315267811e-04,   6.0546138291252597e-04
};

// Constant used to hold all of the IIR A coefficient values (10x10).
const double FILTER_IIR_A_COEFF[FILTER_NUMBER_OF_PLAYERS][FILTER_IIR_A_COEFF_COUNT] = {
	{-5.9637727070163997e+00, 1.9125339333078244e+01, -4.0341474540744173e+01, 6.1537466875368821e+01, -7.0019717951472202e+01, 6.0298814235238908e+01, -3.8733792862566332e+01, 1.7993533279581083e+01, -5.4979061224867767e+00, 9.0332828533799836e-01},
	{-4.6377947119071452e+00, 1.3502215749461570e+01, -2.6155952405269751e+01, 3.8589668330738320e+01, -4.3038990303252589e+01, 3.7812927599537076e+01, -2.5113598088113736e+01, 1.2703182701888053e+01, -4.2755083391143343e+00, 9.0332828533799814e-01},
	{-3.0591317915750906e+00, 8.6417489609637368e+00, -1.4278790253808808e+01, 2.1302268283304240e+01, -2.2193853972079143e+01, 2.0873499791105353e+01, -1.3709764520609323e+01, 8.1303553577931247e+00, -2.8201643879900344e+00, 9.0332828533799470e-01},
	{-1.4071749185996769e+00, 5.6904141470697560e+00, -5.7374718273676368e+00, 1.1958028362868911e+01, -8.5435280598354737e+00, 1.1717345583835968e+01, -5.5088290876998691e+00, 5.3536787286077665e+00, -1.2972519209655604e+00, 9.0332828533799980e-01},
	{8.2010906117760274e-01, 5.1673756579268604e+00, 3.2580350909220908e+00, 1.0392903763919191e+01, 4.8101776408669057e+00, 1.0183724507092506e+01, 3.1282000712126736e+00, 4.8615933365571973e+00, 7.5604535083144853e-01, 9.0332828533799969e-01},
	{2.7080869856154481e+00, 7.8319071217995537e+00, 1.2201607990980708e+01, 1.8651500443681556e+01, 1.8758157568004464e+01, 1.8276088095998926e+01, 1.1715361303018827e+01, 7.3684394621253020e+00, 2.4965418284511718e+00, 9.0332828533799703e-01},
	{4.9479835250075910e+00, 1.4691607003177603e+01, 2.9082414772101068e+01, 4.3179839108869345e+01, 4.8440791644688900e+01, 4.2310703962394356e+01, 2.7923434247706446e+01, 1.3822186510471017e+01, 4.5614664160654375e+00, 9.0332828533799958e-01},
	{6.1701893352279864e+00, 2.0127225876810350e+01, 4.2974193398071741e+01, 6.5958045321253593e+01, 7.5230437667866795e+01, 6.4630411355740080e+01, 4.1261591079244290e+01, 1.8936128791950622e+01, 5.6881982915180593e+00, 9.0332828533800336e-01},
	{7.4092912870072425e+00, 2.6857944460290160e+01, 6.1578787811202332e+01, 9.8258255839887511e+01, 1.1359460153696327e+02, 9.6280452143026380e+01, 5.9124742025776612e+01, 2.5268527576524320e+01, 6.8305064480743436e+00, 9.0332828533800535e-01},
	{8.5743055776347692e+00, 3.4306584753117903e+01, 8.4035290411037110e+01, 1.3928510844056831e+02, 1.6305115418161648e+02, 1.3648147221895817e+02, 8.0686288623299944e+01, 3.2276361903872200e+01, 7.9045143816244963e+00, 9.0332828533800003e-01}
};

// Constant used to hold all of the IIR B coefficient values (10x11).
const double FILTER_IIR_B_COEFF[FILTER_NUMBER_OF_PLAYERS][FILTER_IIR_B_COEFF_COUNT] = {
	{9.0928451882350956e-10,  -0.0000000000000000e+00,  -4.5464225941175478e-09,  -0.0000000000000000e+00,   9.0928451882350956e-09,  -0.0000000000000000e+00,  -9.0928451882350956e-09,  -0.0000000000000000e+00,   4.5464225941175478e-09,  -0.0000000000000000e+00,  -9.0928451882350956e-10},
	{9.0928639888111007e-10,   0.0000000000000000e+00,  -4.5464319944055494e-09,   0.0000000000000000e+00,   9.0928639888110988e-09,   0.0000000000000000e+00,  -9.0928639888110988e-09,   0.0000000000000000e+00,   4.5464319944055494e-09,   0.0000000000000000e+00,  -9.0928639888111007e-10},
	{9.0928646492642129e-10,   0.0000000000000000e+00,  -4.5464323246321064e-09,   0.0000000000000000e+00,   9.0928646492642127e-09,   0.0000000000000000e+00,  -9.0928646492642127e-09,   0.0000000000000000e+00,   4.5464323246321064e-09,   0.0000000000000000e+00,  -9.0928646492642129e-10},
	{9.0928706182467255e-10,   0.0000000000000000e+00,  -4.5464353091233625e-09,   0.0000000000000000e+00,   9.0928706182467251e-09,   0.0000000000000000e+00,  -9.0928706182467251e-09,   0.0000000000000000e+00,   4.5464353091233625e-09,   0.0000000000000000e+00,  -9.0928706182467255e-10},
	{9.0928658835421734e-10,   0.0000000000000000e+00,  -4.5464329417710870e-09,   0.0000000000000000e+00,   9.0928658835421740e-09,   0.0000000000000000e+00,  -9.0928658835421740e-09,   0.0000000000000000e+00,   4.5464329417710870e-09,   0.0000000000000000e+00,  -9.0928658835421734e-10},
	{9.0928659616426674e-10,  -0.0000000000000000e+00,  -4.5464329808213341e-09,  -0.0000000000000000e+00,   9.0928659616426682e-09,  -0.0000000000000000e+00,  -9.0928659616426682e-09,  -0.0000000000000000e+00,   4.5464329808213341e-09,  -0.0000000000000000e+00,  -9.0928659616426674e-10},
	{9.0928348598308410e-10,  -0.0000000000000000e+00,  -4.5464174299154200e-09,  -0.0000000000000000e+00,   9.0928348598308400e-09,  -0.0000000000000000e+00,  -9.0928348598308400e-09,  -0.0000000000000000e+00,   4.5464174299154200e-09,  -0.0000000000000000e+00,  -9.0928348598308410e-10},
	{9.0929582752648066e-10,   0.0000000000000000e+00,  -4.5464791376324036e-09,   0.0000000000000000e+00,   9.0929582752648073e-09,   0.0000000000000000e+00,  -9.0929582752648073e-09,   0.0000000000000000e+00,   4.5464791376324036e-09,   0.0000000000000000e+00,  -9.0929582752648066e-10},
	{9.0926389052076022e-10,   0.0000000000000000e+00,  -4.5463194526038007e-09,   0.0000000000000000e+00,   9.0926389052076014e-09,   0.0000000000000000e+00,  -9.0926389052076014e-09,   0.0000000000000000e+00,  4.5463194526038007e-09,   0.0000000000000000e+00,  -9.0926389052076022e-10},
	{9.0906203307668878e-10,   0.0000000000000000e+00,  -4.5453101653834434e-09,   0.0000000000000000e+00,   9.0906203307668868e-09,   0.0000000000000000e+00,  -9.0906203307668868e-09,   0.0000000000000000e+00,   4.5453101653834434e-09,   0.0000000000000000e+00,  -9.0906203307668878e-10},
};

// Define the static variables used in the filter program (xQueue, yQueue, zQueue, outputQueue, and the last power computed).
static queue_t xQueue;
static queue_t yQueue;
static queue_t zQueue[FILTER_NUMBER_OF_PLAYERS];
static queue_t outputQueue[FILTER_NUMBER_OF_PLAYERS];
static double last_power_computed = 0.0;

/*********************************************************************************************************/
/* Function: square                                                                                      */
/* Purpose: To return the square of a value passed to the function.                                      */
/* Returns: A double value that is the square of the input.                                              */
/*********************************************************************************************************/
double square(double value)
{
	// Return the square value of the passed value (x^2, or value^2).
    return value * value;
}

/*********************************************************************************************************/
/* Function: initXQueue                                                                                  */
/* Purpose: To initialize the xQueue for the program.                                                    */
/* Returns: VOID                                                                                         */
/*********************************************************************************************************/
void initXQueue()
{
	// Initialize the queue and fill it with init values.
    queue_init(&xQueue, FILTER_X_QUEUE_SIZE, "xQueue");
    filter_fillQueue(&xQueue, FILTER_QUEUE_INIT_VALUE);
}

/*********************************************************************************************************/
/* Function: initYQueue                                                                                  */
/* Purpose: To initialize the yQueue for the program.                                                    */
/* Returns: VOID                                                                                         */
/*********************************************************************************************************/
void initYQueue()
{
	// Initialize the queue and fill it with init values.
    queue_init(&yQueue, FILTER_Y_QUEUE_SIZE, FILTER_Y_QUEUE_NAME);
    filter_fillQueue(&yQueue, FILTER_QUEUE_INIT_VALUE);
}

/*********************************************************************************************************/
/* Function: initZQueue                                                                                  */
/* Purpose: To initialize the zQueue for the program.                                                    */
/* Returns: VOID                                                                                         */
/*********************************************************************************************************/
 void initZQueue()
{
	// For every single number of players (the number of filters used).
    for(uint8_t i = 0; i < FILTER_NUMBER_OF_PLAYERS; i++)
    {
		// Create a temporary string variable (used for the queue name).
    	char temp_string[QUEUE_STRING_SIZE];

		// Create the queue name, initialize the queue, and fill it with queue init values.
        sprintf(temp_string, "%s #%d", FILTER_FILTER_Z_QUEUE_NAME, i);
        queue_init(&zQueue[i], FILTER_Z_QUEUE_SIZE, temp_string);
        filter_fillQueue(&zQueue[i], FILTER_QUEUE_INIT_VALUE);
    }
}

/*********************************************************************************************************/
/* Function: initOutputQueue                                                                             */
/* Purpose: To initialize the output queue for the program.                                              */
/* Returns: VOID                                                                                         */
/*********************************************************************************************************/
void initOutputQueue()
{
	// For every single number of players (the number of filters used).
    for(uint8_t i = 0; i < FILTER_NUMBER_OF_PLAYERS; i++)
	{
		// Create a temporary string variable (used for the queue name).
		char temp_string[QUEUE_STRING_SIZE];

		// Create the queue name, initialize the queue, and fill it will queue init values.
		sprintf(temp_string, "%s #%d", FILTER_OUTPUT_QUEUE_NAME, i);
        queue_init(&(outputQueue[i]), FILTER_OUTPUT_QUEUE_SIZE, temp_string);
        filter_fillQueue(&outputQueue[i], FILTER_QUEUE_INIT_VALUE);
    }
}

/*********************************************************************************************************/
/* Function: filter_init                                                                                 */
/* Purpose: To initialize all of the queues in the program.                                              */
/* Returns: VOID                                                                                         */
/*********************************************************************************************************/
void filter_init(){
	// Initialize the x, y, z, and output queue for use in the program.
    initXQueue();
    initYQueue();
    initZQueue();
    initOutputQueue();
}

/*********************************************************************************************************/
/* Function: filter_addNewInput                                                                          */
/* Purpose: To copy an input into the input queue of the FIR-filter (xQueue).                            */
/* Returns: VOID                                                                                         */
/*********************************************************************************************************/
void filter_addNewInput(double x){
	// Push the input (x) onto the xQueue.
    queue_overwritePush(&xQueue, x);
}

/*********************************************************************************************************/
/* Function: filter_fillQueue                                                                            */
/* Purpose: Fills a queue with the given fillValue.                                                      */
/* Returns: VOID                                                                                         */
/*********************************************************************************************************/
void filter_fillQueue(queue_t* q, double fillValue)
{
	// For every element in the passed queue.
    for(uint i = 0; i < q->size; i++)
    {
		// Push the passed fill value into the queue.
        queue_overwritePush(q, fillValue);
    }
}

/*********************************************************************************************************/
/* Function: filter_firFilter                                                                            */
/* Purpose: Invokes the FIR-filter. Input is contents of xQueue.                                         */
/* Returns: A double value that is the value pushed onto the yQueue.                                     */
/*********************************************************************************************************/
double filter_firFilter(){
	// Create a temporary queue data type and initialize it with a queue init value.
    queue_data_t temp_y = FILTER_QUEUE_INIT_VALUE;

	// For every FIR B coefficient value.
    for (uint8_t i = 0; i < FILTER_FIR_B_COEFF_COUNT; i++)
    {
		// Take the current value in the temporary queue data type and multiply it by the current xQueue value and B coefficient value.
        temp_y += queue_readElementAt(&xQueue, FILTER_FIR_B_COEFF_COUNT - 1 - i) * FILTER_FIR_B_COEFF[i];
    }

	// Push the temporary queue data type onto the yQueue.
    queue_overwritePush(&yQueue, temp_y);

	// Return the temporary queue data type.
    return temp_y;
}

/*********************************************************************************************************/
/* Function: filter_iirFilter                                                                            */
/* Purpose: To invoke a single iir filter. Input comes from yQueue.                                      */
/* Returns: A double value that is the value pushed onto the zQueue[filterNumber].                       */
/*********************************************************************************************************/
double filter_iirFilter(uint16_t filterNumber){
	// Create two temporary queue data type variables and initialize them with queue init values.
    queue_data_t temp_z1 = FILTER_QUEUE_INIT_VALUE;
    queue_data_t temp_z2 = FILTER_QUEUE_INIT_VALUE;

	// For every IIR B coefficient value.
    for (uint8_t i = 0; i < FILTER_IIR_B_COEFF_COUNT; i++)
    {
		// Take the current value of the temporary queue value (z1) and add it to the current yQueue value and B coefficient value.
        temp_z1 += queue_readElementAt(&yQueue, FILTER_IIR_B_COEFF_COUNT - 1 - i) * FILTER_IIR_B_COEFF[filterNumber][i];
    }

	// For every IIR A coefficient value.
    for (uint8_t i = 0; i < FILTER_IIR_A_COEFF_COUNT; i++)
    {
		// Take the current value of the temporary queue value (z2) and add it to the current zQueue value and A coefficient value.
        temp_z2 += queue_readElementAt(&zQueue[filterNumber], FILTER_IIR_A_COEFF_COUNT -1 - i) * FILTER_IIR_A_COEFF[filterNumber][i];
    }

	// Push the value of the temporary queue data types z1 and z2 into the zQueue. Also push that value onto the outputQueue.
    queue_overwritePush(&zQueue[filterNumber], (temp_z1 - temp_z2));
    queue_overwritePush(&outputQueue[filterNumber], (temp_z1 - temp_z2));

	// Return the difference between the temporary queue data types z1 and z2.
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
double previous_power[FILTER_NUMBER_OF_PLAYERS] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
double current_power[FILTER_NUMBER_OF_PLAYERS] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
double powerVals[FILTER_NUMBER_OF_PLAYERS];
static double OLDEST_POWER[FILTER_NUMBER_OF_PLAYERS] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

double filter_computePower(uint16_t filterNumber, bool forceComputeFromScratch, bool debugPrint){
    double temp_power = 0.0;
    double prev_power = 0.0;
    double newest_value = 0.0;
    double new_power = 0.0;

    if (forceComputeFromScratch)
    {
		OLDEST_POWER[filterNumber] = queue_readElementAt(&outputQueue[filterNumber], 0);

		for(uint16_t i = 0; i < FILTER_OUTPUT_QUEUE_SIZE; i++){
					//queue_readElementAt
			prev_power += queue_readElementAt(&outputQueue[filterNumber], i)*queue_readElementAt(&outputQueue[filterNumber], i);
		}

		previous_power[filterNumber] = prev_power;
		current_power[filterNumber] = prev_power;
    }

    else
    {
        newest_value = queue_readElementAt(&outputQueue[filterNumber], queue_elementCount(&outputQueue[filterNumber]) - 1);
		new_power = previous_power[filterNumber] - (OLDEST_POWER[filterNumber]*OLDEST_POWER[filterNumber]) + (newest_value*newest_value);
		current_power[filterNumber] = new_power;
		OLDEST_POWER[filterNumber] = outputQueue[filterNumber].data[outputQueue[filterNumber].indexOut];
		previous_power[filterNumber] = new_power;
    }
    
   return current_power[filterNumber];
}

/*********************************************************************************************************/
/* Function: filter_getCurrentPowerValue                                                                 */
/* Purpose: To return the last-computed output power value for the IIR filter [filterNumber].            */
/* Returns: A double value that is the last-computed output power value for the IIR filter.              */
/*********************************************************************************************************/
double filter_getCurrentPowerValue(uint16_t filterNumber){
	// Return the current power for the passed filter number.
    return current_power[filterNumber];
}

/*********************************************************************************************************/
/* Function: filter_getCurrentPowerValue                                                                 */
/* Purpose: To copy the already computed values into a previously-declared array so that they can be     */
/*          accessed from outside the filter software by the detector.                                   */
/* Returns: VOID                                                                                         */
/*********************************************************************************************************/
void filter_getCurrentPowerValues(double powerValues[]){
	// Set the power values to the current power.
    powerValues = current_power;
}

/*********************************************************************************************************/
/* Function: filter_getNormalizedPowerValues                                                             */
/* Purpose: To normalize the previously computed power values by dividing all of the values in           */
/*          normalizedArray by the maximum power value contained in currentPowerValue[].                 */
/* Returns: VOID                                                                                         */
/*********************************************************************************************************/
void filter_getNormalizedPowerValues(double normalizedArray[], uint16_t* indexOfMaxValue){
	// Create a temporary variable to hold the largest value in the array.
    double largest_value = FILTER_QUEUE_INIT_VALUE;

	// For every single player number.
    for(uint8_t i = 0; i < FILTER_NUMBER_OF_PLAYERS; i++){
		// If the current power at the current index is greater than the temporary largest power value.
        if(current_power[i] > largest_value){
			// Set the largest power value to the current power at the current index.
            largest_value = current_power[i];
        }
    }
	
	// Set the normalized array to be the current power array.
    normalizedArray = current_power;

	// For every single player number.
    for(uint8_t j = 0; j < FILTER_NUMBER_OF_PLAYERS; j++){
		// Normalize the power array by dividing all of the values in the array by the largest value.
        normalizedArray[j] = normalizedArray[j]/largest_value;
    }
}

/*********************************************************************************************************
********************************** Verification-assisting functions. *************************************
********* Test functions access the internal data structures of the filter.c via these functions. ********
*********************** These functions are not used by the main filter functions. ***********************
**********************************************************************************************************/

/*********************************************************************************************************/
/* Function: filter_getFirCoefficientArray                                                               */
/* Purpose: To return the FIR B coefficient array.                                                       */
/* Returns: A const double pointer pointing to the FIR B coefficient array.                              */
/*********************************************************************************************************/
const double* filter_getFirCoefficientArray(){
	// Return the FIR B coefficient array.
    return FILTER_FIR_B_COEFF;
}

/*********************************************************************************************************/
/* Function: filter_getFirCoefficientCount                                                               */
/* Purpose: To return the FIR B coefficient array count (number of elements).                            */
/* Returns: An unsigned 32 bit integer value corresponding to the FIR B coefficient count.               */
/*********************************************************************************************************/
uint32_t filter_getFirCoefficientCount(){
	// Returnt he nuber of FIR B coefficients.
    return FILTER_FIR_B_COEFF_COUNT;
}

/*********************************************************************************************************/
/* Function: filter_getIirACoefficientArray                                                              */
/* Purpose: To return the IIR A coefficient array.                                                       */
/* Returns: A const double pointer pointing to the IIR A coefficient array.                              */
/*********************************************************************************************************/
const double* filter_getIirACoefficientArray(uint16_t filterNumber){
	// Return the IIR A coefficient array (at the passed filter number).
    return FILTER_IIR_A_COEFF[filterNumber];
}

/*********************************************************************************************************/
/* Function: filter_getIirACoefficientCount                                                              */
/* Purpose: To return the IIR A coefficient array count (number of elements).                            */
/* Returns: An unsigned 32 bit integer value corresponding to the IIR A coefficient count.               */
/*********************************************************************************************************/
uint32_t filter_getIirACoefficientCount(){
	// Return the number of IIR A coefficients.
    return FILTER_IIR_A_COEFF_COUNT;
}

/*********************************************************************************************************/
/* Function: filter_getIirBCoefficientArray                                                              */
/* Purpose: To return the IIR B coefficient array.                                                       */
/* Returns: A const double pointer pointing to the IIR B coefficient array.                              */
/*********************************************************************************************************/
const double* filter_getIirBCoefficientArray(uint16_t filterNumber){
	// Return the IIR B coefficient array (at the passed filter number).
    return FILTER_IIR_B_COEFF[filterNumber];
}

/*********************************************************************************************************/
/* Function: filter_getIirBCoefficientCount                                                              */
/* Purpose: To return the IIR B coefficient array count (number of elements).                            */
/* Returns: An unsigned 32 bit integer value corresponding to the IIR B coefficient count.               */
/*********************************************************************************************************/
uint32_t filter_getIirBCoefficientCount(){
	// Return the number of IIR B coefficients.
    return FILTER_IIR_B_COEFF_COUNT;
}

/*********************************************************************************************************/
/* Function: filter_getYQueueSize                                                                        */
/* Purpose: To return the size of the yQueue.                                                            */
/* Returns: An unsigned 32 bit integer value corresponding to the size of the yQueue.                    */
/*********************************************************************************************************/
uint32_t filter_getYQueueSize(){
	// Return the size of the yQueue.
    return yQueue.size;
}

/*********************************************************************************************************/
/* Function: filter_getDecimationValue                                                                   */
/* Purpose: To return the decimation value.                                                              */
/* Returns: An unsigned 32 bit integer value corresponding to the decimation value.                      */
/*********************************************************************************************************/
uint16_t filter_getDecimationValue(){
	// Return the decimation value.
    return FILTER_DECIMATION_VALUE;
}

/*********************************************************************************************************/
/* Function: filter_getXQueue                                                                            */
/* Purpose: To return the address of the xQueue.                                                         */
/* Returns: A queue type corresponding to the address of the xQueue.                                     */
/*********************************************************************************************************/
queue_t* filter_getXQueue(){
	// Return the address of the xQueue.
    return &xQueue;
}

/*********************************************************************************************************/
/* Function: filter_getYQueue                                                                            */
/* Purpose: To return the address of the yQueue.                                                         */
/* Returns: A queue type corresponding to the address of the yQueue.                                     */
/*********************************************************************************************************/
queue_t* filter_getYQueue(){
	// Return the address of the yQueue.
    return &yQueue;
}

/*********************************************************************************************************/
/* Function: filter_getZQueue                                                                            */
/* Purpose: To return the address of the zQueue (at the passed filter number).                           */
/* Returns: A queue type corresponding to the address of the zQueue (at the passed filter number).       */
/*********************************************************************************************************/
queue_t* filter_getZQueue(uint16_t filterNumber){
	// Return the address of the zQueue for the passed filter number.
    return &zQueue[filterNumber];
}

/*********************************************************************************************************/
/* Function: filter_getIirOutputQueue                                                                    */
/* Purpose: To return the address of the outputQueue (at the passed filter number).                      */
/* Returns: A queue type corresponding to the address of the outputQueue (at the passed filter number).  */
/*********************************************************************************************************/
queue_t* filter_getIirOutputQueue(uint16_t filterNumber){
	// Return the address of the outputQueue for the passed filter number.
    return &outputQueue[filterNumber];
}

// Returns the address of the firOutputDebugQueue.
//queue_t* filter_getFirOutputDebugQueue();

//void filter_runTest();