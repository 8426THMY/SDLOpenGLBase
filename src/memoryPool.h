#ifndef memoryPool_h
#define memoryPool_h


#include <stdlib.h>
#include <stdint.h>

#include "settingsMemory.h"
#include "utilMemory.h"

#include "utilTypes.h"


#define MEMPOOL_BLOCK_FREE_NEXT_SIZE sizeof(void *)

#ifndef MEMPOOL_MEMORY_LEAN

#define MEMPOOL_SIZE ((size_t)memoryAlign(sizeof(memoryPool)))
#define MEMPOOL_BLOCK_HEADER_SIZE ((uintptr_t)memoryAlign(sizeof(byte_t *)))
#define MEMPOOL_BLOCK_MIN_SIZE (MEMPOOL_BLOCK_HEADER_SIZE + MEMPOOL_BLOCK_FREE_NEXT_SIZE)
// Return the minimum block size for an element of "size" bytes.
#define memPoolGetBlockSize(size) ((size_t)memoryAlign( \
	((size) > MEMPOOL_BLOCK_FREE_NEXT_SIZE) ? \
	(MEMPOOL_BLOCK_HEADER_SIZE + (size)) : \
	MEMPOOL_BLOCK_MIN_SIZE \
))

#else

#define MEMPOOL_SIZE sizeof(memoryPool)
#define MEMPOOL_BLOCK_HEADER_SIZE sizeof(byte_t *)
#define MEMPOOL_BLOCK_MIN_SIZE (MEMPOOL_BLOCK_HEADER_SIZE + MEMPOOL_BLOCK_FREE_NEXT_SIZE)
// Return the minimum block size for an element of "size" bytes.
#define memPoolGetBlockSize(size) ( \
	((size) > MEMPOOL_BLOCK_FREE_NEXT_SIZE) ? \
	(MEMPOOL_BLOCK_HEADER_SIZE + (size)) : \
	MEMPOOL_BLOCK_MIN_SIZE \
)

#endif

#define MEMPOOL_FLAG_ACTIVE   0x00
#define MEMPOOL_FLAG_INACTIVE 0x01
// This is used if there are no active elements after the block.
#define MEMPOOL_FLAG_INVALID  0x02


// Get the value of the current segment pointed to by "block".
#define memPoolBlockGetValue(block) *((void **)block)
// Get the value of the current segment pointed to by "block".
// Unlike the previous definition, this casts to a character.
#define memPoolBlockGetFlag(block) *((byte_t *)block)

// Return the address of the next block in the pool.
#define memPoolBlockGetNextBlock(block, size) memoryAddPointer(block, size)
// Return the address of the previous block in the pool.
#define memPoolBlockGetPrevBlock(block, size) memorySubPointer(block, size)

// Get the block's next pointer from its flags segment.
#define memPoolBlockFreeFlagGetNext(block) memPoolBlockFlagGetData(block)
// Get the block's flags from its next pointer segment.
#define memPoolBlockFreeNextGetFlag(block) memPoolBlockDataGetFlag(block)

// Get the block's data from its flags segment.
#define memPoolBlockFlagGetData(block) ((void **)memoryAddPointer(block, MEMPOOL_BLOCK_HEADER_SIZE))
// Get the block's flags from its data segment.
#define memPoolBlockDataGetFlag(block) ((byte_t *)memorySubPointer(block, MEMPOOL_BLOCK_HEADER_SIZE))
// Get the block's flag value from its data segment.
#define memPoolBlockDataGetFlagValue(block) memPoolBlockGetFlag(memPoolBlockDataGetFlag(block))

// Return whether or not the block is active.
#define memPoolBlockIsActive(block)   (memPoolBlockGetFlag(block) == MEMPOOL_FLAG_ACTIVE)
// Return whether or not the block is inactive.
#define memPoolBlockIsInactive(block) (memPoolBlockGetFlag(block) == MEMPOOL_FLAG_INACTIVE)
// Return whether or not the block is invalid.
#define memPoolBlockIsInvalid(block)  (memPoolBlockGetFlag(block) == MEMPOOL_FLAG_INVALID)

// Return whether or not the block is free.
#define memPoolBlockIsFree(block) !memPoolBlockIsActive(block)
// Return whether or not the block is in use.
#define memPoolBlockIsUsed(block) memPoolBlockIsActive(block)

#define memPoolRegionStart(region) memPoolBlockFreeNextGetFlag(((memoryRegion *)(region))->start)


