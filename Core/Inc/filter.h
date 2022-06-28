/*
 * filter.h
 *
 */

#ifndef INC_FILTER_H_
#define INC_FILTER_H_


#include "arm_math.h"
#include "dsp.h"


typedef struct filter {

    float fc;        										// Filter cutoff frequency
    float ft;         										// Filter transition
    float attenuation;      								// Stop-band attenuation [dB]
    float mu;         										// Fractional timing offset
    uint32_t num_taps;										// Estimated filter length
    float *taps;											// Filter coefficients

    firfilt_crcf filter;									// Filter object

	complex float *input, *output;							// Input, output of filter (probably not needed)


/*
	q15_t fir1_coeffs[TAPS_ARRAY_SIZE];
	q15_t fir2_coeffs[TAPS_ARRAY_SIZE];

	q15_t fir1_state[BLOCK_SIZE + NUM_TAPS];
	q15_t fir2_state[BLOCK_SIZE + NUM_TAPS];

	arm_fir_instance_q15 fir1;
	arm_fir_instance_q15 fir2;

	float32_t  *firInput, *firOutput;*/

	// TODO: Add other required variables and comment the current ones to explain what they do

} *filter_t;

extern struct filter filter;

#endif /* INC_FILTER_H_ */
