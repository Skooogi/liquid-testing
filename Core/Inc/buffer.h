/*
 * buffer.h
 *
 */

#ifndef INC_BUFFER_H_
#define INC_BUFFER_H_

#define RESULT_BUFFER_SIZE	0x1000	/* == 4096. Requirement to store 100 latest AIS messages. An AIS message of interest
									 * is 23 bytes (TODO: TBC) long, and thus 100 messages requires 100 * 23 == 2300 bytes.
									 * NOTE: Length must be a power of two for performance reasons. Probably a longer buffer
									 * could be an option as well if more capacity offers some advantage.
									 * TODO: Can be e.g. half of this length if memory capacity is a problem */
#define EOM					0x09	// == TAB character in 8 bit ASCII (not part of 6 bit ASCII) works as an end of message character

#include <stdint.h>


typedef struct resultbuffer {

	uint8_t data[RESULT_BUFFER_SIZE];	// Buffer for decoded AIS messages that wait to be sent onward to the OBC.
	uint32_t head;						// Result buffer head index.
	uint32_t tail;						// Result buffer tail index.
	uint16_t length;					// Amount of new data in the result buffer (in bytes).
	uint8_t eom;						// End of message character to separate different AIS messages in the result buffer.

} *resultbuf_t;

/* Declare the extern struct resultbuf. */
extern struct resultbuffer resultbuf;


/************* Publicly callable functions *************/
void prvBufferInit();										// Initialize result buffer state
void prvBufferReset();										// Reset result buffer state
uint32_t prvBufferMaxSize();								// Return the maximum length of the result buffer
uint32_t prvBufferLength();									// Return the length of data stored in the result buffer
uint32_t prvBufferIsEmpty();								// Check if result buffer is empty
void prvBufferPushN( uint8_t data[], uint32_t n );			// Push n bytes of data to the result buffer
uint8_t prvBufferPopN( uint8_t *buf, uint32_t n  );			// Pop n bytes of data from the result buffer
uint16_t prvBufferPopMessage();								// Pop all the bytes of the next message from the buffer and return the length of the message


#endif /* INC_BUFFER_H_ */
