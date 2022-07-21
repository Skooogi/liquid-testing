#include "can_interface.h"

#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "debugRTT.h"

uint8_t RxData[8];
uint8_t TxData[8];

FDCAN_RxHeaderTypeDef RxHeader;
FDCAN_TxHeaderTypeDef TxHeader;
FDCAN_HandleTypeDef hfdcan1;

static QueueHandle_t xQueue = NULL;

typedef struct {

	char telemetry[20];
	uint8_t mode;

	float bandwith;
	float centre;
	float cutoff;
	float wSize;
	float vco;

	uint32_t cmx;
	uint32_t registry;

} state;

state global = {0};

static void sendMessage(char* data) {

	size_t curr = 0;
	do {
		if(strlen(data) - curr > 8) {
			memcpy(TxData, data + curr, 8*sizeof(uint8_t));
		}
		else {
			memset(TxData, 0, 8);
			memcpy(TxData, data + curr, (strlen(data)-curr));
		}
		curr += 8;

		/* Start the transmission process*/
		if(HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, TxData) != HAL_OK) {
			while(1);
		}
		pulseLED(30,30);

	} while(curr < strlen(data));

}

/* CAN TX Task */
void canTXTask(void* param) {

	xQueue = xQueueCreate(1, sizeof(uint32_t));

	pulseLED(300,10);
	pulseLED(300,10);
	uint32_t command = 0;

	global.telemetry[0] = 'T';
	global.telemetry[1] = 'l';
	global.telemetry[2] = 'm';
	global.telemetry[3] = 'T';
	global.telemetry[4] = 'r';

	for(;;) {
		xQueueReceive(xQueue, &command, portMAX_DELAY);

		switch(command) {
			case 0x0:
				global.mode ? sendMessage("STATUS=1") : sendMessage("STATUS=0");
				break;
			case 0x1:
				for(int i = 0; i < 7; ++i) {
					sendMessage("AISMSG\t");
				}
				sendMessage("\t\t");
				break;
			case 0x2:
				sendMessage("SET MODE");
				xQueueReceive(xQueue, &command, portMAX_DELAY);
				global.mode = RxData[0];
				break;
			case 0x3:
				sendMessage(global.telemetry);
				break;
			case 0x4:
				sendMessage("WRITE0x01");
				break;
			case 0x5:
				sendMessage("STM32REG");
				break;
			case 0x6:
				sendMessage("CMX994A");
				break;
			case 0x7:
				sendMessage("VCO_FREQ");
				break;
			default:
				sendMessage("!COMMAND");
				break;
		}
		pulseLED(30,30);

	}
}

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
