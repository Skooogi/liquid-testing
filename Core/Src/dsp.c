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
#include <math.h>
#include <stdlib.h>


struct dsp dsp = {

	.processing_request_flag = 0,
	.batch_sn = 0,
	.mix_freq = 0,

};


void prvDSPInit()
{

    /* filter init */
    dsp.fr.fc = 1.0*(ADC_SAMPLERATE/2.0)/(1.0*SYMBOLRATE * SAMPLES_PER_SYMBOL);			         										// Filter cutoff frequency
    dsp.fr.ft = 0.05f;         																											// Filter transition
    dsp.fr.attenuation = 60.0f;   																										// Stop-band attenuation [dB]
    dsp.fr.mu = 0.0f;          																											// Fractional timing offset
    dsp.fr.num_taps = estimate_req_filter_len( dsp.fr.ft, dsp.fr.attenuation );															// Estimate filter length
    dsp.fr.taps = malloc( dsp.fr.num_taps * sizeof(float) );																			// Remember to free at some point
    liquid_firdes_kaiser( dsp.fr.num_taps, dsp.fr.fc, dsp.fr.attenuation, dsp.fr.mu, dsp.fr.taps );										// Create filter taps and store them and their length
    dsp.fr.filter = firfilt_crcf_create( dsp.fr.taps, dsp.fr.num_taps );																// Create filter object

    /* resamp init */
    dsp.rs.filter_delay = 13;    																										// filter semi-length (filter delay)
    dsp.rs.rate = 0.9f;               																									// resampling rate (output/input)
    dsp.rs.bw = 0.5f;              																										// resampling filter bandwidth
    dsp.rs.slsl = -60.0f;          																										// resampling filter sidelobe suppression level
    dsp.rs.npfb = 32;       																											// number of filters in bank (timing resolution)
    dsp.rs.input_length = ADC_RX_BUF_SIZE + dsp.rs.filter_delay;																		// total length of resampler input
    dsp.rs.output_length = ceilf(dsp.rs.rate * dsp.rs.input_length);																	// resampler output length ("zero padded" to incorporate filter delay)
    dsp.resampled_IQ = malloc( dsp.rs.output_length * sizeof(complex float) );															// Allocate memory for the resampled
    dsp.rs.num_written = 0;   																											// number of values written to buffer
    dsp.rs.resampler = resamp_crcf_create( dsp.rs.rate, dsp.rs.filter_delay, dsp.rs.bw, dsp.rs.slsl, dsp.rs.npfb );						// Create resampler

	/* symsync init */
    dsp.ss.sampersym = SAMPLES_PER_SYMBOL;     																							// samples/symbol
    dsp.ss.filter_delay = 3;     																										// filter delay (symbols)
    dsp.ss.beta = 0.3f;  																												// filter excess bandwidth factor
    dsp.ss.npfb  = 32;    																												// number of polyphase filters in bank
    dsp.ss.ftype = LIQUID_FIRFILT_GMSKRX; 																								// filter type
    dsp.synced_IQ = malloc( dsp.rs.output_length * sizeof(complex float) );																// Same length as dsp.resampled_IQ
    dsp.ss.num_written = 0;   																											// number of values written to buffer
    dsp.ss.symsyncer = symsync_crcf_create_rnyquist( dsp.ss.ftype, dsp.ss.sampersym, dsp.ss.filter_delay, dsp.ss.beta, dsp.ss.npfb );	// Create symbol synchronizer

    /* demod init */
    dsp.dm.sampersym = SAMPLES_PER_SYMBOL;    																							// filter samples/symbol
    dsp.dm.filter_delay = dsp.ss.filter_delay;    																						// filter delay (symbols)
    dsp.dm.BT = 0.25f;    																												// bandwidth-time product
    dsp.dm.output_length = ceil(dsp.rs.output_length/(float)SAMPLES_PER_SYMBOL) + AIS_MAX_PAYLOAD_BITS;									// The output length is
    dsp.demodulated_data = malloc( dsp.dm.output_length * sizeof(unsigned int) );														// Demodulation output is stored with offset of maximum AIS message lenght
    dsp.dm.demod = gmskdem_create( dsp.dm.sampersym, dsp.dm.filter_delay, dsp.dm.BT );													// Create demod object

    /* decoder init (decoder is not a liquid-dsp object) */
    prvDecoderInit();

}


