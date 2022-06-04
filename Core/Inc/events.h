/*
 * events.h
 *
 */

#ifndef INC_EVENTS_H_
#define INC_EVENTS_H_

/* This struct containts event flags that keep track of the status of the system. */
struct eventflags {
	/* Set when any event happens. Tells the main loop not to sleep. */
	char dont_sleep;
	/* Event flags for each part of the software */
	char tick, bus;		// TODO: Add flags when needed.
};

extern volatile struct eventflags eventflags;

#define SET_EVENT(e) { eventflags.e = 1; eventflags.dont_sleep = 1; }


#endif /* INC_EVENTS_H_ */
