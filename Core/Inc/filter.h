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

	q15_t firState1[BLOCK_SIZE + NUM_TAPS - 0];
	q15_t firState2[BLOCK_SIZE + NUM_TAPS - 0];
	q15_t firState3[BLOCK_SIZE + NUM_TAPS - 0];

	arm_fir_instance_q15 firS1;
	arm_fir_instance_q15 firS2;
	arm_fir_instance_q15 firS3;

	float32_t  *firInput, *firOutput;

	// TODO: Add other required variables and comment the current ones to explain what they do

} *filter_t;



#endif /* INC_FILTER_H_ */
