/*
 * adc.c
 *
 * This file contains functions controlling the ADC of the MCU.
 *
 */

#include "main.h"
#include "adc.h"

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
	.converting = 0
};

/* Instance of ADC data and state struct for ADCQ (&hadc2) */
struct rfadc adcQ = {
	.converting = 0
};

/* Instance of ADC data and state struct for ADCT (&hadc3) */
struct tempadc adcT = {
	.converting = 0
};


void prvADCTask( void *pvParameters )
{
	( void ) pvParameters;
	// TODO: Implement! :)
	prvADCInit();


}


void prvADCInit(TIM_HandleTypeDef *htim)
{
	/* Initialize the variables storing the states of the ADCs. */
	/*
	adcI.converting = 0;
	adcQ.converting = 0;
	adcT.converting = 0;
	*/

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


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{

	prim = __get_PRIMASK();
	__disable_irq();

	if( hadc == &hadc1 ){

		memcpy( adcI.data + ADC_RX_BUF_SIZE/2, adcI.rx_buf + ADC_RX_BUF_SIZE/2, sizeof(uint16_t) * ADC_RX_BUF_SIZE/2 );
	    memcpy( adcQ.data + ADC_RX_BUF_SIZE/2, adcQ.rx_buf + ADC_RX_BUF_SIZE/2, sizeof(uint16_t) * ADC_RX_BUF_SIZE/2 );

	}

	if( !prim ){
		__enable_irq();

	}

}
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{

	prim = __get_PRIMASK();
	__disable_irq();

	if( hadc == &hadc1 ){
	  memcpy( adcI.data, adcI.rx_buf, sizeof(uint16_t) * ADC_RX_BUF_SIZE/2 );
	  memcpy( adcQ.data, adcQ.rx_buf, sizeof(uint16_t) * ADC_RX_BUF_SIZE/2 );
	}

	if( !prim ){
		__enable_irq();

	}
}


