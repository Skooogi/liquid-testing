/*
 * decoder.c
 *
 * This file contains functions responsible for decoding the received AIS signal.
 *
 */

#include "decoder.h"
#include "main.h"
#include "adc.h"


struct decoder decoder = {

	  .prev_complex = 0,
	  .complex_data = 0,
	  .temp_complex = 0,
	  .temp_I = 0,
	  .temp_Q = 0,
	  .block_size = BLOCK_SIZE,
	  .num_blocks = ADC_RX_BUF_SIZE/BLOCK_SIZE/2

};

/* Demodulates GMSK modulated signal that has been downmixed
 * 	startflag 		= 	signal start in adcX.data
 * 	endflag 		= 	signal end in adcX.data
 * 	demodulated_IQ 	=	array for storing demodulated signal
 * */
void gmsk_demod(int startflag, int endflag, int16_t *demodulated_IQ){

	for(int i = startflag; i < endflag; i++){

		/*temp_I = -(float64_t)(adc1_data[i] - (max_val-min_val)/2) / ((max_val-min_val)/2);// cast uint16 value to float64_t
		temp_Q = -(float64_t)(adc3_data[i] - (max_val-min_val)/2) / ((max_val-min_val)/2);*/

		decoder.temp_I = -(float64_t)(adcI.data_fir[i]) / UINT16_OFFSET;		// cast int16 value to float64_t	TODO: What is done here
		decoder.temp_Q = -(float64_t)(adcQ.data_fir[i]) / UINT16_OFFSET;		// cast int16 value to float64_t	TODO: What is done here

		decoder.complex_data = (decoder.temp_I + (decoder.temp_Q * _Complex_I));
		decoder.temp_complex = decoder.complex_data * conj(decoder.prev_complex); 				// Polar discriminator
		demodulated_IQ[i-startflag] = (int16_t)(SHORT_MAX * (atan2(cimag(decoder.temp_complex), creal(decoder.temp_complex)) / M_PI));
		decoder.prev_complex = decoder.complex_data;

	}
}



