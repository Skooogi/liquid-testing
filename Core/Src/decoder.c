/*
 * decoder.c
 *
 */

#include "decoder.h"
#include "main.h"

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


/* Decode AIS message payload */
void prvPayloadDecode( uint32_t *data )
{
	uint32_t consecutive_ones = 0;																	// Count how many consecutive ones have been encountered in the data
	uint32_t num_removed_bits = 0;																	// How many bits have been removed (destuffed)
	uint32_t decoded_idx = 0;																		// Current index of the unstuffed data buffer
	/* Extract encoded data */
	for ( uint32_t i=0; i<dr.encoded_payload_length - AIS_CRC_LENGTH; i++ )	// TODO: This might not actually be true because of stuffing bits. Fix it!!
	{

		if ( data[i] == data[i-1] )																	// It's okay to go one step back from the "first index" when i==payload_start_idx, since we need to check the level of the last it of the start flag.
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

	/* Extract encoded CRC-16 */
	for ( uint32_t i=0; i<dr.encoded_payload_length - AIS_CRC_LENGTH; i++ )
	{

		if ( data[i] == data[i-1] )																	// It's okay to go one step back from the "first index" when i==payload_start_idx, since we need to check the level of the last it of the start flag.
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

}


/* Take an array of 1s and 0s and convert it to actual binary bytes of data */
void prvBitArrayToBinary( uint32_t array_length )
{
	/* Check that the number of payload bits is a multiple of 6 (AIS message consists of 6 bit chars) */
	if ( !(array_length % 6) )
	{
		Error_Handler();			// TODO: Do something in the error hander
	}
	dr.decoded_payload_length = 0;
	/* Convert digitized data from array of "bits" to bytes */
	uint8_t byte = 0;
	uint8_t j;
	for ( uint32_t i=0; i < array_length; i+=6 )
	{
		for( j=i; j<(i+6); ++j )								// Characters in AIS payload are 6 bits long
		{
			if( dr.decoded_payload[j] == 1 ) byte |= 1 << (7-j);
		}
		dr.decoded_payload[j] = byte + 48;		// Add 48 to a 6 bit char to make it the represent the corresponding 8 bit char TODO: Verify this is all that is needed for conversion.
		byte = 0;
		dr.decoded_payload_length++;
	}
}










/* Detect AIS message start flag */
uint32_t prvDetectStartOrEndFlag(unsigned int *data)
{
	uint32_t flag_found = 1;
	uint32_t i;

	for ( i=1; i<AIS_START_END_FLAG_LENGTH-1; i++ )				// Must check the last bit separately as it should be different
	{
		if ( data[i] == data[i-1] )
		{
			continue;
		}
		else
		{
			flag_found = 0;
			return flag_found;
		}
	}
	if ( data[i] == data[i-1] )
	{
		flag_found = 0;
	}
	return flag_found;
}

