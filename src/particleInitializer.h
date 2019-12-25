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
	void (*func)(const void *initializer, particle *part);
} particleInitializer;


void particleInitializerRandomPosSphere(const void *initializer, particle *part);


#endif