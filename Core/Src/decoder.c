/*
 * decoder.c
 *
 */

#include "decoder.h"
#include <string.h>


/* Lookup table for 16 bit CRC-CCITT */
static uint16_t crc16_table [256] = {

	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5,
	0x60c6, 0x70e7, 0x8108, 0x9129, 0xa14a, 0xb16b,
	0xc18c, 0xd1ad, 0xe1ce, 0xf1ef, 0x1231, 0x0210,
	0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
	0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c,
	0xf3ff, 0xe3de, 0x2462, 0x3443, 0x0420, 0x1401,
	0x64e6, 0x74c7, 0x44a4, 0x5485, 0xa56a, 0xb54b,
	0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6,
	0x5695, 0x46b4, 0xb75b, 0xa77a, 0x9719, 0x8738,
	0xf7df, 0xe7fe, 0xd79d, 0xc7bc, 0x48c4, 0x58e5,
	0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
	0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969,
	0xa90a, 0xb92b, 0x5af5, 0x4ad4, 0x7ab7, 0x6a96,
	0x1a71, 0x0a50, 0x3a33, 0x2a12, 0xdbfd, 0xcbdc,
	0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
	0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03,
	0x0c60, 0x1c41, 0xedae, 0xfd8f, 0xcdec, 0xddcd,
	0xad2a, 0xbd0b, 0x8d68, 0x9d49, 0x7e97, 0x6eb6,
	0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
	0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a,
	0x9f59, 0x8f78, 0x9188, 0x81a9, 0xb1ca, 0xa1eb,
	0xd10c, 0xc12d, 0xf14e, 0xe16f, 0x1080, 0x00a1,
	0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c,
	0xe37f, 0xf35e, 0x02b1, 0x1290, 0x22f3, 0x32d2,
	0x4235, 0x5214, 0x6277, 0x7256, 0xb5ea, 0xa5cb,
	0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
	0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447,
	0x5424, 0x4405, 0xa7db, 0xb7fa, 0x8799, 0x97b8,
	0xe75f, 0xf77e, 0xc71d, 0xd73c, 0x26d3, 0x36f2,
	0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9,
	0xb98a, 0xa9ab, 0x5844, 0x4865, 0x7806, 0x6827,
	0x18c0, 0x08e1, 0x3882, 0x28a3, 0xcb7d, 0xdb5c,
	0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
	0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0,
	0x2ab3, 0x3a92, 0xfd2e, 0xed0f, 0xdd6c, 0xcd4d,
	0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 0x7c26, 0x6c07,
	0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
	0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba,
	0x8fd9, 0x9ff8, 0x6e17, 0x7e36, 0x4e55, 0x5e74,
	0x2e93, 0x3eb2, 0x0ed1, 0x1ef0

};


/* Instance of the decoder to extract and decode AIS messages from the stream of input data. */
struct decoder dr;


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
	dr.decoding_in_progress = 0;

}


