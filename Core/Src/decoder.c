/*
 * decoder.c
 *
 */

#include "decoder.h"
#include "main.h"
#include "crc.h"

/* Instance of the decoder to extract and decode AIS messages from the stream of input data. */
struct decoder dr =
{

};


/* Initialize the decoder object */
void prvDecoderInit()
{

	dr.last_sample = 0;
	dr.lastlast_sample = 0;
	dr.preamble_counter = 0;
	dr.preamble_found = 0;
	dr.startflag_counter = 0;
	dr.startflag_found = 0;
	dr.endflag_counter = 0;
	dr.endflag_found = 0;
	dr.encoded_payload_length = 0;
	dr.decoded_payload_length = 0;

}


/* Reset the state of the encoder when no message was found and searching for a new message starts */
static void prvDecoderReset()
{

	dr.preamble_counter = 0;
	dr.preamble_found = 0;
	dr.startflag_counter = 0;
	dr.startflag_found = 0;
	dr.endflag_counter = 0;
	dr.endflag_found = 0;
	dr.encoded_payload_length = 0;
	dr.decoded_payload_length = 0;

}


/* Detect AIS message preamble.
 * Preamble: 0b01010101... for 24 bits
 * NRZI encoded: 0b001100110011... or 0b110011001100... for 24 bits. */
void prvDetectPreamble(unsigned int sample)
{

	if ( dr.preamble_counter == 0 )
	{
		dr.preamble_counter++;															// The first value can be whatever (either 1 or 0), so we always find at least one suitable value
		dr.preamble_found = 0;															// Could not yet find the total preamble as we have only checked one value.
	}
	else if ( (dr.last_sample == dr.lastlast_sample) && (sample != dr.last_sample) )	// If the two previous samples were equal, the current one should not be
	{
		dr.preamble_counter++;															// Found one more

		if ( dr.preamble_counter < AIS_PREAMBLE_LENGTH )
		{
			dr.preamble_found = 0;														// Did not find complete preamble yet
		}
		else																			// Full length of preamble reached
		{
			dr.preamble_found = 1;														// Preamble found!
			dr.preamble_counter = 0;													// Preamble counter reset
		}

	}
	else if ( (dr.last_sample != dr.lastlast_sample) && (sample == dr.last_sample) )	// If the two previous samples were not equal, the current one should be equal to the previous
	{
		dr.preamble_counter++;															// Found one more
		if ( dr.preamble_counter < AIS_PREAMBLE_LENGTH )
		{
			dr.preamble_found = 0;														// Did not find complete preamble yet
		}
		else																			// Full length of preamble reached
		{
			dr.preamble_found = 1;														// Preamble found!
			dr.preamble_counter = 0;													// Preamble counter reset
		}
	}
	else																				// The sample did not fit the preamble pattern
	{
		dr.preamble_found = 0;															// Did not find preamble
		dr.preamble_counter = 0;														// Preamble counter reset
	}
	dr.lastlast_sample = dr.last_sample;
	dr.last_sample = sample;
}


/* Detect AIS message start flag.
 * Start flag: 0x7e == 0b01111110
 * NRZI encoded: 0b00000001 or 0b11111110 */
