/*
 * adc.c
 *
 * This file contains functions controlling the ADCs of the MCU.
 *
 */


#include <string.h>
#include "main.h"
#include "dsp.h"


/* Instance of ADC data and state struct for both ADC1 and ADC2 (operating in dual mode with ADC1 as master and ADC2 as slave). */
struct rfadc adcIQ;


/* Instance of ADC data and state struct for ADCT (&hadc3) TODO: No functionality implemented for this yet. Could be used to read the internal temperature sensor of the processor for housekeeping data */
struct tempadc adcT;


/* Initialize ADCs */
void prvADCInit(TIM_HandleTypeDef *htim)
{

	memset( adcIQ.rx_buf, 0, ADC_RX_BUF_SIZE*sizeof(uint32_t) );
	memset( adcIQ.data, 0, ADC_RX_BUF_SIZE*sizeof(uint32_t) );
	memset( adcT.temperature_buf, 0, ADC_TEMPERATURE_BUF_SIZE*sizeof(uint16_t) );

}


/* ADC conversion half complete callback */
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{

	if ( hadc == &hadc1 )
	{
		/* Copy the data from the first half of the RX buffer for DSP so that the first half is ready for new data as fast as possible */
		memcpy( adcIQ.data, adcIQ.rx_buf, sizeof(uint32_t)*ADC_RX_BUF_SIZE/2);
	}

	HAL_GPIO_TogglePin(GPIOB, CANLED_Pin);
	//xTaskNotifyGive( DSPTaskHandle );						// Notify the DSP Task that there are data in need of processing

}


/* ADC conversion complete callback */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{

	if(hadc == &hadc1)
	{
		/* Copy the data from the second half of the RX buffer for DSP so that the second half is ready for new data as fast as possible */
		memcpy(adcIQ.data+ADC_RX_BUF_SIZE/2, adcIQ.rx_buf+ADC_RX_BUF_SIZE/2, sizeof(uint32_t)*ADC_RX_BUF_SIZE/2);
	}

	HAL_GPIO_TogglePin(GPIOB, CANLED_Pin);
	//xTaskNotifyGive( DSPTaskHandle );						// Notify the DSP Task that there are data in need of processing TODO: Uncomment for testing

}