/* Function to remove the mean value of the elements of an array from each of its elements */
static void prvSubtractMean(complex float *data, uint32_t data_length)
{

	float real_mean;
	float imag_mean;
	for (uint32_t i=0; i<ADC_RX_BUF_SIZE; i++)
	{
		real_mean = crealf(data[i]);
		imag_mean = cimagf(data[i]);

	}

	for(uint32_t i=0; i< ADC_RX_BUF_SIZE; i++) {
		data[i] -= (real_mean + imag_mean*I);						// subtract mean from each element in array
	}

}


/* Function taking care of the complete DSP pipeline from raw data to a decoded signal
 *
 * The pipeline has a following outline (TODO: Confirm and finalize the outline):
 *
 * 1. 	Interleave I & Q data		--------- I.e. combine the I and Q signals to one single complex IQ array.
 * 2.	Remove DC offset			--------- Subtract the mean from the signal.
 * 3.	Detect peak frequency		--------- Detect whether data contains signal @ +25 kHz or -25 kHz TODO: Currently no estimation, only predefined channel choice.
 * 4.	Downmix						--------- +25 kHz or -25 kHz to 0 Hz.
 * 5.	Lowpass filter				--------- Use firfilt object (liquid-dsp). Filter out everything except signal of interest.
 * 6.	Decimation					--------- Use resamp object (liquid-dsp) decimate excessive samples.
 * 7. 	Matched filter				----,
 * 8. 	Coarse freq sync				|---- Use symsync object (liquid-dsp).
 * 9. 	Time sync						|
 * 10. 	Fine freq sync				----'
 * 11. 	Demodulation				--------- Use gmksdem object (liquid-dsp). Converts the complex signal to a real signal of 0s and 1s.
 * 12. 	Frame detect				--------- Detect preamble, start flag, and end flag. Extract data packet from between the start and end flags.
 * 13.	Extract/decode packet		--------- Extract and destuff packet. Pack array of "bits" to actual binary (bytes).
 * 14. 	--> Output					--------- Push result to output buffer.
 */
