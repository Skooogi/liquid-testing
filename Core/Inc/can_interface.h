/*
 * cubesat_protocol.h
 *
 */

#ifndef CAN_INTERFACE_H 
#define CAN_INTERFACE_H

#include "stm32h7xx_hal.h"

extern FDCAN_RxHeaderTypeDef RxHeader;
extern FDCAN_TxHeaderTypeDef TxHeader;
extern FDCAN_HandleTypeDef hfdcan1;

void canTXTask( void *pvParameters );		// Task for handling the CubeSat Protocol, i.e the command interface of the payload

#endif 
