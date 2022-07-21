#include "debugRTT.h"

#include "main.h"
#include "FreeRTOS.h"
#include "timers.h"

#include <stdio.h>
#include <string.h>
/*
 * RTT communication and some useful functions.
 */

void readRTT(char* p) {
	*p = SEGGER_RTT_WaitKey();
}

void writeRTT(char* p) {
	SEGGER_RTT_Write(0,p,strlen(p));
}

/*
 * Simplified input for configuring an RTT up-buffer for sending data out over RTT
 * for Python scripts to handle.
 */
void array2RTTbuffer(int up_down_flag, int buff_num, int16_t *array, int16_t size) {

	if(up_down_flag > 0){
		char *name = "DataOut";
		if (buff_num == 1) {
			name = "DataOutI";
		} else if (buff_num == 2) {
			name = "DataOutQ";
		}
		SEGGER_RTT_ConfigUpBuffer(buff_num, name, &array[0], size,
								   SEGGER_RTT_MODE_NO_BLOCK_TRIM);
	}
	else{
		char *name = "DataIn";
		if (buff_num == 1) {
			name = "DataInI";
		} else if (buff_num == 2) {
			name = "DataInQ";
		}
		SEGGER_RTT_ConfigDownBuffer(buff_num, name, &array[0], size,
		                             SEGGER_RTT_MODE_NO_BLOCK_SKIP);
	}
	/*
	 * Example code for using the RTT up-buffer '1'='DataOut' buffer for sending
	 * uint16_t data out. To be used in DSP chain for sending ADC data out, for example.
	 *
	int16_t allocArray[0x1000];							// Allocate memory for RTT buffer
	int16_t testArr[] = {11, 22, 33, 44, 55}; 			// Arbitrary test data array
	array2RTTbuffer(1, 1, allocArray, sizeof(allocArray));	// Configure RTT up-buffer '1'='DataOut'
	SEGGER_RTT_Write(1, &testArr[0], sizeof(testArr));	// Write the data to RTT up-buffer '1'

	int16_t testArr2[] = {11, 22, 400, 20000, 32767};	// Repeat with different data values
	SEGGER_RTT_Write(1, &testArr2[0], sizeof(testArr2));
 	 */
}



	/*
	 * Allocate and configure RTT buffers.
	 * Note: One additional int16_t is allocated to each buffer as
	 * 		 sacrifice for the RTT reading which bungles up the last
	 * 		 integer at the end of the buffer. This way intended sample
	 * 		 size is preserved.
	 *
	 * 	 int samples 			How many samples are received and sent back. Size of alloc'd buffers in int16 s.
	 * 	 int bytesPsamp 		How many bytes per data sample (2 for int16_t)
	 * 	 int sleeptime 			How many milliseconds should be waited in between data sample batches
	 */
void initRTTbuffers(int samples, int bytesPsamp, int sleeptime) {
	int16_t allocArrayI[samples+1];								// Allocate memory for RTT buffer
	int16_t allocArrayQ[samples+1];								// Allocate memory for RTT buffer
	array2RTTbuffer(1, 1, allocArrayI, sizeof(allocArrayI));	// Configure RTT up-buffer '1'='DataOutI'
	array2RTTbuffer(1, 2, allocArrayQ, sizeof(allocArrayQ));	// Configure RTT up-buffer '2'='DataOutQ
	int16_t allocDownArrayI[samples+1];								// Allocate memory for RTT buffer
	int16_t allocDownArrayQ[samples+1];								// Allocate memory for RTT buffer
	array2RTTbuffer(-1, 1, allocDownArrayI, sizeof(allocDownArrayI));	// Configure RTT down-buffer '1'='DataInI'
	array2RTTbuffer(-1, 2, allocDownArrayQ, sizeof(allocDownArrayQ));	// Configure RTT down-buffer '2'='DataInQ'

	// Send communication specs over RTT to python scripts
	int16_t allocArraySpecs[6];									// Allocate memory for RTT buffer
	array2RTTbuffer(1, 3, allocArraySpecs, sizeof(allocArraySpecs));// Configure RTT up-buffer '3'='DataOut'
	int16_t specs[6] = {samples, bytesPsamp, sleeptime, samples, bytesPsamp, sleeptime};
	SEGGER_RTT_Write(3, &specs[0], sizeof(specs));

}




//Listens to Terminal 0 and awaits user input.
void prvDebugRTT( void *pvParameters ) {

	( void ) pvParameters;

	char* text = calloc(256, 1);
	writeRTT("Debugger online!");

	for( ;; )
	{
		readRTT(text); //Halts task until user sends input.
		writeRTT(text); //Prints the input back into the terminal.
	}

	free(text);
}

void pulseLED(int ms_duration, int ms_pause) {
	HAL_GPIO_TogglePin(GPIOB, CANLED_Pin);
	vTaskDelay(ms_duration);
	HAL_GPIO_TogglePin(GPIOB, CANLED_Pin);
	vTaskDelay(ms_pause);
}


/*
 * Test function that prints a given 'array' of 'size' line by line to RTT viewer.
 * Function waits for user to answer to question "Print out buffer?" to
 * allow time for starting terminal logging in RTT viewer. Function is
 * also supposed to wait for user to stop terminal logging (doesn't work).
 * NOTE: This is an old function and its preffered to use 'array2RTTbuffer(..)'
 * 		 instead as it communicates directly with python. -Topi
 */
void printData(int16_t *array, int16_t size) {

	char ans;
	printf("Start terminal logger. Print out buffer? y or n \n");
	readRTT(&ans);
	writeRTT(&ans);
	// Print out first number of data samples
	printf("\n%d\n", size);

	// Print out the data
	if (ans == 'y') {
		for( int16_t j = 0; j < size; j++){
			printf("%d\n", array[j]);
		}
	}else{
		printf("Answer 'n'\n");
	}

	printf("End logging. Continue? (press any key)\n");
	//For some reason it does not wait for input here?
	readRTT(&ans);
	writeRTT(&ans);
}