void prvDetectStartFlag(unsigned int sample)
{

	if ( dr.startflag_counter == 0 )
	{
		if ( sample != dr.last_sample )										// The first start flag value is 0 (non-NRZI encoded), so the current sample should be different from the last one (final sample of preamble)
		{
			dr.startflag_counter++;											// Found one more
			dr.startflag_found = 0;											// Have not found the whole start flag yet
		}
		else																// Sample did not match the start flag pattern
		{
			dr.startflag_counter = 0;										// Sample did not match the start flag pattern
			dr.startflag_found = 0;											// Did not find start flag
			dr.preamble_found = 0;											// Reset also preamble found flag, since we have to start looking for a preamble again.
		}
	}
	else if ( dr.startflag_counter < (AIS_START_FLAG_LENGTH - 1) )			// Must check last sample of start flag separately
	{
		if ( sample == dr.last_sample )										// Sample fits start flag pattern
		{
			dr.startflag_counter++;											// Found one more
			dr.startflag_found = 0;
		}
		else																// Sample did not match the start flag pattern
		{
			dr.startflag_counter = 0;										// Sample did not match the start flag pattern
			dr.startflag_found = 0;											// Did not find start flag
			dr.preamble_found = 0;											// Reset also preamble found flag, since we have to start looking for a preamble again.
		}
	}
	else if ( dr.startflag_counter == (AIS_START_FLAG_LENGTH - 1) )
	{
		if ( sample != dr.last_sample )										// Sample fits start flag pattern
		{
			dr.startflag_found = 1;											// Start flag found
			dr.startflag_counter = 0;										// Start flag counter reset
		}
		else																// Sample did not match the start flag pattern
		{
			dr.startflag_counter = 0;										// Sample did not match the start flag pattern
			dr.startflag_found = 0;											// Did not find start flag
			dr.preamble_found = 0;											// Reset also preamble found flag, since we have to start looking for a preamble again.
		}
	}
	else																	// Start flag not found
	{
		dr.startflag_counter = 0;											// Sample did not match the start flag pattern
		dr.startflag_found = 0;												// Did not find start flag
		dr.preamble_found = 0;												// Reset also preamble found flag, since we have to start looking for a preamble again.
	}
}


/* Detect AIS message end flag.
 * End flag: 0x7e == 0b01111110
 * NRZI encoded: 0b00000001 or 0b11111110 */
void prvDetectEndFlag( unsigned int sample )
{

	if ( dr.encoded_payload_length < (AIS_MAX_ENCODED_PAYLOAD_LENGTH + 1) )		// Look for end flag until the maximum combined length of the payload and the CRC (+ 1) has been reached
	{
		if ( dr.endflag_counter == 0 )											// The first symbol can be either 1 or 0.
		{
				dr.endflag_counter++;											// Found the first symbol (will always be found as it's value does not matter)
				dr.endflag_found = 0;											// Have not found the whole end flag yet
		}
		else if ( dr.endflag_counter < (AIS_END_FLAG_LENGTH - 1) )				// Must check last sample of end flag separately
		{
			if ( sample == dr.last_sample )										// Sample fits end flag pattern
			{
				dr.endflag_counter++;											// Found one more
				dr.endflag_found = 0;
			}
			else																// Sample did not match the end flag pattern
			{
				dr.endflag_counter = 0;											// Sample did not match the end flag pattern
				dr.endflag_found = 0;											// Did not find end flag
			}
		}
		else if ( dr.endflag_counter == (AIS_END_FLAG_LENGTH - 1) )
		{
			if ( sample != dr.last_sample )										// Sample fits end flag pattern
			{
				dr.endflag_counter++;											// Found the last one!
				dr.endflag_found = 1;											// End flag found
				dr.encoded_payload_length -= AIS_END_FLAG_LENGTH;				// Remove the length of the end flag from the length of the encoded payload
			}
			else																// Sample did not match the end flag pattern
			{
				dr.endflag_counter = 0;											// Sample did not match the end flag pattern
				dr.endflag_found = 0;											// Did not find end flag
			}
		}
		dr.encoded_payload_length++;											// Increment the length of the encoded payload
		dr.encoded_payload[dr.encoded_payload_length - 1] = sample;				// Save sample to the buffer of the encoded payload
	}
	else																		// No end flag was ever found, start searching for AIS package all over again
	{
		dr.endflag_counter = 0;													// Sample did not match the end flag pattern
		dr.endflag_found = 0;													// Did not find end flag
		dr.startflag_found = 0;													// Reset also start flag found flag, since we start all over looking for package
		dr.preamble_found = 0;													// Reset also preamble found flag, since we have to start looking for a preamble again.
		dr.encoded_payload_length = 0;											// Reset the encoded payload length, since we have to start looking for everything from the start again.
	}
}


