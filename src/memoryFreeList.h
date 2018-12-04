#ifndef memoryFreeList_h
#define memoryFreeList_h


#include <stdlib.h>
#include <stdint.h>

#include "memorySettings.h"
#include "memoryShared.h"


#ifndef MEMFREELIST_MEMORY_LEAN
#define MEMFREELIST_BLOCK_HEADER_SIZE ((uintptr_t)memoryAlign(sizeof(void *)))
#else
#define MEMFREELIST_BLOCK_HEADER_SIZE sizeof(void *)
#endif

//Return the minimum block size for an element of "size" bytes.
#define memFreeListGetBlockSize(size) (((size) > MEMFREELIST_BLOCK_HEADER_SIZE) ? \
                                      (size_t)memoryAlign(MEMFREELIST_BLOCK_HEADER_SIZE + (size)) : \
                                      (size_t)memoryAlign(MEMFREELIST_BLOCK_HEADER_SIZE))
//Return the amount of memory required
//for "num" many blocks of "size" bytes.
#define memFreeListMemoryForBlocks(num, size) memoryGetRequiredSize((num) * memFreeListGetBlockSize(size))


//Block data usage diagrams:
//Occupied:   [          data          ]
//Unoccupied: [next][       fill       ]


typedef struct memoryFreeList {
	size_t blockSize;
	void *nextFreeBlock;

	//This is stored at the very end of the allocated memory,
	//meaning it can be used as a pointer to the end. The
	//structure contains a pointer to the start of the region
	//as well as a pointer to the extension that follows it.
	memoryRegion *region;
} memoryFreeList;


void *memFreeListInit(memoryFreeList *freeList, void *memory, const size_t memorySize, const size_t blockSize);

void *memFreeListAlloc(memoryFreeList *freeList);

void memFreeListFree(memoryFreeList *freeList, void *data);

void *memFreeListExtend(memoryFreeList *freeList, memoryRegion *region, void *memory, const size_t memorySize);
void memFreeListClearRegion(memoryFreeList *freeList, memoryRegion *region);
void memFreeListClear(memoryFreeList *freeList);

void memFreeListDelete(memoryFreeList *freeList);


#endif