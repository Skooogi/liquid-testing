/*
 * filter.h
 *
 */

#ifndef INC_FILTER_H_
#define INC_FILTER_H_


#include "arm_math.h"
#include "dsp.h"

#if defined(ARM_MATH_MVEF) && !defined(ARM_MATH_AUTOVECTORIZE)
/* Must be a multiple of 16 */
#define TAPS_ARRAY_SIZE   		32
#else
#define TAPS_ARRAY_SIZE   		61
#endif

#define NUM_TAPS         		60


typedef struct filter {

	q15_t fir1_coeffs[TAPS_ARRAY_SIZE];
	q15_t fir2_coeffs[TAPS_ARRAY_SIZE];

	q15_t fir1_state[BLOCK_SIZE + NUM_TAPS];
	q15_t fir2_state[BLOCK_SIZE + NUM_TAPS];

	arm_fir_instance_q15 fir1;
	arm_fir_instance_q15 fir2;

	float32_t  *firInput, *firOutput;

	// TODO: Add other required variables and comment the current ones to explain what they do

} *filter_t;

extern struct filter filters;

#endif /* INC_FILTER_H_ */
