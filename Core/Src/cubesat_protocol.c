/*
 * cubesat_protocol.c
 *
 */

#include "cubesat_protocol.h"

#include <csp/csp.h>
#include <csp/interfaces/csp_if_can.h>

/* test mode, used for verifying that host & client can exchange packets over the loopback interface */
void csp_reboot_hook(void) {
	//TODO reboot
}
void csp_shutdown_hook(void) {
	//TODO shutdown
}

/* Task for handling the CubeSat Protocol, i.e the command interface of the payload */
/*void canRXTask( void* param) {

}
*/
