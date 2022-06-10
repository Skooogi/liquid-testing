/*
 * adc.c
 *
 * This file contains functions controlling the ADC of the MCU.
 *
 */

#include "main.h"
#include "adc.h"

/* Instance of ADC data and state struct for ADCI (&hadc1) */
struct mcuadc adcI = {

};

/* Instance of ADC data and state struct for ADCQ (&hadc3) */
struct mcuadc adcQ = {

};


prvADCTask( void *pvParameters )
{
	( void ) pvParameters;
	// TODO: Read data that has been received by the ADC.
}

void prvADCInit()
{

}



