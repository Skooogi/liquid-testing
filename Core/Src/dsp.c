/*
 * dsp.c
 *
 * This file contains the digital signal processing functionality of the payload.
 *
 */

#include "dsp.h"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "filter.h"
#include <math.h>


struct dsp dsp = {

	.processing_request_flag = 0,
	.batch_sn = 0,
	.downmix_freq = 0,

};


void prvDSPInit()
{
	memset( dsp.fft_buf, 0, 2*FFT_SIZE*sizeof(q31_t) );
	memset( dsp.fft_mag_buf, 0, 2*FFT_SIZE*sizeof(q31_t) );
}


/* Function to remove the mean value of the elements of an array from each of its elements */
static void prvSubtractMean(int16_t *data, uint32_t data_length)
{
	q15_t mean;
	arm_mean_q15( data, ADC_RX_BUF_SIZE, &mean );

	for(uint32_t i=0; i< ADC_RX_BUF_SIZE; i++) {
		*(data + i) -= mean;
	}

}


/* Demodulates GMSK modulated signal that has been downmixed      (quadrature fm demodulation)
 * 	startflag 		= 	signal start in adcX.data
 * 	endflag 		= 	signal end in adcX.data
 * 	demodulated_IQ 	=	array for storing demodulated signal */
void prvGMSKDemodulate(uint32_t startflag,  uint32_t endflag, int16_t *demodulated_IQ)
{
	/* Temporary assisting variables */
	float64_t temp_I;
	float64_t temp_Q;
	complex complex_data;
	complex prev_complex_data;
	complex temp_complex_data;
	/* Demodulation */
	for(uint32_t i = startflag; i < endflag; i++)
	{
		temp_I = -(float64_t)(adcI.data_fir[i]) / UINT16_OFFSET;		// cast int16 value to float64_t	TODO: What is done here
		temp_Q = -(float64_t)(adcQ.data_fir[i]) / UINT16_OFFSET;		// cast int16 value to float64_t	TODO: What is done here

		complex_data = (temp_I + (temp_Q * _Complex_I));
		temp_complex_data = complex_data * conj(prev_complex_data); 		// Polar discriminator
		demodulated_IQ[i-startflag] = (int16_t)(SHORT_MAX * (atan2(cimag(temp_complex_data), creal(temp_complex_data)) / M_PI));
		prev_complex_data = complex_data;
	}
}


/* Detect AIS message preamble */
static uint32_t prvDetectPreamble(uint8_t *data)
{
	uint32_t preamble_found = 1;

	if ( data[0] == data[1] )										// Check if first "bits" are equal
	{
		for ( uint32_t i=2; i<AIS_PREAMBLE_LENGTH; i+=2 )
		{
			if ( data[i] != data[i-2] && data[i] == data[i+1] )	// Check that the current "bit" is equal to the next but different from the (two) previous
			{
				continue;
			}
			else												// The "bits" did not represent a preamble, break out of loop and return 0.
			{
				preamble_found = 0;
				break;
			}
		}
	}
	else
	{
		preamble_found = 0;
	}
	return preamble_found;
}


