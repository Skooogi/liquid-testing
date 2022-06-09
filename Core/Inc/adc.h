/*
 * adc.h
 *
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_

#include <stdint.h>

#define ADC_RX_PRIORITY	( tskIDLE_PRIORITY + 3 )
#define ADC_RX_BUF_SIZE 0x200	// TODO: This is just a placeholder value

typedef struct mcuadc {
	uint8_t *rx_buf[ADC_RX_BUF_SIZE];

} *mcuadc_t;


/************* Publicly callable functions *************/
/* Task acquiring latest ADC data */
void prvADCTask( void *pvParameters );

#endif /* INC_ADC_H_ */
