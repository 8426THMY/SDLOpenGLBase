#ifndef particleConstraint_h
#define particleConstraint_h


#include "particle.h"


typedef struct particleConstraint {
	// This should be large enough
	// to store any type of initializer.
	union {
		//
	} data;

	// This function is executed on each particle.
	void (*func)(const void *partConstraint, particle *part, const float time);
} particleConstraint;


#endif