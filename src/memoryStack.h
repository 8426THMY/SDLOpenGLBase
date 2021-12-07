#ifndef memoryStack_h
#define memoryStack_h


#include <stdlib.h>

#include "settingsMemory.h"
#include "utilMemory.h"


// This is a nice abstraction for when we need to free a stack.
#define memStackMemory(stack) ((stack)->bottom)


// Example stack diagram:
//                                    top
//                                     |
//                                     v
// [    data    ][size][  data  ][size][        free        ]


typedef struct memoryStack {
	void *top;
	size_t size;
	void *bottom;
} memoryStack;


void *memStackInit(memoryStack *const restrict stack, void *const restrict memory, const size_t stackSize);

void *memStackAlloc(memoryStack *const restrict stack, const size_t blockSize);
void memStackFreeLast(memoryStack *const restrict stack);


#endif