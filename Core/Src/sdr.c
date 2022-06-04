/*
 * sdr.c
 *
 * This file contains functions controlling the SDR payload generally on a relatively high level.
 * The functions of this file are called when a certain functionality is expected from the SDR.
 * The functions in this file rely heavily on lower level functions in other files. This means that
 * for instance controlling the ADC, filtering or other signal processing is implemented in functions
 * elsewhere. Here these lower level functions are only called based on higher level objectives.
 *
 */
#include "sdr.h"

/* Define instance of the SDR state storing struct sdr. */
struct sdr sdr;

/* Initialize the SDR payload. */
static void sdr_init()	// TODO: Any parameters?
{
	// TODO: Initialize!
}


/* Change operation mode of the SDR */
void sdr_change_mode(int new_mode)
{
	// TODO: Change mode!
}


// TODO: All other nice functions