/* Decode and extract AIS message payload and corresponding CRC-16.  */
void prvPayloadAndCRCDecode()
{
	uint32_t consecutive_ones = 0;																	// Count how many consecutive ones have been encountered in the data
	uint32_t num_removed_bits = 0;																	// How many bits have been removed (destuffed)
	uint32_t decoded_idx = 0;																		// Current index of the unstuffed data buffer
	/* Extract and decode data */
	for ( uint32_t i=0; i<dr.encoded_payload_length; i++ )											// Extract the whole payload (including CRC)
	{

		if ( dr.encoded_payload[i] == dr.encoded_payload[i-1] )																	// It's okay to go one step back from the "first index" when i==payload_start_idx, since we need to check the level of the last it of the start flag.
		{
			dr.decoded_payload[decoded_idx] = 1;													// Value same as previous --> found 1
			consecutive_ones++;
			if (consecutive_ones > 4)
			{
				i++;																				// Skip the stuffing 0
				num_removed_bits++;																	// Increment the removed bits counter
				consecutive_ones = 0;																// Reset the counter of consecutive ones
				continue;
			}
		}
		else
		{
			dr.decoded_payload[decoded_idx] = 0;													// Value not same as previous --> found 0
			consecutive_ones = 0;																	// Reset the counter of consecutive ones
		}
		decoded_idx++;
	}
	dr.decoded_payload_length = dr.encoded_payload_length - num_removed_bits;

	/* Extract and decode CRC-16 */
	memcpy( dr.decoded_crc, (dr.decoded_payload + dr.decoded_payload_length - AIS_CRC_LENGTH), AIS_CRC_LENGTH );	// Copy the CRC-16 to a separate place
	dr.decoded_payload_length = dr.encoded_payload_length - AIS_CRC_LENGTH;											// Update the payload length now that CRC is not included
}


/* Check if the received CRC equals the one that is counted based on the payload data. */
uint32_t prvCheckPayloadCRC()
{
	return ( dr.crc16 == prvCRC16( dr.decoded_payload, dr.decoded_payload_length ) );
}


/* Take the CRC-16 array of 1s and 0s and convert it to actual binary bytes of data */
void prvCRCToBytes()
{

	/* Convert digitized CRC from array of "bits" to bytes */
	uint8_t byte = 0;
	uint32_t byte_idx = 0;											// Index for completed bytes
	uint8_t crc_array[2];
	uint8_t j;
	for ( uint32_t i=0; i < AIS_CRC_LENGTH; i+=8 )					// Move always 8 bits forward to jump to the next byte
	{
		for( j=i; j<(i+8); ++j )									// 8 bits makes a byte
		{
			if( dr.decoded_crc[j] == 1 ) byte |= 1 << (7-j);		// Bitshift to get the bits in their correct locations TODO: Verify this works correctly
		}
		crc_array[byte_idx] = byte;									// Temporarily store the two bytes making up the CRC in an array
		byte = 0;
		byte_idx++;
	}
	dr.crc16 = (crc_array[0] << 8) | crc_array[1];
}


/* Take the payload array of 1s and 0s and convert it to actual binary bytes of data */
void prvPayloadToBytes()
{
	/* Check that the number of payload bits is a multiple of 6 (AIS message consists of 6 bit chars) */
	if ( !( dr.decoded_payload_length % 6) )
	{
		Error_Handler();
	}
	dr.ascii_message_length = 0;
	/* Convert digitized payload from array of "bits" to bytes */
	uint8_t byte = 0;
	uint32_t byte_idx = 0;										// Index for copmleted bytes
	uint32_t j;
	for ( uint32_t i=0; i < dr.decoded_payload_length; i+=6 )		// Move always 6 bits forward to jump to the next char
	{
		for( j=i; j<(i+6); ++j )									// Characters in AIS payload are 6 bits long
		{
			if( dr.decoded_payload[j] == 1 ) byte |= 1 << (7-j);	// Bitshift to get the bits in their correct locations TODO: Verify this works correctly, especially for our 6 bit chars
		}
		dr.ascii_message[byte_idx] = byte + 48;						// Add 48 to a 6 bit char to make it the represent the corresponding 8 bit char TODO: Verify this is all that is needed for conversion.
		byte = 0;													// Reset byte for next one
		byte_idx++;
		dr.ascii_message_length++;									// Count how many bytes (chars) have been found
	}
}


