#include "can_interface.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "debugRTT.h"

FDCAN_RxHeaderTypeDef RxHeader; //FDCAN Bus Transmit Header
FDCAN_TxHeaderTypeDef TxHeader; //FDCAN Bus Receive Header
FDCAN_HandleTypeDef hfdcan1;
uint8_t RxData[8];
uint8_t TxData[8];

static QueueHandle_t xQueue = NULL;

/* CAN TX Task */
void canTXTask(void* param) {

	xQueue = xQueueCreate(1, sizeof(uint32_t));

	pulseLED(300,10);
	pulseLED(300,10);
	uint32_t command = 0;

	for(;;) {
		xQueueReceive(xQueue, &command, portMAX_DELAY);

		switch(command) {
			case 0x1:
				TxData[0] = '1';
				break;
			case 0x2:
				TxData[0] = '2';
				break;
			case 0x3:
				TxData[0] = '3';
				break;
			case 0x4:
				TxData[0] = '4';
				break;
			case 0x5:
				TxData[0] = '5';
				break;
			case 0x6:
				TxData[0] = '6';
				break;
			case 0x7:
				TxData[0] = '7';
				break;
			default:
				TxData[0] = ':';
				TxData[1] = '(';
				break;
		}

		/* Start the transmission process*/
		if(HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, TxData) != HAL_OK) {
			while(1);
		}
		pulseLED(30,30);
	}
}

/* FDCAN RX callback */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs) {

	if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET) {

		if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK) {
			while(1);
		}

		//Check that the message is coming from the obc and contains atleast one character.
		if ((RxHeader.Identifier == 0x1) && (RxHeader.IdType == FDCAN_STANDARD_ID) && (RxHeader.DataLength >= FDCAN_DLC_BYTES_1)) {
			xQueueSendFromISR(xQueue, &RxData[0], 0U);
		}

		if (HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK) {
			while(1);
		}
	}
}

