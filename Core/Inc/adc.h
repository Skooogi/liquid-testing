/*
 * adc.h
 *
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_


#include <stdint.h>
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"


#define ADC_RX_PRIORITY				( tskIDLE_PRIORITY + 1 )
#define ADC_RX_BUF_SIZE  			0x1000						// Must be 2*N*FFT_SIZE, where N is an integer
#define ADC_TEMPERATURE_BUF_SIZE	0x800						// TODO: Just a preliminary guess, probably overkill but depends on the temperature data rate.
#define ADC_DBUF_LEN				4							// How many full sets of raw ADC data can fit one buffer in the double buffer.
#define BUF_SIZE 					0x1000
#define UINT16_OFFSET 				0x8000
#define SHORT_MAX 					0x7FFF						// Must be ADC_RX_BUF_SIZE/(2*N), where N is an integer
#define FFT_SIZE 					0x800
#define ADC_SAMPLERATE 				288000						// Sample rate of the ADC
#define DECIMATION_FACTOR 			3



/****** Only for prerecorded test data START ******/
#define TEST_LENGTH_SAMPLES ((uint32_t) 2048)
/****** Only for prerecorded test data END ******/


/* Structs for storing the states of the RF receiving ADCs */
typedef struct rfadc {

	uint8_t converting;													// Flag to tell whether the ADC is in the middle of conversions or not
	ALIGN_32BYTES ( int16_t rx_buf[ADC_RX_BUF_SIZE] );
	ALIGN_32BYTES ( int16_t data[ADC_RX_BUF_SIZE] );
	ALIGN_32BYTES ( int16_t data_fir[ADC_RX_BUF_SIZE] );

	uint16_t buf_A[ADC_DBUF_LEN * ADC_RX_BUF_SIZE];						// Part of double buffering for ADS131E08S samples for better interrupt handling.
	uint16_t buf_B[ADC_DBUF_LEN * ADC_RX_BUF_SIZE];						// Part of double buffering for ADS131E08S samples for better interrupt handling.
	uint8_t use_A, use_B, A_full, B_full;								// Flags to control the double buffering logic.
	uint32_t dbuf_idx;													// Index where to put the next samples in the currently active buffer.
	uint8_t dbuf_overrun_error;											// Error flag in case neither one of the buffers is available when new data arrives (debugging).

} *rfadc_t;

/* Declare the extern structs adcI and adcQ. */
extern struct rfadc adcI;
extern struct rfadc adcQ;



/* Struct for storing the state of the MCU temperature measuring ADC */
typedef struct tempadc {

	uint8_t converting;														// Flag to tell whether the ADC is in the middle of conversions or not
	ALIGN_32BYTES (uint16_t temperature_buf[ADC_TEMPERATURE_BUF_SIZE]);		// TODO: Is the alignment necessary?

} *tempadc_t;

/* Declare the extern struct adcT */
extern struct tempadc adcT;



/************* Publicly callable functions *************/
void prvADCTask( void *pvParameters );							// Task acquiring latest ADC data
void prvADCInit();												// Initialize the state of the ADC
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc);		// ADC conversion half complete callback
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc);			// ADC conversion complete callback

#endif /* INC_ADC_H_ */
