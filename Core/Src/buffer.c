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

/* Define instance of the result buffer. */
struct result_buffer result_buf;


/* Push a single byte of data to the result buffer. */
static void buffer_push(uint8_t data)
{
	result_buf.data[result_buf.head] = data;
	result_buf.head = (result_buf.head + 1) & (RESULT_BUFFER_SIZE - 1);
	if (buffer_length() == buffer_max_size()) {
		result_buf.tail = (result_buf.head + 1) & (RESULT_BUFFER_SIZE - 1);
	} else {
		result_buf.length += 1;
	}
}


void buffer_reset()
{
	result_buf.head = 0;
	result_buf.tail = 0;
	result_buf.length = 0;
}


/* Returns the maximum capacity of the result buffer in bytes. */
int buffer_max_size() {
	return RESULT_BUFFER_SIZE;
}


/* Ask the length of the buffer, e.g. how many bytes of data is currently contained. */
int buffer_length() {
	return result_buf.length;
}


/* Ask whether buffer is empty. */
int buffer_is_empty() {
	return (buffer_length() == 0);
}


/* Push n bytes of data to the result buffer. */
void buffer_push_n(uint8_t data[], uint32_t n) {
	for (int i=0; i<n; i++) {
		buffer_push(data[i]);
	}
}


/* Pop one byte of data from the result buffer. */
static int buffer_pop() {
	if (!buffer_is_empty()) {
		uint8_t value = result_buf.data[result_buf.tail];
		result_buf.tail = (result_buf.tail + 1) & (RESULT_BUFFER_SIZE - 1);
		result_buf.length -= 1;
		return value;
	} else {
		// TODO: Handle error if function is called for an empty buffer (alternatively check buffer status before calling this)
	}
}


/* Pop a n bytes of data from the result buffer. */
int buffer_pop_n(uint8_t *buf, uint32_t n) {
	int i;
	for (i=0; i<n; i++) {
		buf[i] = buffer_pop();
	}
	return 0;
}


