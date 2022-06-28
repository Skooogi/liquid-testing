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
void array2RTTbuffer(int up_down_flag, int buff_num, int16_t *array, int16_t size) {
	if(up_down_flag > 0){

		//const char name[] = {'D','a','t','a','O','u','t', 48+buff_num,'\0'};

		//const char numba = (char)48+buff_num;
		//char name[9] = "DataOut";
		//strncat(name, &numba, 1);
		//const char *name_ptr = name;

		//const char *name = "DataOut";

		char *name = "DataOut";
		if (buff_num == 1) {
			name = "DataOutI";
		} else if (buff_num == 2) {
			name = "DataOutQ";
		}


		SEGGER_RTT_ConfigUpBuffer(buff_num, name, &array[0], size,
								   SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
	}
	else{
		//const char name[] = {'D','a','t','a','I','n', 48+buff_num, '\0'};
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



