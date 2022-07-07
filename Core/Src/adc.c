/*
 * adc.c
 *
 * This file contains functions controlling the ADC of the MCU.
 *
 */


#include <string.h>
#include "main.h"
#include "dsp.h"


/* Instance of ADC data and state struct for both ADC1 and ADC2 (operating in dual mode with ADC1 as master and ADC2 as slave). */
struct rfadc adcIQ = { };


/* Instance of ADC data and state struct for ADCT (&hadc3) */
struct tempadc adcT = { };


void prvADCInit(TIM_HandleTypeDef *htim)
{
	/* Initialize the arrays storing the states of the ADCs. */
	memset( adcIQ.rx_buf, 0, ADC_RX_BUF_SIZE*sizeof(uint32_t) );
	memset( adcIQ.data, 0, ADC_RX_BUF_SIZE*sizeof(uint32_t) );
	memset( adcT.temperature_buf, 0, ADC_TEMPERATURE_BUF_SIZE*sizeof(uint32_t) );

}


/* ADC conversion half complete callback */
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{

	if ( hadc == &hadc1 )
	{
		memcpy( adcIQ.data, adcIQ.rx_buf, sizeof(uint32_t)*ADC_RX_BUF_SIZE/2);		// Copy the data from the first half of the RX buffer for post processing
	}

	dsp.processing_request_flag = 1;
	HAL_GPIO_TogglePin(GPIOB, CANLED_Pin);
	//xTaskNotifyGive( DSPTaskHandle );						// Notify the DSP Task that there are data in need of processing

}

/* ADC conversion complete callback */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{

	if(hadc == &hadc1)
	{
		memcpy(adcIQ.data+ADC_RX_BUF_SIZE/2, adcIQ.rx_buf+ADC_RX_BUF_SIZE/2, sizeof(uint32_t)*ADC_RX_BUF_SIZE/2);	// Copy the data from the second half of the RX buffer for post processing
	}

	dsp.processing_request_flag = 1;
	HAL_GPIO_TogglePin(GPIOB, CANLED_Pin);
	//xTaskNotifyGive( DSPTaskHandle );						// Notify the DSP Task that there are data in need of processing TODO: Uncomment for testing

}

