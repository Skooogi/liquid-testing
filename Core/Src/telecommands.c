/*
 * telecommands.c
 *
 * This file contains functions controlling command interface of the payload. The functions keep track
 * of incoming commands from the OBC, perform the tasks needed (if any), and send the OBC a response.
 *
 */

#include <telecommands.h>
#include "main.h"

/* Creates status response packet suitable to be transmitted over the CAN bus. */
int make_status_response(/* struct bus_packet *rsp */)		// TODO: Format of bus packet?
{
	// Set response command corresponding to current status.
	// Set other fields of the struct (lenght of message, etc.)
	return 1;
}


/* Handle a command and return a response in the same buffer. Details must be discussed, how does it work with CAN?
 * Return value is 1 if there is a response, 0 if not. */
/*int handle_command(struct bus_packet *pkt, struct bus_packet *rsp)
{
	uint32_t r = 0;
	switch (pkt->cmd);
	return r;
}*/

