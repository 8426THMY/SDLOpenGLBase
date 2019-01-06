#ifndef memoryStack_h
#define memoryStack_h


#include <stdlib.h>

#include "settingsMemory.h"
#include "memoryShared.h"


//Example stack diagram:
//                                   top
//                                    |
//                                    v
//[    data    ][size][  data  ][size][        free        ]


typedef struct memoryStack {
	void *top;
	size_t size;
	void *bottom;
} memoryStack;


void *memStackInit(memoryStack *stack, void *memory, const size_t stackSize);

void *memStackAlloc(memoryStack *stack, const size_t blockSize);
void memStackFreeLast(memoryStack *stack);

void memStackDelete(memoryStack *stack);


#endif