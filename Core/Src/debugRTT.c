#include "debugRTT.h"

#include "main.h"
#include "FreeRTOS.h"
#include "timers.h"

#include <string.h>
/*
 * RTT communication and some useful functions.
 */

void readRTT(char* p) {
	*p = SEGGER_RTT_WaitKey();
}

void writeRTT(char* p) {
	SEGGER_RTT_Write(0,p,strlen(p));
}

//Listens to Terminal 0 and awaits user input.
void prvDebugRTT( void *pvParameters ) {

	( void ) pvParameters;

	char* text = calloc(256, 1);
	writeRTT("Debugger online!");

	for( ;; )
	{
		readRTT(text); //Halts task until user sends input.
		writeRTT(text); //Prints the input back into the terminal.
	}

	free(text);
}

void pulseLED(int ms_duration, int ms_pause) {
	HAL_GPIO_TogglePin(GPIOB, CANLED_Pin);
	vTaskDelay(ms_duration);
	HAL_GPIO_TogglePin(GPIOB, CANLED_Pin);
	vTaskDelay(ms_pause);
}
