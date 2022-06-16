/*
 * dsp.h
 *
 */

#ifndef CORE_INC_DSP_H_
#define CORE_INC_DSP_H_

#include <stdint.h>
#include "adc.h"
#include "math.h"
#include "complex.h"
#include "arm_math.h"
#include "arm_const_structs.h"
#include "FreeRTOS.h"
#include "task.h"

#define BLOCK_SIZE         				64
#define NUM_BLOCKS 						ADC_RX_BUF_SIZE/BLOCK_SIZE/2

#define AIS_PACKAGE_MAX_LENGHT			256								// bits
#define AIS_PREAMBLE_LENGTH				24								// bits
#define AIS_START_OR_END_FLAG_LENGTH	8								// bits

#define SNR_THRESHOLD_F32    			75.0f


typedef struct dsp {

	uint8_t processing_request_flag;
	uint8_t dbuf_false_processing_request_error;	// Error flag to signal if data processing has been requested without ready data (debugging)
	uint32_t batch_sn;
	complex complex_data;
	complex prev_complex;
	complex temp_complex;
	int16_t demodulated_IQ[ADC_RX_BUF_SIZE];
	float64_t temp_I;
	float64_t temp_Q;
	int32_t downmix_freq;

	q31_t fft_max_mag;
	uint32_t fft_max_mag_idx;
	uint32_t mix_freq;
	float32_t radians;
	float32_t sine_value;

	q31_t fft_buf[FFT_SIZE*2];
	q31_t fft_mag_buf[FFT_SIZE*2];
	uint8_t	ifft_flag;					// Perform IFFT? (Regular FFT if 0)
	uint8_t bit_reverse_flag;

	uint32_t prim;

	int16_t processed_data[ADC_RX_BUF_SIZE];
	int16_t decimated_data[ADC_RX_BUF_SIZE/DECIMATION_FACTOR];
	uint8_t digitized_data[ADC_RX_BUF_SIZE/DECIMATION_FACTOR];
	uint8_t	final_data[ADC_RX_BUF_SIZE/DECIMATION_FACTOR/8]

	// TODO: Explain all variables. Decoder heavily depends on the filter implementation that can be found in filter.c and filter.h

} *dsp_t;

/* Declare the extern struct decoder. */
extern struct dsp dsp;

/* Declare DSP Task handle */
extern TaskHandle_t DSPTaskHandle;


/************* Publicly callable functions *************/
void prvDSPInit();
void prvDMSKDemodulate();
void prvDSPTask( void *pvParameters );		// Task for digital signal processing of the RF signals


#endif /* CORE_INC_DSP_H_ */

