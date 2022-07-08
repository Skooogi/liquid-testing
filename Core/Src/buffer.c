/*
 * buffer.c
 *
 * This file contains the implementation of the ring buffer that is responsible for
 * storing the decoded AIS messages that are to be sent to the OBC. The messages are
 * sent to the OBC only on-demand, and the newest messages are sent first. Buffer overflow
 * is ignored and oldest data is overwritten.
 *
 */


#include "buffer.h"
#include "main.h"



/* Define instance of the result buffer. */
struct resultbuffer resultbuf;


/* Initialize the result buffer state */
void prvBufferInit()
{
	resultbuf.head = 0;
	resultbuf.tail = 0;
	resultbuf.length = 0;
	resultbuf.eom = EOM;
}


/* Reset the result buffer state */
void prvBufferReset()
{
	resultbuf.head = 0;
	resultbuf.tail = 0;
	resultbuf.length = 0;
}


/* Push a single byte of data to the result buffer. */
static void prvBufferPush( uint8_t data )
{
	resultbuf.data[resultbuf.head] = data;									// Store data at the head of the circular buffer
	resultbuf.head = (resultbuf.head + 1) & (RESULT_BUFFER_SIZE - 1);		// Update the buffer head index
	if (prvBufferLength() == prvBufferMaxSize())
	{																		// Check if buffer is full
		resultbuf.tail = (resultbuf.head + 1) & (RESULT_BUFFER_SIZE - 1);	// Buffer full --> overwrite data (buffer tail points to the index in front of the head)
	}
	else																	// Buffer not full
	{
		resultbuf.length += 1;												// Buffer contains one sample more than before
	}
}


/* Returns the maximum capacity of the result buffer in bytes. */
uint32_t prvBufferMaxSize()
{
	return RESULT_BUFFER_SIZE;
}


/* Ask the length of the buffer, e.g. how many bytes of data is currently contained. */
uint32_t prvBufferLength()
{
	return resultbuf.length;
}


/* Ask whether buffer is empty. */
uint32_t prvBufferIsEmpty()
{
	return (prvBufferLength() == 0);
}


/* Push n bytes of data to the result buffer. */
void prvBufferPushN( uint8_t *data, uint32_t n )
{
	for (int i=0; i<n; i++)
	{
		prvBufferPush(data[i]);		// Push a byte of data to the result buffer
	}
}


/* Pop one byte of data from the result buffer. DO NOT call for and empty result buffer. */
static uint8_t prvBufferPop()
{
		uint8_t value = resultbuf.data[resultbuf.tail];							// Get oldest byte from result buffer
		resultbuf.tail = (resultbuf.tail + 1) & (RESULT_BUFFER_SIZE - 1);		// Update tail location
		resultbuf.length--;														// Buffer contains one sample less than before
		return value;															// Return the byte popped from the buffer
}


/* Pop n bytes of data from the result buffer. */
uint8_t prvBufferPopN( uint8_t *buf, uint32_t n )
{
	int i;
	for (i=0; i<n; i++)
	{
		buf[i] = prvBufferPop();		// Take a byte from the result buffer
	}
	return 0;
}


/* Pop bytes until an end of message character and store the bytes in the array pointed to by the parameter. Return number of bytes popped. */
uint16_t prvBufferPopMessage( uint8_t *message_buf )
{
	uint16_t message_length = 0;						// Counts the number of bytes in the message
	uint8_t value;
	for ( uint32_t i=0; i<prvBufferLength(); i++ )		// Look only through existing data
	{
		value = prvBufferPop();
		if ( value == resultbuf.eom )					// Check if popped value is the end of message character
		{
			break;
		}
		else
		{
			message_buf[i] = value;						// Store the value in the given array
			message_length++;
		}
	}
	return message_length;
}


