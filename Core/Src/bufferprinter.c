/*
 *
 * This script tests using the J-Link RTT connection for data transfer. The intended use is to transfer ADC data
 * for plotting and debugging on host PC. This file only tests sending a buffer of integers to be saved in a file.
 *
 */

#include <stdio.h>

#include "SEGGER_RTT.h"
#include "debugRTT.h"

/*
 * Test function that prints a given 'array' of 'size' line by line.
 * Function waits for user to answer to question "Print out buffer?" to
 * allow time for starting terminal logging in RTT viewer. Function is
 * also supposed to wait for user to stop terminal logging (doesn't work).
 */
void printData(uint32_t *array, uint32_t size) {

	char ans = 'n';
	printf("Print out buffer? y or n \n");
	readRTT(&ans);

	if (ans == 'y') {
		for( uint32_t j = 0; j < size; j++){
			printf("%d\n", array[j]);
		}
	}else{
		printf("Answer 'n'\n");
	}

	printf("End logging. Continue? (press any key)\n");
	//For some reason it does not wait for input here?
	readRTT(&ans);
	printf("%c", ans);
}
