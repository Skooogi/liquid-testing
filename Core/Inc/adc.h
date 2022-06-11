/*
 * adc.h
 *
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_

#include <stdint.h>
#include "main.h"

#define ADC_RX_PRIORITY	( tskIDLE_PRIORITY + 1 )

#define ADC_RX_BUF_SIZE  			((uint32_t)  0x7FFF)
#define ADC_TEMPERATURE_BUF_SIZE	0x1000		// TODO: Just a preliminary guess, probably overkill but depends on the temperature data rate.
#define BUF_SIZE 					0x1000
#define UINT16_OFFSET 				0x8000
#define SHORT_MAX 					0x7FFF
#define FFT_SIZE 					0x800
#define SAMPLERATE 					312500		// TODO: This must change
#define DECIMATION_FACTOR 			3

#define SNR_THRESHOLD_F32    		75.0f
#define BLOCK_SIZE            		64

#if defined(ARM_MATH_MVEF) && !defined(ARM_MATH_AUTOVECTORIZE)
/* Must be a multiple of 16 */
#define NUM_TAPS_ARRAY_SIZE   		32
#else
#define NUM_TAPS_ARRAY_SIZE   		61
#endif

#define NUM_TAPS         			60


/****** Only for prerecorded test data START ******/
#define TEST_LENGTH_SAMPLES ((uint32_t) 2048)
/****** Only for prerecorded test data END ******/


typedef struct rfadc {

	ALIGN_32BYTES (uint16_t  rx_buf[ADC_RX_BUF_SIZE]);
	ALIGN_32BYTES (int16_t 	 data[ADC_RX_BUF_SIZE]);
	ALIGN_32BYTES (int16_t 	 data_fir[ADC_RX_BUF_SIZE]);

} *rfadc_t;

/* Declare the extern structs ADCI and ADCQ. */
extern struct rfadc adcI;
extern struct rfadc adcQ;


typedef struct tempadc {

	ALIGN_32BYTES (uint16_t  temperature_buf[ADC_TEMPERATURE_BUF_SIZE]);	// TODO: Is the alignment necessary?

} *tempadc_t;

/* Declare the extern structs ADCI and ADCQ. */
extern struct tempadc adcT;



/************* Publicly callable functions *************/
void prvADCTask( void *pvParameters );							// Task acquiring latest ADC data
void prvADCInit();												// Initialize the state of the ADC
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc);		// ADC conversion half complete callback
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc);			// ADC conversion complete callback

#endif /* INC_ADC_H_ */
