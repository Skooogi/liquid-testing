#include "can_interface.h"

#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "debugRTT.h"

uint8_t RxData[8];
uint8_t TxData[8];
char ais_example[][80] = {
	"!AIVDM,1,1,,A,13aEOK?P00PD2wVMdLDRhgvL289?,0*26\t",
	"!AIVDM,1,1,,B,16S`2cPP00a3UF6EKT@2:?vOr0S2,0*00\t",
	"!AIVDM,2,1,9,B,53nFBv01SJ<thHp6220H4heHTf2222222222221?50:454o<`9QSlUDp,0*09\t",
	"!AIVDM,2,2,9,B,888888888888880,2*2E\t" };

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

void getStatus() {
	global.mode ? sendMessage("STATUS=1") : sendMessage("STATUS=0");
}

void getMessages(short num) {
	for(int i = 0; i < num; ++i) {
		sendMessage(ais_example[i%4]);
	}
	sendMessage("\t\t");
}

void setParam(short param, float val) {

	switch(param) {
		case 0x0:
			global.bandwith = val;
			break;
		case 0x1:
			global.centre = val;
			break;
		case 0x2:
			global.cutoff = val;
			break;
		case 0x03:
			global.wSize = val;
			break;
		default:
			break;
	}
}

void setSTM32Registry(short reg, uint32_t val) {
	
}

void setVCOFreq(float freq) {
	global.vco = freq;
}

/* CAN TX Task */
void canTXTask(void* param) {

	xQueue = xQueueCreate(1, sizeof(uint64_t));

	pulseLED(300,10);
	pulseLED(300,10);
	uint64_t command = 0;

	global.telemetry[0] = 'T';
	global.telemetry[1] = 'l';
	global.telemetry[2] = 'm';
	global.telemetry[3] = 'T';
	global.telemetry[4] = 'r';

	for(;;) {
		xQueueReceive(xQueue, &command, portMAX_DELAY);

		switch(command>>48) {
			case 0x0:
				getStatus();
				break;
			case 0x1:
				getMessages((uint32_t) command);
				break;
			case 0x2:
				global.mode = (command & 0xFFFF) ? 1 : 0;
				break;
			case 0x3:
				sendMessage(global.telemetry);
				break;
			case 0x4:
				setParam((short)(command >> 32),(float)command);
				break;
			case 0x5:
				setSTM32Registry((short)(command >> 32),(uint32_t)command);
				break;
			case 0x6:
				setSTM32Registry((short)(command >> 32),(uint32_t)command);
				break;
			case 0x7:
				setVCOFreq((float) command);
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
			uint8_t temp = 0;
			for(int i = 0; i < 4; ++i) {
				temp = RxData[i];
				RxData[i] = RxData[7-i];
				RxData[7-i] = temp;
			}
			xQueueSendFromISR(xQueue, RxData, 0U);
		}

		if (HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK) {
			while(1);
		}
	}
}
