#ifndef state_h
#define state_h


#include <string.h>


typedef struct stateObject {
	//Vector of pointers to data for each state the object was active in.
	//We store a vector of pointers so we don't have to copy huge amounts
	//of data when we shift the arrays over.
	//Note: If states[0] is NULL, the object has most likely been removed.
	void **states;
	size_t numStates;
} stateObject;


void stateObjInit(stateObject *stateObj, const size_t stateSize);

void stateObjShift(stateObject *stateObj, const size_t stateSize, void (*copyFunc)(const void *s1, void *s2), void (*deleteFunc)(void *s));
#warning "This function is never called, and we don't handle what should happen when all the states are NULL."
void stateObjRemove(stateObject *stateObj, void (*deleteFunc)(void *s));

void stateObjDelete(stateObject *stateObj, void (*deleteFunc)(void *s));


#endif