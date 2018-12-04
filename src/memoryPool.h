#ifndef memoryPool_h
#define memoryPool_h


#include <stdlib.h>
#include <stdint.h>

#include "memorySettings.h"
#include "memoryShared.h"

#include "utilTypes.h"


#ifndef MEMPOOL_MEMORY_LEAN
#define MEMPOOL_BLOCK_HEADER_SIZE ((uintptr_t)memoryAlign(sizeof(byte_t *)))
#else
#define MEMPOOL_BLOCK_HEADER_SIZE sizeof(byte_t *)
#endif

#define MEMPOOL_BLOCK_FLAG_SIZE MEMPOOL_BLOCK_HEADER_SIZE
#define MEMPOOL_BLOCK_FREE_NEXT_SIZE sizeof(void *)

//Return the minimum block size for an element of "size" bytes.
#define memPoolGetBlockSize(size) (((size) > MEMPOOL_BLOCK_FREE_NEXT_SIZE) ? \
                                  (size_t)memoryAlign(MEMPOOL_BLOCK_HEADER_SIZE + (size)) : \
                                  (size_t)memoryAlign(MEMPOOL_BLOCK_HEADER_SIZE + MEMPOOL_BLOCK_FREE_NEXT_SIZE))
//Return the amount of memory required
//for "num" many blocks of "size" bytes.
#define memPoolMemoryForBlocks(num, size) memoryGetRequiredSize((num) * memPoolGetBlockSize(size))


//Block data usage diagrams:
//Occupied:   [active][       data       ]
//Unoccupied: [active][next][    fill    ]


typedef struct memoryPool {
	size_t blockSize;
	void *nextFreeBlock;

	//This is stored at the very end of the allocated memory,
	//meaning it can be used as a pointer to the end. The
	//structure contains a pointer to the start of the region
	//as well as a pointer to the extension that follows it.
	memoryRegion *region;
} memoryPool;


void *memPoolInit(memoryPool *pool, void *memory, const size_t memorySize, const size_t blockSize);

void *memPoolAlloc(memoryPool *pool);

void memPoolFree(memoryPool *pool, void *data);
void memPoolFreeInvalidate(memoryPool *pool, memoryRegion *region, void *data);

void *memPoolExtend(memoryPool *pool, memoryRegion *region, void *memory, const size_t memorySize);
void memPoolClearRegion(memoryPool *pool, memoryRegion *region, byte_t flag);
void memPoolClear(memoryPool *pool);

void memPoolDelete(memoryPool *pool);


#endif