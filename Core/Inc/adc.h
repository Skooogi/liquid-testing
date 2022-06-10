/*
 * adc.h
 *
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_

#include <stdint.h>

#define ADC_RX_PRIORITY	( tskIDLE_PRIORITY + 1 )

#define ADC_RX_BUF_SIZE  		((uint32_t)  0x7FFF)
#define BUF_SIZE 				0x1000
#define UINT16_OFFSET 			0x8000
#define SHORT_MAX 				0x7FFF
#define FFT_SIZE 				0x800
#define SAMPLERATE 				312500		// TODO: This must change
#define DECIMATION_FACTOR 		3

#define SNR_THRESHOLD_F32    	75.0f
#define BLOCK_SIZE            	64

#if defined(ARM_MATH_MVEF) && !defined(ARM_MATH_AUTOVECTORIZE)
/* Must be a multiple of 16 */
#define NUM_TAPS_ARRAY_SIZE   	32
#else
#define NUM_TAPS_ARRAY_SIZE   	61
#endif

#define NUM_TAPS         		60


/****** Only for prerecorded test data START ******/
#define TEST_LENGTH_SAMPLES ((uint32_t) 2048)
/****** Only for prerecorded test data END ******/


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


typedef struct mcuadc {

	ALIGN_32BYTES (uint16_t   rx_buf[ADC_RX_BUF_SIZE]);
	ALIGN_32BYTES (int16_t 	 data[ADC_RX_BUF_SIZE]);
	ALIGN_32BYTES (int16_t 	 data_fir[ADC_RX_BUF_SIZE]);

} *mcuadc_t;

/* Declare the extern structs ADCI and ADCQ. */
extern struct mcuadc adcI;
extern struct mcuadc adcQ;


/************* Publicly callable functions *************/
/* Task acquiring latest ADC data */
void prvADCTask( void *pvParameters );
void prvADCInit();

#endif /* INC_ADC_H_ */
