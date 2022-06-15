/*
 * filter.c
 *
 * This file contains functions responsible for filtering received RF signals.
 *
 */

#include "filter.h"

/* Instance of struct containing filters for DSP */
struct filter filters = { };


/* This function calculates the coefficients for a FIR filter (a windowed sinc-filter). Cutoff frequency is normalized ([0, 1]) */
static uint32_t calculate_filter_coeffs(uint32_t fs, uint32_t fc, uint32_t res, q15_t *retcoeffs)
{
	// TODO: Define the desired cutoff frequencies for the settings (nothing prevents having even more different options). The filtering decimation ratio must fit the cutoff frequency.

	/* First, let's set the cutoff-frequency according to the settings. */
	//float fs = MATTI_DATA_RATE;
	//float fc = filter.f_cutoff;
	float step = fc/(fs/2); 							// Cutoff is fc = step*Fs/2. Step is the step at which the sinc function is sampled for the filter.

	/* sinc(x) = sin(pi*x)/(pi*x), sinc(0)=1 */
	float limit=step*res;								// Limit of interval at which to sample sinc. Increasing trans narrows transition band and reduces ripples.
	const uint32_t N = 2*res + 1;						// Length of array c used to compute the coefficients.
	float x;
	float c[N];
	for (int i=0; i<N; i++) {
		x = -limit + i*step;
		if (fabs(x) > 0) {
			c[i] = sin(M_PI*x)/(M_PI*x);
		} else {
			c[i] = 1;
		}
	}
	/* Windowing, using the Hann window. */
	float win[N];
	float cw[N];
	for (int i=0; i<N; i++) {//n = 0 : N-1,
		win[i] = 0.5 * (1-cos(2*M_PI*i/(N-1)));
		cw[i] = c[i] * win[i];
	}
	/* Normalize to have DC gain equal to 1. */
	float sum_cw = 0;
	for (int i=0; i<N; i++) {
		sum_cw += cw[i];
	}
	for (int i=0; i<N; i++) {
		//c[i] = c[i]/sum_c;
		cw[i] = cw[i]/sum_cw;
	}
	/* Move center frequency to w0. */
	float w0 = 0; 										// From 0 to 1, 1 is Fs/2. We want a low pass filter, so w0 = 0.
	float c0 = (1/cos(M_PI*w0)) * (1/cos(M_PI*w0)); 	// To maintain DC gain equal to 1
	for (int i=0; i<N; i++) {
		cw[i] = cw[i] * cos(M_PI*w0*i) * c0;
	}
	/* Quantization */
	int32_t Q = 256; 									// 2^8 bits for the coefficient mantissa
	int32_t cwr[N];
	for (int i=0; i<N; i++) {
		cwr[i] = (int32_t) round(cw[i]*Q);				// The coefficients are rounded to integers.
	}
	int32_t *cwp = cwr;
	int i = 0;
	while ((cwp[i] == 0) && (cwp[(N-1)-i] == 0)) {
		i++;
	}
	const uint32_t j = i;
	const uint32_t cn = N-2*j;
	memcpy(retcoeffs, cwp+j, cn*sizeof(int32_t));

	return cn;

}



/* Initialize the filters */
void prvFilterInit()
{
	/* Calculate the filter coefficients */
	calculate_filter_coeffs( ADC_SAMPLERATE, ADC_SAMPLERATE/4, 43, filters.fir1_coeffs );		// TODO: Determine correct parameters (test in MATLAB)
	calculate_filter_coeffs( ADC_SAMPLERATE, ADC_SAMPLERATE/4, 43, filters.fir2_coeffs );		// TODO: Determine correct parameters (test in MATLAB)

	arm_fir_init_q15( &(filters.fir1), NUM_TAPS, filters.fir1_coeffs, filters.fir1_state, BLOCK_SIZE );
	arm_fir_init_q15( &(filters.fir2), NUM_TAPS, filters.fir2_coeffs, filters.fir2_state, BLOCK_SIZE );
	// TODO: Could check that the filter initialization works (return value)
}




