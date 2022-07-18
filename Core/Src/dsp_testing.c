/*
 * dsp_testing.c
 *
 *  Created on: 21.6.2022
 *      Author: topir
 */

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "debugRTT.h"
#include "saved_signal.h"


void prvDSPTestingTask( void *pvParameters ) {

	/* Remove compiler warning about unused parameter. */
	( void ) pvParameters;


	int samples = 500;  	// How many samples are received and sent back. Size of alloc'd buffers in int16 s.
	int bytesPsamp = 2; 	// 2 bytes per data sample (int16_t)
	int sleeptime = 2000;
	printf("Begin DSP testing task \n");

	//initRTTbuffers(samples, bytesPsamp, sleeptime);
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

	// Allocate buffers for read data
	int16_t readDataI[samples];
	int16_t readDataQ[samples];
	uint32_t numBytesI, numBytesQ, batch;


	printf("Begin read, process, return data loop..\n");

	for( ;; ){
		/*
		 * DATA IN part
		 */
		numBytesI = SEGGER_RTT_Read(1, &readDataI[0], sizeof(readDataI));
		printf("Read %d bytes from down-buff 1 = 'I':\n", (int)numBytesI);
		numBytesQ = SEGGER_RTT_Read(2, &readDataQ[0], sizeof(readDataQ));

		// CHANGE to just 'if (numBytes)' when comfortable and no prints are needed
		if (numBytesI > 9) {
			// Print out a small amount of readings
			printf(" [");
			for (int k = 0; k < 10; k++){
				printf("%d ", readDataI[k]);
			}
			printf("... %d]\n", readDataI[numBytesI/bytesPsamp - 1]);
			printf("Read %d bytes from down-buff 2 = 'Q':\n [", (int)numBytesQ);
			for (int k = 0; k < 10; k++){
				printf("%d ", readDataQ[k]);
			}
			printf("... %d]\n", readDataQ[numBytesQ/bytesPsamp - 1]);


			/*
			 * PROCESS DATA
			 */
			for (int k = 0; k < sizeof(readDataI)/bytesPsamp; k++){
				readDataI[k] = 1 * readDataI[k];
				readDataQ[k] = 1 * readDataQ[k];
			}

			/*
			 * DATA OUT part
			 */
			// WRITE DATA to up-buffers
			numBytesI = SEGGER_RTT_Write(1, &readDataI[0], numBytesI);	// Write I data to up-buffer '1' = I
			numBytesQ = SEGGER_RTT_Write(2, &readDataQ[0], numBytesQ);	// Write Q data to up-buffer '2' = Q
			if (numBytesI > 0) printf("Send I data back, ");
			if (numBytesQ > 0) printf("Send Q data back, ");
			batch++;
			printf("Batch num: %d; ", (int)batch);

		}
		else {
			batch = 0;
			printf("Nothing read; ");
		}

		// WAIT A LITTLE FOR NEXT LOOP (5sec)
		printf("waiting %d secs\n\n", (sleeptime/1000));
		vTaskDelay(sleeptime);
	}


}

