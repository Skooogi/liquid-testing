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

	/* Use double buffering to buy time for our heavy signal processing. */

	/* ADCI */
ADCI_A_buf:
	/* If A is not full AND (if B is full OR B is not used) save to A.*/
	if ( !adcI.A_full && (adcI.B_full || !adcI.use_B) )
	{
		adcI.use_A = 1;
		if ( adcI.dbuf_idx < ADC_DBUF_LEN )
		{
			memcpy( adcI.buf_A + ((adcI.dbuf_idx) * ADC_RX_BUF_SIZE), adcI.rx_buf, ADC_RX_BUF_SIZE );
			memset( adcI.rx_buf, 0, ADC_RX_BUF_SIZE*sizeof(uint16_t) );		// This can in principle be removed (helps to identify issues for debugging)
			adcI.dbuf_idx++;
			goto ADCQ_A_buf;
		}
		else
		{
			adcI.use_A = 0;
			adcI.A_full = 1;
			adcI.dbuf_idx = 0;
			goto ADCI_B_buf;
		}
	}
ADCI_B_buf:
	/* If B is not full AND (if A is full OR A is not in use) save to B.*/
	if ( !adcI.B_full && (adcI.A_full || !adcI.use_A) )
	{
		adcI.use_B = 1;
		if ( adcI.dbuf_idx < ADC_DBUF_LEN )
		{
			memcpy( adcI.buf_B + adcI.dbuf_idx * ADC_RX_BUF_SIZE, adcI.rx_buf, ADC_RX_BUF_SIZE );
			memset( adcI.rx_buf, 0, ADC_RX_BUF_SIZE*sizeof(uint16_t) );		// This can in principle be removed (helps to identify issues for debugging)
			adcI.dbuf_idx++;
			goto ADCQ_A_buf;
		}
		else
		{
			adcI.use_B = 0;
			adcI.B_full = 1;
			adcI.dbuf_idx = 0;
			goto ADCI_A_buf;
		}
	}
	// TODO: Can be removed once no longer needed.
	/* The code should end up here only if both a and b buffers are full of unprocessed
	 * data when new data is received from the ADC. Raise error flag for debugging purposes. */
	adcI.dbuf_overrun_error = 1;


	/* ADCQ */
ADCQ_A_buf:
	/* If A is not full AND (if B is full OR B is not used) save to A.*/
	if ( !adcQ.A_full && (adcQ.B_full || !adcQ.use_B) )
	{
		adcQ.use_A = 1;
		if ( adcQ.dbuf_idx < ADC_DBUF_LEN )
		{
			memcpy( adcQ.buf_A + ((adcQ.dbuf_idx) * ADC_RX_BUF_SIZE), adcQ.rx_buf, ADC_RX_BUF_SIZE );
			memset( adcQ.rx_buf, 0, ADC_RX_BUF_SIZE*sizeof(uint16_t) );		// This can in principle be removed (helps to identify issues for debugging)
			adcQ.dbuf_idx++;
			if ( (adcI.A_full && adcQ.A_full) || (adcI.B_full && adcQ.B_full))
			{
				dsp.processing_request_flag = 1;
				xTaskNotifyGive( DSPTaskHandle );		// Notify the DSP Task that there are data in need of processing
			}
			return;
		}
		else
		{
			adcQ.use_A = 0;
			adcQ.A_full = 1;
			adcQ.dbuf_idx = 0;
			goto ADCQ_B_buf;
		}
	}
ADCQ_B_buf:
	/* If B is not full AND (if A is full OR A is not in use) save to B.*/
	if ( !adcI.B_full && (adcI.A_full || !adcI.use_A) )
	{
		adcI.use_B = 1;
		if ( adcI.dbuf_idx < ADC_DBUF_LEN )
		{
			memcpy( adcI.buf_B + adcI.dbuf_idx * ADC_RX_BUF_SIZE, adcI.rx_buf, ADC_RX_BUF_SIZE );
			memset( adcI.rx_buf, 0, ADC_RX_BUF_SIZE*sizeof(uint16_t) );		// This can in principle be removed (helps to identify issues for debugging)
			adcI.dbuf_idx++;
			return;
		}
		else
		{
			adcI.use_B = 0;
			adcI.B_full = 1;
			adcI.dbuf_idx = 0;
			goto ADCQ_A_buf;
		}
	}
	/* The code should end up here only if both A and B buffers are full of unprocessed
	 * data when new data is received from the ADC. Raise error flag for debugging purposes. */
	adcI.dbuf_overrun_error = 1;
}

