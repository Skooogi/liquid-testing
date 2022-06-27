/*
 * dsp_testing.c
 *
 *  Created on: 21.6.2022
 *      Author: topir
 */
#include <stdio.h>
#include "main.h"
#include "freeRTOS.h"
#include "debugRTT.h"
#include "saved_signal.h"


void prvDSPTestingTask( void *pvParameters ) {

	/* Remove compiler warning about unused parameter. */
	( void ) pvParameters;

	printf("Begin DSP testing task\n");

	int16_t allocArrayI[10000];									// Allocate memory for RTT buffer
	array2RTTbuffer(1, 1, allocArrayI, sizeof(allocArrayI));	// Configure RTT up-buffer '1'='DataOutI'
	int16_t allocArrayQ[10000];									// Allocate memory for RTT buffer
	array2RTTbuffer(1, 2, allocArrayQ, sizeof(allocArrayQ));	// Configure RTT up-buffer '2'='DataOutQ'

	int16_t testArr[10000];	// Add 10k saved signal points to test array
	// Save test data to I up-buffer '1'
	for (int k = 0; k<10000; k++){
		testArr[k] = savedIdata[k];
	}
	SEGGER_RTT_Write(1, &testArr[0], sizeof(testArr));	// Write the data to RTT up-buffer '1'
	// THEN Q, up-buffer number '2'
	for (int k = 0; k<10000; k++){
		testArr[k] = savedQdata[k];
	}
	SEGGER_RTT_Write(2, &testArr[0], sizeof(testArr));	// Write the data to RTT up-buffer '2'

	int16_t allocDownArrayI[10000];								// Allocate memory for RTT buffer
	array2RTTbuffer(-1, 1, allocDownArrayI, sizeof(allocArrayI));	// Configure RTT down-buffer '1'='DataInI'
	int16_t allocDownArrayQ[10000];								// Allocate memory for RTT buffer
	array2RTTbuffer(-1, 2, allocDownArrayQ, sizeof(allocArrayQ));	// Configure RTT down-buffer '2'='DataInQ'


	for( ;; ){
		printf("Did you read it yet?\n");
		vTaskDelay(1000);
	}

}

