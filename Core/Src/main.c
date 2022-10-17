#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <string.h>
#include <math.h>
#include "liquid.h"

#define SIGNAL_STEPS 5

int main() {
	
	printf("Testing liquid-dsp!\n");
	//FILE IO
	FILE* in = fopen("./res/signal.txt", "r");
	FILE* out = fopen("./res/processed.txt", "w");

	if(!in || !out) {
		printf("Error opening required files :(\n");
		fclose(in);
		fclose(out);
		return 1;
	}
	//FILE IO
		

	int j = 0;
	float i = 0;
	float q = 0;
	complex float sum = 0;
	uint32_t signal_length = 0;
	uint32_t current_step = 0;

	fscanf(in, "%u", &signal_length);
	complex float* samples = malloc(sizeof(complex float) * signal_length * SIGNAL_STEPS);

	while ((fscanf(in, "%f,%f", &i, &q)) != 0 && j < signal_length) {
		samples[j + signal_length * current_step] = i + q * I;
		sum += samples[j + signal_length * current_step];
		//printf("[%i] %f,%f\n", j, crealf(samples[j]), cimagf(samples[j]));
		j++;
	}

	/*Signal processing
	 * [x] 1. DC offset removal
	 * [x] 2. Downmixing
	 * [x] 3. Lowpass
	 * [x] 4. Decimation
	 * [?] 5. Symbolic synchronization
	 * [?] 6. Gaussian median shift key demodulation
	 * [ ] 7.
	 * [ ] 8.
	 * [ ] 9.
	 * [ ] 10.
	 * [ ] 11.
	 * [ ] 12.
	 * [ ] 13.
	 * [ ] 14.
	 */

	//1. DC offset removal
	sum /= (complex float)(j+1);
	for(int k = 0; k < signal_length; ++k) {
		samples[k + signal_length * current_step] -= sum;
	}
	current_step++;

	//2. Downmixing
	nco_crcf fdem = nco_crcf_create(LIQUID_NCO);
	nco_crcf_set_frequency(fdem, 2*M_PI*0.0875f);
	nco_crcf_set_phase(fdem, 0);
	nco_crcf_mix_block_down(
			fdem, 
			(complex float*) samples + signal_length * (current_step - 1), 
			(complex float*) samples + signal_length * current_step, signal_length);
	current_step++;

	//3. Lowpass filter
	float fc = 0.01f; // Filter cutoff frequency
	float ft = 0.1f; // Filter transition band width TODO: Wider bandwidth decreases filter length
	float attenuation = 60.0f; // Stop-band attenuation [dB] TODO: Less attenuation decreases filter length
	float mu = 0.0f; // Fractional timing offset TODO: Don't know what this does
	int num_taps = estimate_req_filter_len(ft, attenuation);	// Estimate filter length
	firfilt_crcf filter = firfilt_crcf_create_kaiser(num_taps, fc, attenuation, mu); // Create filter object
	firfilt_crcf_set_scale(filter, 2.0f*fc); // Scale filter
	for ( uint32_t k = 0; k < signal_length; ++k ) {
		//printf("%f, %f \t", crealf(samples[k]), cimagf(samples[k]));
		firfilt_crcf_push(filter, samples[k + signal_length * (current_step - 1)] );
		firfilt_crcf_execute(filter, &samples[k + signal_length * (current_step)] );
		//printf("%f, %f\n", crealf(samples[k]), cimagf(samples[k]));
	}
	current_step++;

	//4. Decimation
	float rate = 0.1f;	// Resampling rate (output/input). We want to decimate from 288 kHz to 28.8 kHz.
	int output_length = ceilf(rate * signal_length);
	uint32_t num_written = 0;   // Number of values written by the resampler to it output
	msresamp_crcf resampler = msresamp_crcf_create(rate, attenuation);	// Create msresamp object
	msresamp_crcf_execute(
			resampler, 
			samples + signal_length * (current_step - 1), 
			output_length, 
			samples + signal_length * current_step, 
			&(num_written));
	current_step++;

	//5. Symsync
	uint32_t sampersym = 3; //SAMPLES_PER_SYMBOL*dsp.rs.rate;
	uint32_t filter_delay = 3;	// Filter delay (symbols) NOTE: Hard faults when > 9...
	float beta = 0.3f;		// Filter excess bandwidth factor TODO: What does it do?
	// Number of polyphase filters in bank (more filters == better timing resolution) 
	// TODO: Value just from example
	uint32_t npfb  = 32;		
	symsync_crcf symsyncer = symsync_crcf_create_rnyquist(LIQUID_FIRFILT_GMSKRX, sampersym, filter_delay, beta, npfb);
    	symsync_crcf_execute(
			symsyncer, 
			samples + signal_length * (current_step - 1), 
			num_written, 
			samples + signal_length * current_step, 
			&(num_written));
	current_step++;

	//6. Demodulation
	/*
	output_length = ceil(output_length/(float)3) + 52;	// The output length is
	gmskdem demod = gmskdem_create(sampersym, filter_delay, beta);// Create demod object
	for(uint32_t k = 0; k < num_written; ++k ) {
		gmskdem_demodulate(
				demod, 
				samples + signal_length * (current_step - 1), 
				(unsigned int*) (samples + k + signal_length * current_step));
    	}*/

	//FILE IO
	fprintf(out, "%i,%i\n", SIGNAL_STEPS, signal_length);
	for(int step = 0; step < 1; ++step) {
		
		for(int k = 0; k < signal_length; ++k) {
			if(step < 3) {
				fprintf(out, "%f,%f\n", crealf(samples[k + signal_length * step]), cimagf(samples[k + signal_length * step]));
			}
			//fprintf(out, "%i,%i\n", demod_samples[k], demod_samples[k]);
	//		printf("%i", demod_samples[k]);
	//		if((k+1) % 8 == 0 && k != 0)
		//		printf("\n");
		}
	}

	fclose(in);
	fclose(out);
	free(samples);
	//FILE IO

	return 0;
}
