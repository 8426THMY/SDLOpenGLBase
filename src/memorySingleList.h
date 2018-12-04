#ifndef memorySingleList_h
#define memorySingleList_h


#include <stdlib.h>
#include <stdint.h>

#include "memorySettings.h"
#include "memoryShared.h"


#ifndef MEMSINGLELIST_MEMORY_LEAN
#define MEMSINGLELIST_BLOCK_HEADER_SIZE ((uintptr_t)memoryAlign(sizeof(void *)))
#else
#define MEMSINGLELIST_BLOCK_HEADER_SIZE sizeof(void *)
#endif

#define MEMSINGLELIST_BLOCK_FREE_FLAG_SIZE MEMSINGLELIST_BLOCK_HEADER_SIZE
#define MEMSINGLELIST_BLOCK_FREE_NEXT_SIZE sizeof(void *)
#define MEMSINGLELIST_BLOCK_USED_NEXT_SIZE MEMSINGLELIST_BLOCK_HEADER_SIZE

//Return the minimum block size for an element of "size" bytes.
#define memSingleListGetBlockSize(size) (((size) > MEMSINGLELIST_BLOCK_FREE_NEXT_SIZE) ? \
                                        (size_t)memoryAlign(MEMSINGLELIST_BLOCK_HEADER_SIZE + (size)) : \
                                        (size_t)memoryAlign(MEMSINGLELIST_BLOCK_HEADER_SIZE + MEMSINGLELIST_BLOCK_FREE_NEXT_SIZE))
//Return the amount of memory required
//for "num" many blocks of "size" bytes.
#define memSingleListMemoryForBlocks(num, size) memoryGetRequiredSize((num) * memSingleListGetBlockSize(size))


//Block data usage diagrams:
//Occupied:   [next][    data    ]
//Unoccupied: [flag][next][ fill ]


typedef struct memorySingleList {
	size_t blockSize;
	void *nextFreeBlock;

	//This is stored at the very end of the allocated memory,
	//meaning it can be used as a pointer to the end. The
	//structure contains a pointer to the start of the region
	//as well as a pointer to the extension that follows it.
	memoryRegion *region;
} memorySingleList;


void *memSingleListInit(memorySingleList *singleList, void *memory, const size_t memorySize, const size_t blockSize);

void *memSingleListAlloc(memorySingleList *singleList);
void *memSingleListPrepend(memorySingleList *singleList, void **start);
void *memSingleListAppend(memorySingleList *singleList, void **start);
void *memSingleListInsertBefore(memorySingleList *singleList, void **start, void *prevData);
void *memSingleListInsertAfter(memorySingleList *singleList, void **start, void *data);

void memSingleListFree(memorySingleList *singleList, void **start, void *data, void *prevData);
void memSingleListFreeInvalidate(memorySingleList *singleList, memoryRegion *region, void **start, void *data, void *prevData);

void *memSingleListExtend(memorySingleList *singleList, memoryRegion *region, void *memory, const size_t memorySize);
void memSingleListClearRegion(memorySingleList *singleList, memoryRegion *region, uintptr_t flag);
void memSingleListClear(memorySingleList *singleList);

void memSingleListDelete(memorySingleList *singleList);


#endif