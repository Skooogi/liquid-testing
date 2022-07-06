/*
 * decoder.h
 *
 */

#include <stdint.h>


#ifndef CORE_INC_DECODER_H_
#define CORE_INC_DECODER_H_

#define AIS_MAX_PACKAGE_LENGTH			256							// bits
#define AIS_RAMP_UP_LENGHT				8							// bits
#define AIS_PREAMBLE_LENGTH				24							// bits
#define AIS_START_END_FLAG_LENGTH		8							// bits
#define AIS_START_FLAG_LENGTH			8							// bits
#define AIS_END_FLAG_LENGTH				8							// bits
#define AIS_MAX_PAYLOAD_BITS			168							// bits
#define AIS_CRC_LENGTH					16							// bits
#define AIS_MAX_ENCODED_PAYLOAD_LENGTH	AIS_MAX_PACKAGE_LENGTH - AIS_RAMP_UP_LENGHT - AIS_PREAMBLE_LENGTH - AIS_START_FLAG_LENGTH - AIS_END_FLAG_LENGTH	// bits
#define AIS_BITS_PER_CHAR				6



/* Struct for storing all decoding related options, variables and data. */
typedef struct decoder {

	uint32_t last_sample;																// Store the last sample (i-1) value for comparison purposes
	uint32_t lastlast_sample;															// Store the sample before the last sample (i-2) for comparison purposes

	uint32_t preamble_counter;															// Count how many samples fitting the preamble pattern have been found
	uint32_t preamble_found;															// Detected preamble

	uint32_t startflag_counter;															// Count how many samples fitting the start flag pattern have been found
	uint32_t startflag_found;															// Detected start flag

	uint32_t endflag_counter;															// Count how many samples fitting the end flag pattern have been found
	uint32_t endflag_found;																// Detected end

	uint32_t encoded_payload_length;													// Length of the encoded payload (contains CRC-16)
	uint32_t encoded_payload[AIS_MAX_ENCODED_PAYLOAD_LENGTH];							// Array to store the encoded payload and CRC for decoding

	uint8_t decoded_payload[AIS_MAX_PAYLOAD_BITS];										// Array for storing the encoded payload data (1s and 0s)
	uint32_t decoded_payload_length;													// Length of the decoded payload;
	uint8_t decoded_crc[AIS_CRC_LENGTH];

	uint8_t ascii_message[AIS_MAX_PAYLOAD_BITS/AIS_BITS_PER_CHAR];						// Array for storing the final decode message (in 8 bit ASCII)
	uint32_t ascii_message_length;														// Length of the ASCII message in bytes.
	uint16_t crc16;																		// Decoded CRC-16

	uint32_t decoding_in_progress;														// Flag is set if promising decoding is in progress (e.g. preamble already found, don't change channels when set)

} *decoder_t;

/* Declare the extern struct dsp. */
extern struct decoder dr;


/************* Publicly callable functions *************/
void prvDecoderInit();
void prvDecoderReset();
void prvDetectPreamble( unsigned int sample );
void prvDetectStartFlag( unsigned int sample );
void prvDetectEndFlag( unsigned int sample );
void prvPayloadAndCRCDecode();
void prvPayloadToBytes();
void prvCRCToBytes();
uint32_t prvCheckPayloadCRC();


#endif /* CORE_INC_DECODER_H_ */
