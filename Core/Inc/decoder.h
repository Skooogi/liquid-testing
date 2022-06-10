/*
 * decoder.h
 *
 */

#ifndef INC_DECODER_H_
#define INC_DECODER_H_


#include <stdint.h>
#include "math.h"
#include "complex.h"
#include "adc.h"
#include "arm_math.h"


typedef struct decoder {

	complex complex_data;
	complex prev_complex;
	complex temp_complex;
	int16_t demodulated_IQ[ADC_RX_BUF_SIZE];
	float64_t temp_I;
	float64_t temp_Q;

	uint32_t prim;
	q31_t maxValue;

	uint32_t block_size;
	uint32_t num_blocks;
	float32_t snr;

	// TODO: Explain all variables. Decoder heavily depends on the filter implementation that can be found in filter.c and filter.h

} *decoder_t;

/* Declare the extern struct decoder. */
extern struct decoder decoder;


#endif /* INC_DECODER_H_ */
