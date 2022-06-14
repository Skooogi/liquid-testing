/*
 * cubesat_protocol.c
 *
 */

#include "cubesat_protocol.h"

#include <csp/csp.h>
#include <csp/drivers/can_socketcan.h>
#include <csp/interfaces/csp_if_can.h>

// Can pins PB8 PB9

void initCSP() {
	
}

/* Task for handling the CubeSat Protocol, i.e the command interface of the payload */
void prvCSPTask( void *pvParameters ) {
}

