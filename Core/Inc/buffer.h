/*
 * buffer.h
 *
 */

#ifndef INC_BUFFER_H_
#define INC_BUFFER_H_

#define RESULT_BUFFER_SIZE 0x1000		/* == 4096. Requirement to store 100 latest AIS messages. An AIS message of interest
									 * is 23 bytes (TODO: TBC) long, and thus 100 messages requires 100 * 23 == 2300 bytes.
									 * NOTE: Length must be a power of two for performance reasons. Probably a longer buffer
									 * could be an option as well if more capacity offers some advantage. */

#include <stdint.h>

struct result_buffer {

	uint8_t data[RESULT_BUFFER_SIZE];		// Buffer for decoded AIS messages that wait to be sent onward to the OBC.
	uint32_t head;						// Result buffer head index.
	uint32_t tail;						// Result buffer tail index.
	uint16_t length;					// Amount of new data in the result buffer (in bytes).

};

/* Declare the extern struct resultbuf. */
typedef struct result_buffer *result_buffer_t;
extern struct result_buffer result_buf;

// Some static helper functions are found in buffer.c but not here.

/************* Publicly callable functions *************/
void buffer_init();
void buffer_reset();
int buffer_max_size();
int buffer_length();
int buffer_is_empty();
void buffer_push_n(uint8_t data[], uint32_t n);
int buffer_pop_n(uint8_t *buf, uint32_t n);


#endif /* INC_BUFFER_H_ */
