/*
 * buffer.c
 *
 * This file contains the implementation of the ring buffer that is responsible for
 * storing the decoded AIS messages that are to be sent to the OBC. The messages are
 * sent to the OBC only on-demand, and the newest messages are sent first. Buffer overflow
 * is ignored.
 *
 */


#include "buffer.h"
#include "main.h"


/* Define instance of the result buffer. */
struct resultbuffer resultbuf;


/* Push a single byte of data to the result buffer. */
static void buffer_push(uint8_t data)
{
	resultbuf.data[resultbuf.head] = data;									// Store data at the head of the circular buffer
	resultbuf.head = (resultbuf.head + 1) & (RESULT_BUFFER_SIZE - 1);		// Update the buffer head index
	if (buffer_length() == buffer_max_size()) {								// Check if buffer is full
		resultbuf.tail = (resultbuf.head + 1) & (RESULT_BUFFER_SIZE - 1);	// Buffer full --> overwrite data (buffer tail points to the index in front of the head)
	} else {																// Buffer not full
		resultbuf.length += 1;												// Increase the buffer length by one
	}
}


/* Reset the result buffer state */
void buffer_reset()
{
	resultbuf.head = 0;				// Reset head index
	resultbuf.tail = 0;				// Reset tail index
	resultbuf.length = 0;			// Reset length
}


/* Returns the maximum capacity of the result buffer in bytes. */
uint32_t buffer_max_size() {
	return RESULT_BUFFER_SIZE;
}


/* Ask the length of the buffer, e.g. how many bytes of data is currently contained. */
uint32_t buffer_length() {
	return resultbuf.length;
}


/* Ask whether buffer is empty. */
uint32_t buffer_is_empty() {
	return (buffer_length() == 0);
}


/* Push n bytes of data to the result buffer. */
void buffer_push_n(uint8_t *data, uint32_t n) {
	for (int i=0; i<n; i++) {
		buffer_push(data[i]);		// Push a byte of data to the result buffer
	}
}


/* Pop one byte of data from the result buffer. */
static uint8_t buffer_pop() {
	if (!buffer_is_empty()) {													// Check that buffer is not empty
		uint8_t value = resultbuf.data[resultbuf.tail];							// Get oldest byte from result buffer
		resultbuf.tail = (resultbuf.tail + 1) & (RESULT_BUFFER_SIZE - 1);		// Update tail location
		resultbuf.length--;														// Decrease buffer length
		return value;															// Return the byte popped from the buffer
	} else {
		Error_Handler();		// TODO: Handle error if function is called for an empty buffer (alternatively check buffer status before calling this)
	}
}


/* Pop a n bytes of data from the result buffer. */
uint8_t buffer_pop_n(uint8_t *buf, uint32_t n) {
	int i;
	for (i=0; i<n; i++) {
		buf[i] = buffer_pop();		// Pop a byte from the result buffer
	}
	return 0;
}


