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
#include "liquid.h"

#define BLOCK_SIZE         				64
#define NUM_BLOCKS 						ADC_RX_BUF_SIZE/BLOCK_SIZE/2

#define SAMPLES_PER_SYMBOL				3
#define SYMBOLRATE						9600.0f

#define AIS_PACKAGE_MAX_LENGHT			256								// bits
#define AIS_PREAMBLE_LENGTH				24								// bits
#define AIS_START_END_FLAG_LENGTH		8								// bits
#define AIS_MAX_PAYLOAD_LENGTH			168								// bits

#define DSP_FIR_BUF_SIZE				0x1000							// TODO: Determine value
#define DSP_FIR_BUF_SIZE				0x1000							// TODO: Determine value

#define SNR_THRESHOLD_F32    			75.0f


typedef struct dsp {

	// TODO: Could maybe make own structs for different objects

	uint8_t processing_request_flag;
	uint32_t batch_sn;

	uint32_t frame_counter;

	float downmix_freq;

	/* Arrays to store intermediate results. */
	complex float raw_IQ[ADC_RX_BUF_SIZE];
	complex float filtered_IQ[ADC_RX_BUF_SIZE];
	complex float *resampled_IQ;
	complex float *synced_IQ;
    uint32_t *demod_output;

	/* Resampler (resamp) options (for decimation) */
    resamp_crcf resampler;													// Resample object
    uint32_t resamp_filter_delay;		    								// filter semi-length (filter delay)
    float resamp_rate;               										// resampling rate (output/input)
    float resamp_bw;              											// resampling filter bandwidth
    float resamp_slsl;          											// resampling filter sidelobe suppression level
    uint32_t resamp_npfb;       											// number of filters in bank (timing resolution)
    uint32_t resamp_input_length;											// number of input samples
    uint32_t resamp_output_length;											// Number of output samples
    unsigned int resamp_num_written;   										// number of values written to buffer

	/* Symbol synchronizer (symsync) options */
	symsync_crcf symsyncer;													// Symsync object
    uint32_t symsync_sampersym;    											// samples/symbol
    uint32_t symsync_filter_delay;  	 									// filter delay (symbols)
    float symsync_beta;  													// filter excess bandwidth factor
    uint32_t symsync_npfb;    												// number of polyphase filters in bank
    int32_t symsync_ftype; 													// filter type
    unsigned int symsync_num_written;										// number of values written to buffer

    /* GMSK demodulation (gmskdem) options */
    gmskdem demod;															// GMSK demodulator object
    uint32_t demod_sampersym;    											// filter samples/symbol
    uint32_t demod_filter_delay;    										// filter delay (symbols)
    float demod_BT;    														// bandwidth-time product










	// All below this is the old implementation, stored only for reference in development phase.
	complex float raw_complex_data[ADC_RX_BUF_SIZE];
	complex float demodulated_IQ[ADC_RX_BUF_SIZE];

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

	// TODO: Explain all variables.


} *dsp_t;

/* Declare the extern struct decoder. */
extern struct dsp dsp;

/* Declare DSP Task handle */
extern TaskHandle_t DSPTaskHandle;


/************* Publicly callable functions *************/
void prvDSPInit();
void prvDSPTask( void *pvParameters );		// Task for digital signal processing of the RF signals


#endif /* CORE_INC_DSP_H_ */

