#ifndef particleOperator_h
#define particleOperator_h


#include "particle.h"


typedef struct particleOperator {
	// This should be large enough
	// to store any type of operator.
	union {
		//
	} data;

	// This function is executed on each particle.
	void (*func)(const void *partOperator, particle *part, const float time);
} particleOperator;


#endif