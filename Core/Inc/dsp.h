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

#define AIS_PACKAGE_MAX_LENGHT			256							// bits
#define AIS_PREAMBLE_LENGTH				24							// bits
#define AIS_START_END_FLAG_LENGTH		8							// bits
#define AIS_MAX_PAYLOAD_LENGTH			168							// bits

#define SNR_THRESHOLD_F32    			75.0f


/* A struct to store filter (firfilt) object options. */
struct filter {
    firfilt_crcf filter;											// Filter object
    float fc;        												// Filter cutoff frequency
    float ft;         												// Filter transition
    float attenuation;      										// Stop-band attenuation [dB]
    float mu;         												// Fractional timing offset
    uint32_t num_taps;												// Estimated filter length
    float *taps;													// Filter coefficients
	complex float *input, *output;									// Input, output of filter (probably not needed)
};

/* A struct to store resampler (resamp) object options (for decimation). */
struct resamp {
    resamp_crcf resampler;											// Resample object
    uint32_t filter_delay;		    								// filter semi-length (filter delay)
    float rate;               										// resampling rate (output/input)
    float bw;              											// resampling filter bandwidth
    float slsl;          											// resampling filter sidelobe suppression level
    uint32_t npfb;       											// number of filters in bank (timing resolution)
    uint32_t input_length; 											// number of input samples TODO: define beforehand so no need for malloc later
    uint32_t output_length;											// Number of output samples
    unsigned int num_written;   									// number of values written to buffer
};

/* A struct to store symbol synchronizer (symsync) object options . */
struct symsync {
	symsync_crcf symsyncer;											// Symsync object
    uint32_t sampersym;    											// samples/symbol
    uint32_t filter_delay;  	 									// filter delay (symbols)
    float beta;  													// filter excess bandwidth factor
    uint32_t npfb;    												// number of polyphase filters in bank
    int32_t ftype; 													// filter type
    unsigned int num_written;										// number of values written to buffer
};

/* A struct to store symbol synchronizer (symsync) object options . */
struct demod {
    gmskdem demod;													// GMSK demodulator object
    uint32_t sampersym;    											// filter samples/symbol
    uint32_t filter_delay;    										// filter delay (symbols)
    float BT;    													// bandwidth-time product
};

/* Struct for storing all DSP related options, variables and data. */
typedef struct dsp {

	/* General variables */
	uint8_t processing_request_flag;
	uint32_t batch_sn;
	uint32_t frame_counter;
	float downmix_freq;

	/* Arrays to store intermediate results. */
	complex float raw_IQ[ADC_RX_BUF_SIZE];
	complex float filtered_IQ[ADC_RX_BUF_SIZE];
	complex float *resampled_IQ;
	complex float *synced_IQ;
    unsigned int *demod_output;

    /* Structs for storing options of the liquid-dsp objects used. */
    struct filter fr;												// Filter object options
    struct resamp rs;												// Resampler object options
    struct symsync ss;												// Symbol synchronizer object options
    struct demod dm;												// GMSK demodulator object options

    //
    //
    //
    //
    //

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

	//
	//
	//
	//
	//

} *dsp_t;

/* Declare the extern struct dsp. */
extern struct dsp dsp;

/* Declare DSP Task handle */
//extern TaskHandle_t DSPTaskHandle;


/************* Publicly callable functions *************/
void prvDSPInit();
void prvDSPTask( void *pvParameters );		// Task for digital signal processing of the RF signals


#endif /* CORE_INC_DSP_H_ */

