#ifndef particleInitializer_h
#define particleInitializer_h


#include "particle.h"


typedef struct particleInitializer {
	// This should be large enough
	// to store any type of initializer.
	union {
		//
	} data;

	// This function is executed on each particle.
	void (*func)(const void *const restrict initializer, particle *const restrict part);
} particleInitializer;


void particleInitializerRandomPosSphere(const void *const restrict initializer, particle *const restrict part);


#endif