/* Reset the state of the encoder when no message was found and searching for a new message starts */
void prvDecoderReset()
{

	dr.preamble_counter = 0;
	dr.preamble_found = 0;
	dr.startflag_counter = 0;
	dr.startflag_found = 0;
	dr.endflag_counter = 0;
	dr.endflag_found = 0;
	dr.encoded_payload_length = 0;
	dr.decoded_payload_length = 0;
	dr.decoding_in_progress = 0;

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
			dr.decoding_in_progress = 1;												// Make sure to continue decoding this message instead of changing channel for next data
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
			dr.decoding_in_progress = 1;												// Make sure to continue decoding this message instead of changing channel for next data
		}
	}
	else																				// The sample did not fit the preamble pattern
	{
		dr.preamble_found = 0;															// Did not find preamble
		dr.preamble_counter = 0;														// Preamble counter reset
	}
	dr.lastlast_sample = dr.last_sample;												// Update the second to last sample
	dr.last_sample = sample;															// Update the last sample
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
			dr.decoding_in_progress = 0;									// Decoding is no longer in progress for this sample, can start looking from other channels as well
		}
	}
	else if ( dr.startflag_counter < (AIS_START_FLAG_LENGTH - 1) )			// Must check last sample of start flag separately so don't iterate until the last one
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
			dr.decoding_in_progress = 0;									// Decoding is no longer in progress for this sample, can start looking from other channels as well
		}
	}
	else if ( dr.startflag_counter == (AIS_START_FLAG_LENGTH - 1) )			// Check the last sample of the possible start flag
	{
		if ( sample != dr.last_sample )										// Sample fits start flag pattern
		{
			dr.startflag_found = 1;											// Start flag found!
			dr.startflag_counter = 0;										// Start flag counter reset
			dr.startflag_end_value = sample;								// Store the value of the last bit of the start flag so it can be compared with the first bit of the payload
		}
		else																// Sample did not match the start flag pattern
		{
			dr.startflag_counter = 0;										// Sample did not match the start flag pattern
			dr.startflag_found = 0;											// Did not find start flag
			dr.preamble_found = 0;											// Reset also preamble found flag, since we have to start looking for a preamble again.
			dr.decoding_in_progress = 0;									// Decoding is no longer in progress for this sample, can start looking from other channels as well
		}
	}
	else																	// Start flag not found
	{
		dr.startflag_counter = 0;											// Sample did not match the start flag pattern
		dr.startflag_found = 0;												// Did not find start flag
		dr.preamble_found = 0;												// Reset also preamble found flag, since we have to start looking for a preamble again.
		dr.decoding_in_progress = 0;										// Decoding is no longer in progress for this sample, can start looking from other channels as well
	}
	dr.lastlast_sample = dr.last_sample;									// Update the second to last sample
	dr.last_sample = sample;												// Update the last sample
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
				dr.decoding_in_progress = 0;									// Decoding is no longer in progress for this sample, can start looking from other channels as well
			}
		}
		else if ( dr.endflag_counter == (AIS_END_FLAG_LENGTH - 1) )				// Check the last sample of the possible end flag
		{
			if ( sample != dr.last_sample )										// Sample fits end flag pattern
			{
				dr.endflag_counter++;											// Found the last one
				dr.endflag_found = 1;											// End flag found!
				dr.encoded_payload_length -= AIS_END_FLAG_LENGTH;				// Remove the length of the end flag from the length of the encoded payload
			}
			else																// Sample did not match the end flag pattern
			{
				dr.endflag_counter = 0;											// Sample did not match the end flag pattern
				dr.endflag_found = 0;											// Did not find end flag
				dr.decoding_in_progress = 0;									// Decoding is no longer in progress for this sample, can start looking from other channels as well
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
		dr.decoding_in_progress = 0;											// Decoding is no longer in progress for this sample, can start looking from other channels as well
	}
	dr.lastlast_sample = dr.last_sample;										// Update the second to last sample
	dr.last_sample = sample;													// Update the last sample
}


/* Decode and extract AIS message payload and corresponding CRC-16.  */
void prvPayloadAndCRCDecode()
{
	uint32_t consecutive_ones = 0;											// Count how many consecutive ones have been encountered in the data
	uint32_t num_removed_bits = 0;											// How many bits have been removed (destuffed)
	uint32_t decoded_idx = 0;												// Current index of the unstuffed data buffer
	/* Extract and decode data */
	if ( dr.encoded_payload[0] == dr.startflag_end_value )					// Compare the first bit separately with the stored value of the last bit of the start flag
	{
		dr.decoded_payload[decoded_idx] = 1;								// Value same as previous --> found 1
		consecutive_ones++;													// Increment the counter of consecutive ones
	}
	else
	{
		dr.decoded_payload[decoded_idx] = 0;								// Value not same as previous --> found 0
		consecutive_ones = 0;												// Reset the counter of consecutive ones
	}

	for ( uint32_t i=1; i<dr.encoded_payload_length; i++ )					// Extract the whole payload (including CRC)
	{

		if ( dr.encoded_payload[i] == dr.encoded_payload[i-1] )				// It's okay to go one step back from the "first index" when i==payload_start_idx, since we need to compare to the last bit of the start flag.
		{
			dr.decoded_payload[decoded_idx] = 1;							// Value same as previous --> found 1
			consecutive_ones++;												// Increment the counter of consecutive ones
			if (consecutive_ones > 4)
			{
				i++;														// Skip the stuffing 0
				num_removed_bits++;											// Increment the removed bits counter
				consecutive_ones = 0;										// Reset the counter of consecutive ones
				continue;
			}
		}
		else
		{
			dr.decoded_payload[decoded_idx] = 0;							// Value not same as previous --> found 0
			consecutive_ones = 0;											// Reset the counter of consecutive ones
		}
		decoded_idx++;
	}
	dr.decoded_payload_length = dr.encoded_payload_length - num_removed_bits;

	/* Extract and decode CRC-16 */
	memcpy( dr.decoded_crc, (dr.decoded_payload + dr.decoded_payload_length - AIS_CRC_LENGTH), AIS_CRC_LENGTH );	// Copy the CRC-16 bits to its own array
	dr.decoded_payload_length = dr.encoded_payload_length - AIS_CRC_LENGTH;											// Update the payload length now that CRC is not included
}


