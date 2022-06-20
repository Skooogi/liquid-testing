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
 * for Python Script 'jlinker.py' to handle.
 */
void array2RTTbuffer(int16_t *array, int16_t size) {
	SEGGER_RTT_ConfigUpBuffer(1, "DataOut", &array[0], size,
	                           SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
	/*
	 * Example code for using the RTT up-buffer '1'='DataOut' buffer for sending
	 * uint16_t data out. To be used in DSP chain for sending ADC data out, for example.
	 *
	int16_t allocArray[0x1000];							// Allocate memory for RTT buffer
	int16_t testArr[] = {11, 22, 33, 44, 55}; 			// Arbitrary test data array
	array2RTTbuffer(allocArray, sizeof(allocArray));	// Configure RTT up-buffer '1'='DataOut'
	SEGGER_RTT_Write(1, &testArr[0], sizeof(testArr));	// Write the data to RTT up-buffer '1'

	int16_t testArr2[] = {11, 22, 400, 20000, 32767};	// Repeat with different data values
	SEGGER_RTT_Write(1, &testArr2[0], sizeof(testArr2));
 	 */
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



