#include "main.h"

int main(void) {

	init_stm32();

	while(1) {
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);

		// Wait for 500 ms
		HAL_Delay(500);	
	}
}
