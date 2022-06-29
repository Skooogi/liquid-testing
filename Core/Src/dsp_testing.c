/*
 * dsp_testing.c
 *
 *  Created on: 21.6.2022
 *      Author: topir
 */
#include <stdio.h>
#include "main.h"
#include "FreeRTOS.h"
#include "debugRTT.h"
#include "saved_signal.h"


void prvDSPTestingTask( void *pvParameters ) {

	/* Remove compiler warning about unused parameter. */
	( void ) pvParameters;


	int samples = 200;  // How many samples are received and sent back. Size of alloc'd buffers in int16 s.
	int bytesPsamp = 2;  // 2 bytes per data sample (int16_t)
	printf("Begin DSP testing task\n");



	// Allocate and configure RTT buffers
	int16_t allocArrayI[samples];								// Allocate memory for RTT buffer
	int16_t allocArrayQ[samples];								// Allocate memory for RTT buffer
	array2RTTbuffer(1, 1, allocArrayI, sizeof(allocArrayI));	// Configure RTT up-buffer '1'='DataOutI'
	array2RTTbuffer(1, 2, allocArrayQ, sizeof(allocArrayQ));	// Configure RTT up-buffer '2'='DataOutQ
	int16_t allocDownArrayI[samples];								// Allocate memory for RTT buffer
	int16_t allocDownArrayQ[samples];								// Allocate memory for RTT buffer
	array2RTTbuffer(-1, 1, allocDownArrayI, sizeof(allocDownArrayI));	// Configure RTT down-buffer '1'='DataInI'
	array2RTTbuffer(-1, 2, allocDownArrayQ, sizeof(allocDownArrayQ));	// Configure RTT down-buffer '2'='DataInQ'

	// READ THE DOWN BUFFER
	int16_t readDataI[samples];
	int16_t readDataQ[samples];
	unsigned NumBytes;

	/* Add 10k _SAVED_ signal points to test array ; OBSOLETE as data samples come through RTT now
	int16_t testArr[samples];
	// Save test data to I up-buffer '1'
	for (int k = 0; k<10000; k++){
		testArr[k] = savedIdata[k];
	}
	for (int k = 0; k<10000; k++){
		testArr[k] = savedQdata[k];
	}
	*/


	printf("Begin read, process, return data loop..");

	for( ;; ){
		printf("Did you read it yet?\n");


		/*
		 * DATA IN part
		 */

		NumBytes = SEGGER_RTT_Read(1, &readDataI[0], sizeof(readDataI));
		printf("Read %d bytes from down-buff 1 = 'I':\n", NumBytes);
		NumBytes = SEGGER_RTT_Read(2, &readDataQ[0], sizeof(readDataQ));

		// CHANGE to just 'if (NumBytes)' when comfortable and no prints are needed
		if (NumBytes > 9) {
			// Print out a small amount of readings
			printf(" [");
			for (int k = 0; k < 10; k++){
				printf("%d ", readDataI[k]);
			}
			printf("... %d]\n", readDataI[NumBytes/2-1]);
			printf("Read %d bytes from down-buff 2 = 'Q':\n [", NumBytes);
			for (int k = 0; k < 10; k++){
				printf("%d ", readDataQ[k]);
			}
			printf("... %d]\n", readDataQ[samples-1]);


			/*
			 * PROCESS DATA
			 */
			for (int k = 0; k < sizeof(readDataI)/bytesPsamp; k++){
				readDataI[k] = 2 * readDataI[k];
				readDataQ[k] = 4 * readDataQ[k];
			}

			/*
			 * DATA OUT part
			 */
			// WRITE DATA to up-buffers
			SEGGER_RTT_Write(1, &readDataI[0], sizeof(readDataI));	// Write I data to up-buffer '1' = I
			SEGGER_RTT_Write(2, &readDataQ[0], sizeof(readDataQ));	// Write Q data to up-buffer '2' = Q

		}
		else {
			printf("Nothing read waiting 5 secs\n");
		}

		// WAIT A LITTLE FOR NEXT LOOP (5sec)
		vTaskDelay(5000);

	}

}

