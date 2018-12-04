#ifndef memoryDoubleList_h
#define memoryDoubleList_h


#include <stdlib.h>
#include <stdint.h>

#include "memorySettings.h"
#include "memoryShared.h"


#ifndef MEMDOUBLELIST_MEMORY_LEAN
#define MEMDOUBLELIST_BLOCK_HEADER_SIZE ((uintptr_t)memoryAlign(sizeof(void *) + sizeof(void *)))
#else
#define MEMDOUBLELIST_BLOCK_HEADER_SIZE (sizeof(void *) + sizeof(void *))
#endif

#define MEMDOUBLELIST_BLOCK_FREE_FLAG_SIZE MEMDOUBLELIST_BLOCK_HEADER_SIZE
#define MEMDOUBLELIST_BLOCK_FREE_NEXT_SIZE sizeof(void *)
#define MEMDOUBLELIST_BLOCK_USED_NEXT_SIZE sizeof(void *)
#define MEMDOUBLELIST_BLOCK_USED_PREV_SIZE (MEMDOUBLELIST_BLOCK_HEADER_SIZE - MEMDOUBLELIST_BLOCK_USED_NEXT_SIZE)

//Return the minimum block size for an element of "size" bytes.
#define memDoubleListGetBlockSize(size) (((size) > MEMDOUBLELIST_BLOCK_FREE_NEXT_SIZE) ? \
                                        (size_t)memoryAlign(MEMDOUBLELIST_BLOCK_HEADER_SIZE + (size)) : \
                                        (size_t)memoryAlign(MEMDOUBLELIST_BLOCK_HEADER_SIZE + MEMDOUBLELIST_BLOCK_FREE_NEXT_SIZE))
//Return the amount of memory required
//for "num" many blocks of "size" bytes.
#define memDoubleListMemoryForBlocks(num, size) memoryGetRequiredSize((num) * memDoubleListGetBlockSize(size))


//Block data usage diagrams:
//Occupied:   [next][prev][    data    ]
//Unoccupied: [flag][fill][next][ fill ]


typedef struct memoryDoubleList {
	size_t blockSize;
	void *nextFreeBlock;

	//This is stored at the very end of the allocated memory,
	//meaning it can be used as a pointer to the end. The
	//structure contains a pointer to the start of the region
	//as well as a pointer to the extension that follows it.
	memoryRegion *region;
} memoryDoubleList;


void *memDoubleListInit(memoryDoubleList *doubleList, void *memory, const size_t memorySize, const size_t blockSize);

void *memDoubleListAlloc(memoryDoubleList *doubleList);
void *memDoubleListPrepend(memoryDoubleList *doubleList, void **start);
void *memDoubleListAppend(memoryDoubleList *doubleList, void **start);
void *memDoubleListInsertBefore(memoryDoubleList *doubleList, void **start, void *prevData);
void *memDoubleListInsertAfter(memoryDoubleList *doubleList, void **start, void *data);

void memDoubleListFree(memoryDoubleList *doubleList, void **start, void *data);
void memDoubleListFreeInvalidate(memoryDoubleList *doubleList, memoryRegion *region, void **start, void *data);

void *memDoubleListExtend(memoryDoubleList *doubleList, memoryRegion *region, void *memory, const size_t memorySize);
void memDoubleListClearRegion(memoryDoubleList *doubleList, memoryRegion *region, uintptr_t flag);
void memDoubleListClear(memoryDoubleList *doubleList);

void memDoubleListDelete(memoryDoubleList *doubleList);


#endif