/* Take the CRC-16 array of 1s and 0s and convert it to actual binary bytes of data */
void prvCRC8Bit()
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


/* Take the payload array of 1s and 0s and convert it to 8 bit ASCII */
void prvPayloadTo8Bit()
{
	/* Check that the number of payload bits is a multiple of 6 (AIS message consists of 6 bit chars) */
	if ( !( dr.decoded_payload_length % 6) )
	{
		Error_Handler();
	}
	dr.ascii_message_length = 0;
	/* Convert digitized payload from array of "bits" to bytes */
	uint8_t byte = 0;
	uint32_t byte_idx = 0;											// Index for copmleted bytes
	uint32_t j;
	for ( uint32_t i=0; i < dr.decoded_payload_length; i+=6 )		// Move always 6 bits forward to jump to the next char
	{
		for( j=i; j<(i+6); ++j )									// Characters in AIS payload are 6 bits long
		{
			if( dr.decoded_payload[j] == 1 ) byte |= 1 << (7-j);	// Bitshift to get the bits in their correct locations TODO: Verify this works correctly, especially for our 6 bit chars
		}
		/* Convert the 6 bit ASCII to 8 bit ASCII (source: http://ldsrc.blogspot.com/2017/09/0-understanding-ais-nmea-0183.html) TODO: Verify this works correctly */
		if ( byte > 32 )
		{
			dr.ascii_message[byte_idx] = byte + 48 + 8;
		}
		else
		{
			dr.ascii_message[byte_idx] = byte + 48;
		}
		byte = 0;													// Reset byte for next one
		byte_idx++;
		dr.ascii_message_length++;									// Count how many bytes (chars) have been found
	}
}


/* Take the payload array of 1s and 0s and convert it so that it can be used to check for CRC */
void prvPayloadTo6Bit()
{
	// TODO: This function needs to be checked for bugs and logical errors in the implementation. Didn't have too much time to think about it - your friendly neighborhood Ville
	/* Check that the number of payload bits is a multiple of 6 (AIS message consists of 6 bit chars) */
	if ( !( dr.decoded_payload_length % 6) )
	{
		Error_Handler();
	}
	dr.ascii_message_length = 0;
	/* Convert digitized payload from array of "bits" to bytes */
	uint8_t byte = 0;
	uint32_t bit_idx = 0;												// Index of bit inside current byte
	uint32_t byte_idx = 0;												// Index for completed bytes
	for ( uint32_t i=0; i < dr.decoded_payload_length; i+=1 )
	{
		if ( bit_idx < 8 )
		{
			if( dr.decoded_payload[i] == 1 ) byte |= 1 << (7-bit_idx);	// Bitshift to get the bits in their correct locations TODO: Verify this works correctly, especially for our 6 bit chars
			i++;
		}
		else
		{
			dr.dense_message[byte_idx] = byte;
			bit_idx = 0;
			byte = 0;
			if( dr.decoded_payload[i] == 1 ) byte |= 1 << (7-bit_idx);	// Bitshift to get the bits in their correct locations TODO: Verify this works correctly, especially for our 6 bit chars
		}
		byte = 0;														// Reset byte for next one
		dr.dense_message_length++;										// Count how many bytes (chars) have been found
	}
	/* Data must be padded to full contain and integer amount of bytes. Shift the last byte so that the (possible) padded zeros are the most significant bits.
	 * This way they should not affect the result of calculating the CRC. Refer to: https://stackoverflow.com/questions/3411654/best-way-to-generate-crc8-16-when-input-is-odd-number-of-bits-not-byte-c-or-p */
	dr.dense_message[dr.dense_message_length - 1] = dr.dense_message[dr.dense_message_length - 1] >> (dr.decoded_payload_length % 8);
}


/* Calculates the CRC-16 for given data of length len. TODO: Check that the this actually works and that the payload given to this is in the right format. */
static uint16_t prvCRC16( const uint8_t* data, uint32_t len )
{
	uint16_t crc = 0x0000;										// Init value for CRC-CCITT (See "CRC-16/KERMIT" at https://reveng.sourceforge.io/crc-catalogue/all.htm)
	while (len-- > 0)
	{
		uint16_t idx = (crc ^ *(data++)) & 0xFF;				// Calculate lookup index
		crc = ((crc >> 8) & 0xFF) ^ crc16_table[idx];			// Lookup from CRC table at calculated index
	}
	return crc;													// No inversion or xor needed for CRC-CCITT TODO: Confirm this if CRC check is not working
}


/* Check if the received CRC equals the one that is counted from the payload data. */
uint8_t prvCheckPayloadCRC()
{
	return ( dr.crc16 == prvCRC16( dr.dense_message, dr.dense_message_length ) );
}


