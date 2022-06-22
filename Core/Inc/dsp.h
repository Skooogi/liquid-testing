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
#define AIS_START_END_FLAG_LENGTH		8								// bits
#define AIS_MAX_PAYLOAD_LENGTH			168								// bits

#define DSP_FIR_BUF_SIZE				0x1000							// TODO: Determine value
#define DSP_FIR_BUF_SIZE				0x1000							// TODO: Determine value

#define SNR_THRESHOLD_F32    			75.0f


typedef struct dsp {

	uint8_t processing_request_flag;
	uint32_t batch_sn;

	complex float raw_complex_data[ADC_RX_BUF_SIZE];
	complex float demodulated_IQ[ADC_RX_BUF_SIZE];
	float downmix_freq;

	complex float fft_max_mag;
	uint32_t fft_max_mag_idx;
	float32_t mix_freq;

	complex float fft_buf[FFT_SIZE*2];
	complex float fft_mag_buf[FFT_SIZE*2];

	complex float processed_data[ADC_RX_BUF_SIZE];
	complex float decimated_data[ADC_RX_BUF_SIZE/DECIMATION_FACTOR];
	complex float digitized_data[ADC_RX_BUF_SIZE/DECIMATION_FACTOR];

	uint32_t stuffed_payload_length;
	uint8_t unstuffed_payload[AIS_MAX_PAYLOAD_LENGTH];
	uint32_t unstuffed_payload_length;

	uint8_t	decoded_data[ADC_RX_BUF_SIZE/DECIMATION_FACTOR/6];

	// TODO: Explain all variables.w

} *dsp_t;

/* Declare the extern struct decoder. */
extern struct dsp dsp;

/* Declare DSP Task handle */
extern TaskHandle_t DSPTaskHandle;


/************* Publicly callable functions *************/
void prvDSPInit();
void prvDSPTask( void *pvParameters );		// Task for digital signal processing of the RF signals


#endif /* CORE_INC_DSP_H_ */

