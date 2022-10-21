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

	uint8_t mode;

	float bandwidth;
	float centre;
	float cutoff;
	float wSize;
	float vco;

	uint32_t cmx[100];
	uint32_t registry[100];

} state;

state global = {0};

static void sendData(unsigned char* data, uint32_t size) {
	
	memset(TxData, 0, 8);
	for(int i = 0; i < size; ++i) {
		memcpy(TxData+i,&data[size-1-i], sizeof(uint8_t));
	}

	/* Start the transmission process*/
	if(HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, TxData) != HAL_OK) {
		while(1);
	}
	pulseLED(30,30);
}

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

void setStatus(uint32_t command) {
	global.mode = (command & 0xFFFF) ? 1 : 0;
	sendMessage("MODE_SET");
}

void getMessages(short num) {
	for(int i = 0; i < num; ++i) {
		sendMessage(ais_example[i%4]);
	}
	sendMessage("\t\t");
}

void setParam(short param, uint32_t val) {

	//Swap float bytes
	float swapped = 0;
	unsigned char* temp1 = (unsigned char*) &val;
	unsigned char* temp2 = (unsigned char*) &swapped;
	temp2[0] = temp1[0];
	temp2[1] = temp1[1];
	temp2[2] = temp1[2];
	temp2[3] = temp1[3];

	switch(param) {
		case 0x0:
			global.bandwidth = swapped;
			break;
		case 0x1:
			global.centre = swapped;
			break;
		case 0x2:
			global.cutoff = swapped;
			break;
		case 0x3:
			global.wSize = swapped;
			break;
		case 0x4:
			global.vco = swapped;
		default:
			sendMessage("!PARAM");
			return;
			break;
	}
	sendMessage("PARAMSET");
}

void getTelemetry() {
	
	sendMessage("TELEMTR:");
	getStatus();
	sendMessage("BANDWDTH");
	unsigned char bytes[5] = {};
	memcpy(bytes, &global.bandwidth, sizeof(float));
	sendData(bytes, 4);
	sendMessage("CENTRE");
	memcpy(bytes, &global.centre, sizeof(float));
	sendData(bytes, 4);
	sendMessage("CUTOFF");
	memcpy(bytes, &global.cutoff, sizeof(float));
	sendData(bytes, 4);
	sendMessage("WINDOWSZ");
	memcpy(bytes, &global.wSize, sizeof(float));
	sendData(bytes, 4);
	sendMessage("VCO");
	memcpy(bytes, &global.vco, sizeof(float));
	sendData(bytes, 4);
	/*
	sendMessage("CMX");
	memcpy(bytes, &global.cmx, sizeof(uint32_t));
	sendData(bytes, 4);
	sendMessage("REGISTRY");
	memcpy(bytes, &global.registry, sizeof(uint32_t));
	sendData(bytes, 4);*/
	sendMessage("\t\t");
}

void getSTM32Registry(short reg, uint32_t val) {
	if(reg < 0 || reg > 99) {
		sendMessage("NO_REG");
		return;
	}

	unsigned char data[2] = {0};
	if(global.registry[reg]) {
		data[0] = 1;
	}
	sendData(data, 1);
}

void setSTM32Registry(short reg, uint32_t val) {
	if(reg < 0 || reg > 99) {
		sendMessage("NO_REG");
		return;
	}

	global.registry[reg] = val ? 1 : 0;
	sendMessage("REG_SET");
}

void setCMXRegistry(short reg, uint32_t val) {
	if(reg < 0 || reg > 99) {
		sendMessage("NO_REG");
		return;
	}

	global.cmx[reg] = val ? 1 : 0;
	sendMessage("REG_SET");
}

void getCMXRegistry(short reg, uint32_t val) {
	if(reg < 0 || reg > 99) {
		sendMessage("NO_REG");
		return;
	}

	global.cmx[reg] ? sendData((unsigned char*) 0x1, 1) : sendData((unsigned char*) 0x0, 1);
}

/* CAN TX Task */
void canTXTask(void* param) {

	xQueue = xQueueCreate(1, sizeof(uint64_t));

	pulseLED(300,50);
	pulseLED(300,50);
	uint64_t command = 0;

	global.bandwidth = 1.0f;
	global.centre = 2.0f;
	global.cutoff = 3.0f;
	global.wSize = 4.0f;
	global.vco = 5.0f;
	for(int i = 0; i < 100; ++i) {
		global.registry[i] = i;
		global.cmx[i] = i;
	}

	for(;;) {
		xQueueReceive(xQueue, &command, portMAX_DELAY);

		switch(command>>48) {
			case 0x0:
				getStatus();
				break;
			case 0x1:
				setStatus((uint32_t) command);
				break;
			case 0x2:
				getMessages((uint32_t) command);
				break;
			case 0x3:
				setParam((short)(command >> 32),(uint32_t) command);
				break;
			case 0x4:
				setParam(4,(uint32_t) command);
				break;
			case 0x5:
				getTelemetry();
				break;
			case 0x6:
				getSTM32Registry((short)(command >> 32),(uint32_t)command);
				break;
			case 0x7:
				setSTM32Registry((short)(command >> 32),(uint32_t)command);
				break;
			case 0x8:
				setCMXRegistry((short)(command >> 32),(uint32_t)command);
				break;
			case 0x9:
				getCMXRegistry((short)(command >> 32),(uint32_t)command);
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