/* Detect AIS message start or end flag */
static uint32_t prvDetectStartOrEndFlag(uint8_t *data)
{
	uint32_t flag_found = 1;
	uint32_t i;

	for ( i=1; i<AIS_START_END_FLAG_LENGTH-1; i++ )		// Must check the last bit separately as it should be different
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


/* Destuff AIS message payload */
static void prvPayloadDestuff(uint8_t *digitized_data, uint32_t payload_start_idx)
{
	uint32_t consecutive_ones = 0;		// Count how many consecutive ones have been encountered in the data
	uint32_t num_removed_bits = 0;		// How many bits have been removed (destuffed)
	uint32_t unstuffed_idx = 0;			// Current index of the unstuffed data buffer
	for ( uint32_t i=payload_start_idx; i < payload_start_idx + dsp.stuffed_payload_length; i++ )
	{
		if ( dsp.digitized_data[i] == 1 )
		{
			consecutive_ones++;
			if (consecutive_ones > 4)
			{
				i++;					// Skip the stuffing 0
				num_removed_bits++;		// Increment the removed bits counter
				consecutive_ones = 0;	// Reset the counter of consecutive ones
			}
		}
		else
		{
			consecutive_ones = 0;
		}
		dsp.unstuffed_payload[unstuffed_idx] = dsp.digitized_data[i];
		unstuffed_idx++;
	}
	dsp.unstuffed_payload_length = dsp.stuffed_payload_length - num_removed_bits;
}


/* Take an array of 1s and 0s and convert it to actual binary bytes of data */
static void prvBitArrayToBinary()
{
	/* Convert digitized data from array of "bits" to bytes */
	uint8_t byte = 0;
	uint8_t j;
	for ( uint32_t i=0; i < AIS_MAX_PAYLOAD_LENGTH; i+=6 )
	{
		for( j = i; j < i+6; ++j )								// Characters in AIS payload are 6 bits long
		{
			if( dsp.unstuffed_payload[j] == 1 ) byte |= 1 << (7-j);
		}
		dsp.decoded_data[j] = byte;
		byte = 0;
	}
}


/* Function taking care of the complete DSP pipeline from raw data to a decoded signal */
static void prvDSPPipeline()
{

	/* Remove DC spike from the data. */
	prvSubtractMean( adcI.data, ADC_RX_BUF_SIZE );
	prvSubtractMean( adcQ.data, ADC_RX_BUF_SIZE );

	/* Save data to fft_buf, use 32 bit values shifted left 16 bits because that helps. */
	for(uint32_t i=0; i<ADC_RX_BUF_SIZE; i+=FFT_SIZE*2)
	{
		for(uint16_t j=0; j < FFT_SIZE*2; j+=2)		// Interleaving
		{
			dsp.fft_buf[j] = (adcI.data[i+j]<<16);
			dsp.fft_buf[j+1] = (adcQ.data[i+j+1]<<16);
		}
	}

	/* Perform FFT */
	arm_cfft_q31(&arm_cfft_sR_q31_len2048, dsp.fft_buf, dsp.ifft_flag, dsp.bit_reverse_flag);
	/* Calculate the complex magnitude of the FFT. */
	arm_cmplx_mag_q31(dsp.fft_buf, dsp.fft_mag_buf, FFT_SIZE);
	/* Find max magnitude and index of said max magnitude */
	arm_max_q31(dsp.fft_mag_buf, FFT_SIZE, &(dsp.fft_max_mag), &(dsp.fft_max_mag_idx));

	/* Calculate peak frequency */
	if ( dsp.fft_max_mag_idx > FFT_SIZE/2 )		// On right plane --> get negative value
	{
		dsp.mix_freq = (int32_t)(dsp.fft_max_mag_idx * ((ADC_SAMPLERATE/2) / FFT_SIZE) % (ADC_SAMPLERATE/2)) - ADC_SAMPLERATE/2;
	}
	else  										// On left plane --> get positive value
	{
		dsp.mix_freq = dsp.fft_max_mag_idx * (ADC_SAMPLERATE/2) / FFT_SIZE;
	}

	/* Peak expected around 25 kHz, check if mix_freq has valid value */
	if( (dsp.mix_freq > 30000 && dsp.mix_freq < 20000) || (dsp.mix_freq < -30000 && dsp.mix_freq > -20000) )	// mix_frequency far off, not correct one
	{
		// TODO: Do some proper error handling if mix_freq is invalid.
		dsp.mix_freq = 0;
		memset(dsp.fft_mag_buf, 0, 2*FFT_SIZE*sizeof(q31_t));
	}

	/* Downmixing */
	if(dsp.mix_freq != 0)
	{
		for(uint16_t i = 0; i<ADC_RX_BUF_SIZE; i++)
		{
			  dsp.radians = 2 * M_PI * i * dsp.mix_freq / ((float32_t)ADC_SAMPLERATE);
			  // TODO: cosine for I data
			  dsp.sine_value = arm_sin_f32(dsp.radians); 								// Move to positive values only TODO: What does this comment mean?
			  adcI.data[i] =(adcI.data[i])*dsp.sine_value;	// TODO: float
			  adcQ.data[i] =(adcQ.data[i])*dsp.sine_value;	// TODO: float
		}
	}

	/* Lowpass filter both I and Q data */
	for(uint32_t i=0; i < NUM_BLOCKS; i++)
	{
		arm_fir_q15(&(filters.fir1), adcI.data+i*BLOCK_SIZE, adcI.data_fir+i*BLOCK_SIZE, BLOCK_SIZE);
	}
	for(int i=0; i < NUM_BLOCKS; i++)
	{
		arm_fir_q15(&(filters.fir1), adcQ.data+i*BLOCK_SIZE, adcQ.data_fir+i*BLOCK_SIZE, BLOCK_SIZE);
	}

	/* Demodulate I and Q signals, save result to demodulated_IQ */
	prvGMSKDemodulate(0, ADC_RX_BUF_SIZE, dsp.demodulated_IQ);

	/* TODO: Implement clock recovery Müller Müller */

	/* Filter the demodulated data with a 0.1 lowpass filter */
	for(uint32_t i=0; i < NUM_BLOCKS; i++)
	{
		arm_fir_q15(&(filters.fir2), dsp.demodulated_IQ+i*BLOCK_SIZE, dsp.processed_data+i*BLOCK_SIZE, BLOCK_SIZE);
	}


	/* Decimate the data */
	for(uint32_t i = 0; i<ADC_RX_BUF_SIZE/DECIMATION_FACTOR; i++)
	{
		  dsp.decimated_data[i] = dsp.processed_data[i*DECIMATION_FACTOR];
	}

	/* Digitize the data */
	prvSubtractMean(dsp.decimated_data, ADC_RX_BUF_SIZE/DECIMATION_FACTOR);		// Subtract the mean from the data.
	for (uint32_t i=0; i<ADC_RX_BUF_SIZE/DECIMATION_FACTOR; i++)
	{
		if (dsp.decimated_data[i] >= 0)											// "Bit" is 1 if datapoint >= 0.
		{
			dsp.digitized_data[i] = 1;
		}
		else																	// "Bit" is 0 if datapoint < 0.
		{
			dsp.digitized_data[i] = 0;
		}
	}

	/* Look for preamble, start flag, and end flag. Determine payload length and destuff payload data. */
	for (uint32_t i=0; i<ADC_RX_BUF_SIZE/DECIMATION_FACTOR - AIS_PACKAGE_MAX_LENGHT; i++)
	{
		/* If true, then preamble found */
		if ( prvDetectPreamble(dsp.digitized_data + i) )
		{
			/* If true, then start flag found */
			if ( prvDetectStartOrEndFlag(dsp.digitized_data + i + AIS_PREAMBLE_LENGTH) )
			{
				/*  */
				for ( dsp.stuffed_payload_length=0; dsp.stuffed_payload_length<=AIS_MAX_PAYLOAD_LENGTH; dsp.stuffed_payload_length++ )
				{
					/* If true, end flag detected, stop counting */
					if ( prvDetectStartOrEndFlag(dsp.digitized_data + i + AIS_PREAMBLE_LENGTH + AIS_START_END_FLAG_LENGTH) )
					{
						break;
					}
				}
				prvPayloadDestuff(dsp.digitized_data, i + AIS_PREAMBLE_LENGTH + AIS_START_END_FLAG_LENGTH);
			}
		}
	}

	// TODO: Buffer sizes so that they are compatible with decimation and still amount to integers and important data is not lost. Maybe datarate?

	// TODO: Start flag detection and AIS message extraction.

	// TODO: Push data to result buffer.

}


/* DSP Task responsible for the complete DSP pipeline */
void prvDSPTask( void *pvParameters )
{

	/* Remove compiler warning about unused parameter. */
	( void ) pvParameters;

	for( ;; )
	{
		/* The ADCTask gives a notification when data requires processing. This task remains blocked until
		 * a notification is received. Upon receiving the notification, clear the notification and start DSP. */
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		if (!dsp.processing_request_flag)	// Check if processing is requested.
		{
			continue;	// No data to be processed.
		}
		else
		{
			/* Processing requested, reset request flag. */
			dsp.processing_request_flag = 0;

			/* Check if the data requiring processing is in the A or B buffers of the double buffer system. */
			if (adcI.A_full && adcQ.A_full)
			{
				for (int i=0;i<ADC_DBUF_LEN;i++)
				{
					/* Copy data from the double buffer to the actual data buffers of both ADC instances. */
					memcpy( adcI.data, adcI.buf_A + i*ADC_RX_BUF_SIZE, ADC_RX_BUF_SIZE );
					memcpy( adcQ.data, adcQ.buf_A + i*ADC_RX_BUF_SIZE, ADC_RX_BUF_SIZE );
					/* Do DSP */
					prvDSPPipeline();
					dsp.batch_sn++;
				}
				adcI.A_full = 0;
				adcQ.A_full = 0;
			}
			else if (adcI.B_full && adcQ.B_full)
			{
				for (int i=0;i<ADC_DBUF_LEN;i++)
				{
					/* Copy data from the double buffer to the actual data buffers of both ADC instances. */
					memcpy( adcI.data, adcI.buf_B + i*ADC_RX_BUF_SIZE, ADC_RX_BUF_SIZE );
					memcpy( adcI.data, adcI.buf_B + i*ADC_RX_BUF_SIZE, ADC_RX_BUF_SIZE );
					/* Do DSP */
					prvDSPPipeline();
					dsp.batch_sn++;
				}
				adcI.B_full = 0;
				adcQ.B_full = 0;
			}
			else
			{
				// TODO: Can be removed once no longer needed, only for debugging.
				/* Ends up here if processing is requested when there is no data ready for processing */
				dsp.dbuf_false_processing_request_error = 1;
			}

		}

	}

}



