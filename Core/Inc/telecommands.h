/*
 * telecommands.h
 *
 */

#ifndef INC_TELECOMMANDS_H_
#define INC_TELECOMMANDS_H_

/* Subsystem-specific command handler.
 * Return 1 if there is a response, 0 if not. */
int handle_command(/*struct bus_packet *pkt, struct bus_packet *rsp*/);

#endif /* INC_TELECOMMANDS_H_ */