static void prvDSPPipeline()
{

	/* Interleave the I and Q signals to one single complex IQ array. */
	for(uint32_t i=0; i<ADC_RX_BUF_SIZE; i++)
	{
		// ADC dualmode sampling stores both I and Q samples in same buffer as single 32 bit value (2*16 bits) so they have to be separated
		int16_t I_value = adcIQ.data[i] & 0x00FF;					// TODO: Check that the correct bytes are taken (endianness etc.)
		int16_t Q_value = (adcIQ.data[i] >> 16) & 0x00FF;			// TODO: Check that the correct bytes are taken (endianness etc.)
		dsp.raw_IQ[i] = I_value + Q_value*I;
	}

	/* Remove DC spike from the data. */
	prvSubtractMean( dsp.raw_IQ, ADC_RX_BUF_SIZE );


	/* Assume channel. TODO: Use fft to find on which channel there is a signal (or is there any signal on the right channels at all) */
#define A 1		// Is the signal on the A channel? (NOTE: This can be choosed for debugging)
#if A
	dsp.mix_freq = -25e3;		// Amount to shift by
#else
	dsp.mix_freq = +25e3;		// Amount to shift by
#endif

	/* TODO: Perform FFT */
	//arm_cfft_q31(&arm_cfft_sR_q31_len2048, dsp.fft_buf, dsp.ifft_flag, dsp.bit_reverse_flag);
	/* Calculate the complex magnitude of the FFT. */
	//arm_cmplx_mag_q31(dsp.fft_buf, dsp.fft_mag_buf, FFT_SIZE);
	/* Find max magnitude and index of said max magnitude */
	//arm_max_q31(dsp.fft_mag_buf, FFT_SIZE, &(dsp.fft_max_mag), &(dsp.fft_max_mag_idx));

	/* Calculate peak frequency */
	/*if ( dsp.fft_max_mag_idx > FFT_SIZE/2 )		// On right plane --> get negative value
	{
		dsp.mix_freq = (int32_t)(dsp.fft_max_mag_idx * ((ADC_SAMPLERATE/2) / FFT_SIZE) % (ADC_SAMPLERATE/2)) - ADC_SAMPLERATE/2;
	}
	else  											// On left plane --> get positive value
	{
		dsp.mix_freq = dsp.fft_max_mag_idx * (ADC_SAMPLERATE/2) / FFT_SIZE;
	}*/

	/* Peak expected around 25 kHz, check if mix_freq has valid value */
	/*if( (dsp.mix_freq > 30000 && dsp.mix_freq < 20000) || (dsp.mix_freq < -30000 && dsp.mix_freq > -20000) )	// mix_frequency far off, not correct one
	{
		dsp.mix_freq = 0;
		memset(dsp.fft_mag_buf, 0, 2*FFT_SIZE*sizeof(q31_t));
	}*/


	/* Downmix from +25 kHz or -25 kHz to 0 Hz. */
	if(dsp.mix_freq != 0)
	{
		float t;
		for( uint16_t i = 0; i<ADC_RX_BUF_SIZE; i++ )
		{
			t = (1.0*i) / (1.0*ADC_SAMPLERATE);														// Time of sample
			dsp.raw_IQ[i] = (complex float) (dsp.raw_IQ[i] * cexp(2*I*M_PI*dsp.mix_freq*t));		// Downmix
		}
	}


	/* Lowpass filter */
    for ( uint32_t i=0; i<ADC_RX_BUF_SIZE; i++ )
    {
        firfilt_crcf_push( dsp.fr.filter, dsp.raw_IQ[i] );    										// Push filter input sample to the internal buffer of the filter
        firfilt_crcf_execute( dsp.fr.filter, &(dsp.filtered_IQ[i]) ); 								// Compute output
    }


    /* Resample (decimate) */
    resamp_crcf_execute_block( dsp.rs.resampler, dsp.filtered_IQ, dsp.rs.input_length, dsp.resampled_IQ, &(dsp.rs.num_written) );  	// Execute resampler


    /* Time and frequency synchronization */
    symsync_crcf_execute( dsp.ss.symsyncer, dsp.resampled_IQ, dsp.rs.output_length, dsp.synced_IQ, &(dsp.ss.num_written) );


    /* Demodulation */
    uint32_t total_samples = dsp.dm.sampersym * dsp.rs.output_length;
    for ( uint32_t i; i<total_samples; i++ )
    {
    	gmskdem_demodulate( dsp.dm.demod, dsp.synced_IQ, &(dsp.demodulated_data[i + AIS_MAX_PACKAGE_LENGTH*dsp.dm.sampersym]) );
    }


    /* Frame detection: Look for preamble, start flag, and end flag. Determine payload length and destuff payload data. */
	for ( uint32_t i=0; i<dsp.dm.output_length; i++ )
	{
		if ( !dsp.dr.preamble_found )						// If preamble is not found, look for that
		{
			prvDetectPreamble( dsp.demodulated_data[i] );
		}
		else if ( !dsp.dr.startflag_found )					// If preamble is found but the start flag is not found, look for that
		{
			prvDetectStartFlag( dsp.demodulated_data[i] );
		}
		else if ( !dsp.dr.endflag_found )					// If preamble and start flag are found but the end flag is not found, look for that
		{
			prvDetectEndFlag( dsp.demodulated_data[i] );
		}
		else if ( dsp.dr.endflag_found )					// If preamble, start flag, and end flag are found, decode the acquired payload
		{
			prvPayloadDecode( dsp.dr.encoded_payload );
		}

		/* If true, then preamble found */
		//if ( prvDetectPreamble(dsp.demodulated_data[i]) )
		//{
			/* If true, then start flag found */
			//if ( prvDetectStartOrEndFlag(dsp.demodulated_data + i + AIS_PREAMBLE_LENGTH) )
			//{
				/* Iterate over the possible payload and look for end flag */
				//for ( dsp.dr.encoded_payload_length=0; dsp.dr.encoded_payload_length<=AIS_MAX_PAYLOAD_BITS; dsp.dr.encoded_payload_length++ )
				//{
					/* If true, end flag detected, stop counting */
					//if ( prvDetectStartOrEndFlag(dsp.demodulated_data + i + AIS_PREAMBLE_LENGTH + AIS_START_END_FLAG_LENGTH) )
					//{
					//	break;
					//}
				//}
				/* Decode the payload */
				//prvPayloadDecode( dsp.demodulated_data, i + AIS_PREAMBLE_LENGTH + AIS_START_END_FLAG_LENGTH );

				/* Bitshift the 1s and 0s to bytes */
				//prvBitArrayToBinary( dsp.dr.decoded_payload_length );
			//}
		//}
	}
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

			/* Do DSP */
			prvDSPPipeline();
			dsp.batch_sn++;
		}

	}

}



