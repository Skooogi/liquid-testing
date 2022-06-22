/*
 * adc.c
 *
 * This file contains functions controlling the ADC of the MCU.
 *
 */

#include "main.h"
#include "dsp.h"
#include "sdr.h"

#include "math.h"
#include "usbd_cdc_if.h"
#include "complex.h"
#include "arm_math.h"
#include "arm_const_structs.h"




//#include "../../Drivers/gnu-ais/callbacks.h"
//#include "../../Drivers/gnu-ais/filter.h"
//#include "../../Drivers/gnu-ais/hmalloc.h"
//#include "../../Drivers/gnu-ais/protodec.h"
//#include "../../Drivers/gnu-ais/receiver.h"



uint32_t prim;

/* Instance of ADC data and state struct for ADCI (&hadc1) */
struct rfadc adcI = {
	.converting = 0,
	.dbuf_overrun_error = 0
};

/* Instance of ADC data and state struct for ADCQ (&hadc2) */
struct rfadc adcQ = {
	.converting = 0,
	.dbuf_overrun_error = 0
};

/* Instance of ADC data and state struct for ADCT (&hadc3) */
struct tempadc adcT = {
	.converting = 0
};


void prvADCInit(TIM_HandleTypeDef *htim)
{
	/* Initialize the variables storing the states of the ADCs. */

	adcI.converting = 0;
	adcQ.converting = 0;
	adcT.converting = 0;


	memset( adcI.rx_buf, 0, ADC_RX_BUF_SIZE*sizeof(uint16_t) );
	memset( adcI.data, 0, ADC_RX_BUF_SIZE*sizeof(uint16_t) );
	memset( adcI.data_fir, 0, ADC_RX_BUF_SIZE*sizeof(uint16_t) );

	memset( adcQ.rx_buf, 0, ADC_RX_BUF_SIZE*sizeof(uint16_t) );
	memset( adcQ.data, 0, ADC_RX_BUF_SIZE*sizeof(uint16_t) );
	memset( adcQ.data_fir, 0, ADC_RX_BUF_SIZE*sizeof(uint16_t) );

	memset( adcT.temperature_buf, 0, ADC_TEMPERATURE_BUF_SIZE*sizeof(uint16_t) );


	/* Start RF receiving ADCs. Save data to adcX.rx_buf */
	if ( HAL_ADC_Start_DMA( ADCI, (uint32_t *)adcI.rx_buf, ADC_RX_BUF_SIZE ) != HAL_OK )
	{
		Error_Handler(); //does nothing-> TODO: error handler
	}

    if ( HAL_ADC_Start_DMA( ADCQ, (uint32_t *)adcQ.rx_buf, ADC_RX_BUF_SIZE ) != HAL_OK )
    {
    	Error_Handler(); //does nothing-> TODO: error handler
    }


    /* Start MCU temperature reading ADC. Save data to adcT.rx_buf */
    if ( HAL_ADC_Start_DMA( ADCT, (uint32_t *)adcT.temperature_buf, ADC_TEMPERATURE_BUF_SIZE ) != HAL_OK )
    {
    	Error_Handler(); //does nothing-> TODO: error handler
    }


    /* Start timer for the ADCs */
    if ( HAL_TIM_Base_Start( TIM_1 ) != HAL_OK )		// TODO: Check if this is the right timer and right place to start it
    {
    	Error_Handler(); //does nothing-> TODO: error handler
    }

}


/* ADC conversion complete callback */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{

	prim = __get_PRIMASK();
	__disable_irq();

	memcpy(adcI.data, adcI.rx_buf, ADC_RX_BUF_SIZE);
	memcpy(adcQ.data, adcQ.rx_buf, ADC_RX_BUF_SIZE);

	dsp.processing_request_flag = 1;
	xTaskNotifyGive( DSPTaskHandle );		// Notify the DSP Task that there are data in need of processing

}

