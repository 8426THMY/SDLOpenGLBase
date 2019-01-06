#ifndef memoryPool_h
#define memoryPool_h


#include <stdlib.h>
#include <stdint.h>

#include "settingsMemory.h"
#include "memoryShared.h"

#include "utilTypes.h"


#define MEMPOOL_BLOCK_FREE_NEXT_SIZE sizeof(void *)

#ifndef MEMPOOL_MEMORY_LEAN

#define MEMPOOL_SIZE ((size_t)memoryAlign(sizeof(memoryPool)))
#define MEMPOOL_BLOCK_HEADER_SIZE ((uintptr_t)memoryAlign(sizeof(byte_t *)))
#define MEMPOOL_BLOCK_MIN_SIZE (MEMPOOL_BLOCK_HEADER_SIZE + MEMPOOL_BLOCK_FREE_NEXT_SIZE)
//Return the minimum block size for an element of "size" bytes.
#define memPoolGetBlockSize(size) ((size_t)memoryAlign( \
	((size) > MEMPOOL_BLOCK_FREE_NEXT_SIZE) ? \
	(MEMPOOL_BLOCK_HEADER_SIZE + (size)) : \
	MEMPOOL_BLOCK_MIN_SIZE \
))

#else

#define MEMPOOL_SIZE sizeof(memoryPool)
#define MEMPOOL_BLOCK_HEADER_SIZE sizeof(byte_t *)
#define MEMPOOL_BLOCK_MIN_SIZE (MEMPOOL_BLOCK_HEADER_SIZE + MEMPOOL_BLOCK_FREE_NEXT_SIZE)
//Return the minimum block size for an element of "size" bytes.
#define memPoolGetBlockSize(size) ( \
	((size) > MEMPOOL_BLOCK_FREE_NEXT_SIZE) ? \
	(MEMPOOL_BLOCK_HEADER_SIZE + (size)) : \
	MEMPOOL_BLOCK_MIN_SIZE \
)

#endif

#define MEMPOOL_FLAG_ACTIVE   0x00
#define MEMPOOL_FLAG_INACTIVE 0x01
//This is used if there are no active elements after the block.
#define MEMPOOL_FLAG_INVALID  0x02


//Get the value of the current segment pointed to by "block".
#define memPoolBlockGetValue(block) *((void **)block)
//Get the value of the current segment pointed to by "block".
//Unlike the previous definition, this casts to a character.
#define memPoolBlockGetFlag(block) *((byte_t *)block)

//Return the address of the next block in the pool.
#define memPoolBlockGetNextBlock(block, size) memoryAddPointer(block, size)
//Return the address of the previous block in the pool.
#define memPoolBlockGetPrevBlock(block, size) memorySubPointer(block, size)

//Get the block's data from its flags segment.
#define memPoolBlockFlagGetData(block) ((void **)memoryAddPointer(block, MEMPOOL_BLOCK_HEADER_SIZE))
//Get the block's flags from its data segment.
#define memPoolBlockDataGetFlag(block) ((byte_t *)memorySubPointer(block, MEMPOOL_BLOCK_HEADER_SIZE))

//Return whether or not the block is active.
#define memPoolBlockIsActive(block)   (memPoolBlockGetFlag(block) == MEMPOOL_FLAG_ACTIVE)
//Return whether or not the block is inactive.
#define memPoolBlockIsInactive(block) (memPoolBlockGetFlag(block) == MEMPOOL_FLAG_INACTIVE)
//Return whether or not the block is invalid.
#define memPoolBlockIsInvalid(block)  (memPoolBlockGetFlag(block) == MEMPOOL_FLAG_INVALID)

//Return whether or not the block is free.
#define memPoolBlockIsFree(block) !memPoolBlockIsActive(block)
//Return whether or not the block is in use.
#define memPoolBlockIsUsed(block) memPoolBlockIsActive(block)


//Return the amount of memory required
//for "num" many blocks of "size" bytes.
#define memPoolMemoryForBlocks(num, size) memoryGetRequiredSize((num) * memPoolGetBlockSize(size))


//Block data usage diagrams:
//Occupied:   [active][       data       ]
//Unoccupied: [active][next][    fill    ]


#warning "Either store an array of bits (for the flags) at the beginning of the memory or don't use headers at all (blocks know whether they're active)."
typedef struct memoryPool {
	size_t blockSize;
	void *nextFreeBlock;

	//This is stored at the very end of the allocated memory,
	//meaning it can be used as a pointer to the end. The
	//structure contains a pointer to the start of the region
	//as well as a pointer to the extension that follows it.
	memoryRegion *region;
} memoryPool;


void *memPoolInit(memoryPool *pool, void *memory, const size_t numBlocks, const size_t blockSize);

void *memPoolAlloc(memoryPool *pool);

void memPoolFree(memoryPool *pool, void *data);

void memPoolClearRegion(memoryPool *pool, memoryRegion *region, const byte_t flag, void *next);
void memPoolClearLastRegion(memoryPool *pool, memoryRegion *region);
void memPoolClear(memoryPool *pool);

void *memPoolExtend(memoryPool *pool, void *memory, const size_t numBlocks);

void memPoolDelete(memoryPool *pool);


#endif