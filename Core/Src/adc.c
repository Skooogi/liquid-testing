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
//#include "arm_math.h"
//#include "arm_const_structs.h"

//#include "../../Drivers/gnu-ais/callbacks.h"
//#include "../../Drivers/gnu-ais/filter.h"
//#include "../../Drivers/gnu-ais/hmalloc.h"
//#include "../../Drivers/gnu-ais/protodec.h"
//#include "../../Drivers/gnu-ais/receiver.h"

uint32_t prim;

/* Instance of ADC data and state struct for ADCI (&hadc1) */
struct mcuadc adcI = {

};

/* Instance of ADC data and state struct for ADCQ (&hadc3) */
struct mcuadc adcQ = {

};


void prvADCTask( void *pvParameters )
{
	( void ) pvParameters;
	// TODO: Read data that has been received by the ADC.
}


void prvADCInit()
{

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