// Return the amount of memory required
// for "num" many blocks of "size" bytes.
#define memPoolMemoryForBlocks(num, size) ((num) * memPoolGetBlockSize(size))
// Return the amount of memory required for a
// region of "num" many blocks of "size" bytes.
#define memPoolMemoryForBlocksRegion(num, size) memoryGetRequiredSize(memPoolMemoryForBlocks(num, size))


#ifdef MEMPOOL_COUNT_USED_BLOCKS
#define MEMPOOL_LOOP_BEGIN(allocator, node, type)                                       \
{                                                                                       \
	memoryRegion *allocator##_region_##node = allocator.region;                         \
	size_t allocator##_remaining_##node = allocator.usedBlocks;                         \
	type *node = (void *)memPoolBlockFreeFlagGetNext(allocator##_region_##node->start); \
	for(;;){                                                                            \
		const uintptr_t allocator##_flag_##node = memPoolBlockDataGetFlagValue(node);   \
		if(allocator##_flag_##node == MEMPOOL_FLAG_ACTIVE){                             \
			--allocator##_remaining_##node;

#define MEMPOOL_LOOP_INACTIVE(node)                                 \
		}else if(allocator##_flag_##node == MEMPOOL_FLAG_INACTIVE){ \

#define MEMPOOL_LOOP_END(allocator, node)                                                 \
		}                                                                                 \
		if(allocator##_remaining_##node <= 0){                                            \
			break;                                                                        \
		}                                                                                 \
		node = memPoolBlockGetNextBlock(node, allocator.blockSize);                       \
		if((void *)node >= (void *)allocator##_region_##node){                            \
			allocator##_region_##node = allocator##_region_##node->next;                  \
			if(allocator##_region_##node == NULL){                                        \
				break;                                                                    \
			}                                                                             \
			node = (void *)memPoolBlockFreeFlagGetNext(allocator##_region_##node->start); \
		}                                                                                 \
	}                                                                                     \
}
#else
#define MEMPOOL_LOOP_BEGIN(allocator, node, type)                                       \
{                                                                                       \
	memoryRegion *allocator##_region_##node = allocator.region;                         \
	type *node = (void *)memPoolBlockFreeFlagGetNext(allocator##_region_##node->start); \
	for(;;){                                                                            \
		const uintptr_t allocator##_flag_##node = memPoolBlockDataGetFlagValue(node);   \
		if(allocator##_flag_##node == MEMPOOL_FLAG_ACTIVE){

#define MEMPOOL_LOOP_INACTIVE(node)                                 \
		}else if(allocator##_flag_##node == MEMPOOL_FLAG_INACTIVE){ \

#define MEMPOOL_LOOP_END(allocator, node)                                                 \
		}else if(allocator##_flag_##node == MEMPOOL_FLAG_INVALID){                        \
			break;                                                                        \
		}                                                                                 \
		node = memPoolBlockGetNextBlock(node, allocator.blockSize);                       \
		if((void *)node >= (void *)allocator##_region_##node){                            \
			allocator##_region_##node = allocator##_region_##node->next;                  \
			if(allocator##_region_##node == NULL){                                        \
				break;                                                                    \
			}                                                                             \
			node = (void *)memPoolBlockFreeFlagGetNext(allocator##_region_##node->start); \
		}                                                                                 \
	}                                                                                     \
}
#endif


// Block data usage diagrams:
// Used: [active][       data       ]
// Free: [active][next][    fill    ]


#warning "Either store an array of bits (for the flags) at the beginning of the memory or don't use headers at all (blocks know whether they're active)."
typedef struct memoryPool {
	size_t blockSize;
	void *nextFreeBlock;
	#ifdef MEMPOOL_COUNT_USED_BLOCKS
	size_t usedBlocks;
	#endif

	// This is stored at the very end of the allocated memory,
	// meaning it can be used as a pointer to the end. The
	// structure contains a pointer to the start of the region
	// as well as a pointer to the extension that follows it.
	memoryRegion *region;
} memoryPool;


void *memPoolInit(memoryPool *const restrict pool, void *const restrict memory, const size_t memorySize, const size_t blockSize);

void *memPoolAlloc(memoryPool *const restrict pool);

void memPoolFree(memoryPool *const restrict pool, void *const restrict data);

void memPoolClearRegion(memoryPool *const restrict pool, memoryRegion *const restrict region, const byte_t flag, void *const restrict next);
void memPoolClearLastRegion(memoryPool *const restrict pool, memoryRegion *const restrict region);
void memPoolClear(memoryPool *const restrict pool);

#ifdef MEMORYREGION_EXTEND_ALLOCATORS
void *memPoolExtend(memoryPool *const restrict pool, void *const restrict memory, const size_t memorySize);
#endif


#endif