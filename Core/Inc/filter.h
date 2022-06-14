/*
 * filter.h
 *
 */

#ifndef INC_FILTER_H_
#define INC_FILTER_H_


#include "arm_math.h"
#include "dsp.h"


typedef struct filter {

	q15_t firCoeff025[NUM_TAPS_ARRAY_SIZE];
	q15_t firCoeff001[NUM_TAPS_ARRAY_SIZE];
	q15_t firCoeffhighpass[NUM_TAPS_ARRAY_SIZE];

	q15_t firState1[BLOCK_SIZE + NUM_TAPS];
	q15_t firState2[BLOCK_SIZE + NUM_TAPS];
	q15_t firState3[BLOCK_SIZE + NUM_TAPS];

	arm_fir_instance_q15 fir1;
	arm_fir_instance_q15 fir2;

	float32_t  *firInput, *firOutput;

	// TODO: Add other required variables and comment the current ones to explain what they do

} *filter_t;

extern struct filter filters;

#endif /* INC_FILTER_H_ */
