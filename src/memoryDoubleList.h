#ifndef memoryDoubleList_h
#define memoryDoubleList_h


#include <stdlib.h>
#include <stdint.h>

#include "memorySettings.h"
#include "memoryShared.h"


#define MEMDOUBLELIST_BLOCK_FREE_FLAG_SIZE MEMDOUBLELIST_BLOCK_HEADER_SIZE
#define MEMDOUBLELIST_BLOCK_FREE_NEXT_SIZE sizeof(void *)
#define MEMDOUBLELIST_BLOCK_USED_NEXT_SIZE sizeof(void *)
#define MEMDOUBLELIST_BLOCK_USED_PREV_SIZE (MEMDOUBLELIST_BLOCK_HEADER_SIZE - MEMDOUBLELIST_BLOCK_USED_NEXT_SIZE)

#ifndef MEMDOUBLELIST_MEMORY_LEAN

#define MEMDOUBLELIST_BLOCK_HEADER_SIZE ((uintptr_t)memoryAlign(sizeof(void *) + sizeof(void *)))
#define MEMDOUBLELIST_BLOCK_MIN_SIZE (MEMDOUBLELIST_BLOCK_HEADER_SIZE + MEMDOUBLELIST_BLOCK_FREE_NEXT_SIZE)
//Return the minimum block size for an element of "size" bytes.
#define memDoubleListGetBlockSize(size) ((size_t)memoryAlign( \
	((size) > MEMDOUBLELIST_BLOCK_FREE_NEXT_SIZE) ? \
	(MEMDOUBLELIST_BLOCK_HEADER_SIZE + (size)) : \
	MEMDOUBLELIST_BLOCK_MIN_SIZE \
))

#else

#define MEMDOUBLELIST_BLOCK_HEADER_SIZE (sizeof(void *) + sizeof(void *))
#define MEMDOUBLELIST_BLOCK_MIN_SIZE (MEMDOUBLELIST_BLOCK_HEADER_SIZE + MEMDOUBLELIST_BLOCK_FREE_NEXT_SIZE)
//Return the minimum block size for an element of "size" bytes.
#define memDoubleListGetBlockSize(size) ( \
	((size) > MEMDOUBLELIST_BLOCK_FREE_NEXT_SIZE) ? \
	(MEMDOUBLELIST_BLOCK_HEADER_SIZE + (size)) : \
	MEMDOUBLELIST_BLOCK_MIN_SIZE \
)

#endif


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


void *memDoubleListInit(memoryDoubleList *doubleList, void *memory, const size_t numBlocks, const size_t blockSize);

void *memDoubleListAlloc(memoryDoubleList *doubleList);
void *memDoubleListPrepend(memoryDoubleList *doubleList, void **start);
void *memDoubleListAppend(memoryDoubleList *doubleList, void **start);
void *memDoubleListInsertBefore(memoryDoubleList *doubleList, void **start, void *prevData);
void *memDoubleListInsertAfter(memoryDoubleList *doubleList, void **start, void *data);

void memDoubleListFree(memoryDoubleList *doubleList, void **start, void *data);

void memDoubleListClearRegion(memoryDoubleList *doubleList, memoryRegion *region, const byte_t flag, void *next);
void memDoubleListClearLastRegion(memoryDoubleList *doubleList, memoryRegion *region);
void memDoubleListClear(memoryDoubleList *doubleList);

void *memDoubleListExtend(memoryDoubleList *doubleList, void *memory, const size_t numBlocks);

void memDoubleListDelete(memoryDoubleList *doubleList);


#endif