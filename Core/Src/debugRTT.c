#include "debugRTT.h"

#include "main.h"
#include "FreeRTOS.h"
#include "timers.h"
/*
 * RTT communication and some useful functions.
 */

//Listens to Terminal 0 and awaits user input.
void prvDebugRTT( void *pvParameters ) {

	( void ) pvParameters;

	char* text = calloc(256, 1);
	SEGGER_RTT_TerminalOut(0, "Debugger online!");

	for( ;; )
	{
		*text = SEGGER_RTT_WaitKey(); //Halts task until user sends input.
		SEGGER_RTT_TerminalOut(0, text); //Prints the input back into the terminal.
	}

	free(text);
}

//TODO print and scan overrites

void pulseLED(int ms_duration, int ms_pause) {
	HAL_GPIO_TogglePin(GPIOB, CANLED_Pin);
	vTaskDelay(ms_duration);
	HAL_GPIO_TogglePin(GPIOB, CANLED_Pin);
	vTaskDelay(ms_pause);
}
