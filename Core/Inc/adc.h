/*
 * adc.h
 *
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_


#include <stdint.h>
#include "stm32h7xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"


#define ADC_RX_PRIORITY				( tskIDLE_PRIORITY + 1 )			// ADC receive task priority
#define ADC_RX_BUF_SIZE  			0x200								// Size of the ADC receive buffer
#define ADC_TEMPERATURE_BUF_SIZE	0x20								// TODO: Just a preliminary guess
#define ADC_SAMPLERATE 				288000.0f							// Sample rate of the ADC


/* Struct for storing the data of the RF receiving ADCs (ADC1 a.k.a ADCI and ADC2 a.k.a ADCQ) */
typedef struct rfadc
{

	uint32_t rx_buf[ADC_RX_BUF_SIZE];									// Converted data of both ADC1 and ADC2 are stored in this buffer
	int32_t data[ADC_RX_BUF_SIZE];										// The converted data is copied to this buffer in an interrupt for post processing

} *rfadc_t;

/* Declare the extern struct adcIQ . */
extern struct rfadc adcIQ;


/* Struct for storing the state of the MCU temperature measuring ADC */
typedef struct tempadc
{

	uint16_t temperature_buf[ADC_TEMPERATURE_BUF_SIZE];

} *tempadc_t;

/* Declare the extern struct adcT */
extern struct tempadc adcT;


/************* Publicly callable functions *************/
void prvADCTask( void *pvParameters );									// Task acquiring latest ADC data
void prvADCInit();														// Initialize the state of the ADC
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc);				// ADC conversion half complete callback
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc);					// ADC conversion complete callback


#endif /* INC_ADC_H_ */
