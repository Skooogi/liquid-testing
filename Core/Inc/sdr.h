/*
 * sdr.h
 *
 */

#ifndef INC_SDR_H_
#define INC_SDR_H_

#include <stdint.h>


/* Declare the extern struct sdr. */
typedef struct sdr *sdr_t;
extern struct sdr sdr;

/* A struct to contain the state of the SDR. */
struct sdr {
	enum { SDR_STANDY, SDR_MEASURE } sdr_mode;		// Operation modes of the SDR.
};


/************* Publicly callable functions *************/
void sdr_change_mode(int new_mode);			// Change the operation mode of the SDR


#endif /* INC_SDR_H_ */

