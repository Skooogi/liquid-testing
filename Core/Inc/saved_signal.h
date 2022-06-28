/*
 * saved_signal.h
 *
 *  Created on: 20.6.2022
 *      Author: topir
 */

#ifndef CORE_INC_SAVED_SIGNAL_H_
#define CORE_INC_SAVED_SIGNAL_H_



extern const int16_t savedIdata[20000];
extern const int16_t savedQdata[20000];

/*
 * EXAMPLE FOR WRITING 10k saved signal to RTT up-buffer

int16_t allocArray[10000];							// Allocate memory for RTT buffer
array2RTTbuffer(1, 1, allocArray, sizeof(allocArray));	// Configure RTT up-buffer '1'='DataOut'
int16_t testArr[10000];								// Add 10k saved signal points to test array
for (int k = 0; k<10000; k++){
	testArr[k] = savedIdata[k];
}
SEGGER_RTT_Write(1, &testArr[0], sizeof(testArr));	// Write the data to RTT up-buffer '1'
*/

#endif /* CORE_INC_SAVED_SIGNAL_H_ */
