#ifndef memoryFreeList_h
#define memoryFreeList_h


#include <stdlib.h>
#include <stdint.h>

#include "settingsMemory.h"
#include "utilMemory.h"


#ifndef MEMFREELIST_MEMORY_LEAN

#define MEMFREELIST_BLOCK_HEADER_SIZE ((uintptr_t)memoryAlign(sizeof(void *)))
// Return the minimum block size for an element of "size" bytes.
#define memFreeListGetBlockSize(size) ((size_t)memoryAlign( \
	((size) > MEMFREELIST_BLOCK_HEADER_SIZE) ? \
	(MEMFREELIST_BLOCK_HEADER_SIZE + (size)) : \
	MEMFREELIST_BLOCK_HEADER_SIZE \
))

#else

#define MEMFREELIST_BLOCK_HEADER_SIZE sizeof(void *)
// Return the minimum block size for an element of "size" bytes.
#define memFreeListGetBlockSize(size) ( \
	((size) > MEMFREELIST_BLOCK_HEADER_SIZE) ? \
	(MEMFREELIST_BLOCK_HEADER_SIZE + (size)) : \
	MEMFREELIST_BLOCK_HEADER_SIZE \
)

#endif


// Get the block after the one specified.
#define memFreeListBlockGetNextBlock(block, size) memoryAddPointer(block, size)
// Get the block before the one specified.
#define memFreeListBlockGetPrevBlock(block, size) memorySubPointer(block, size)

// Get the next pointer of a free block.
#define memFreeListBlockFreeGetNext(block) *((void **)block)

#define memFreeListRegionStart(region) (((memoryRegion *)(region))->start)


// Return the amount of memory required
// for "num" many blocks of "size" bytes.
#define memFreeListMemoryForBlocks(num, size) ((num) * memFreeListGetBlockSize(size))
// Return the amount of memory required for a
// region of "num" many blocks of "size" bytes.
#define memFreeListMemoryForBlocksRegion(num, size) memoryGetRequiredSize(memFreeListMemoryForBlocks(num, size))


#define MEMFREELIST_LOOP_BEGIN(allocator, node, type)           \
{                                                               \
	memoryRegion *allocator##_region_##node = allocator.region; \
	type *node = (void *)(allocator##_region_##node->start);    \
	for(;;){                                                    \

#define MEMFREELIST_LOOP_END(allocator, node)                            \
		node = memFreeListBlockGetNextBlock(node, allocator.blockSize);  \
		if((void *)node >= (void *)allocator##_region_##node){           \
			allocator##_region_##node = allocator##_region_##node->next; \
			if(allocator##_region_##node == NULL){                       \
				break;                                                   \
			}                                                            \
			node = (void *)allocator##_region_##node->start;             \
		}                                                                \
	}                                                                    \
}


// Block data usage diagrams:
// Used: [          data          ]
// Free: [next][       fill       ]


typedef struct memoryFreeList {
	size_t blockSize;
	// Points to the next pointer of a free block.
	void *nextFreeBlock;
	#ifdef MEMFREELIST_COUNT_USED_BLOCKS
	size_t usedBlocks;
	#endif

	// This is stored at the very end of the allocated memory,
	// meaning it can be used as a pointer to the end. The
	// structure contains a pointer to the start of the region
	// as well as a pointer to the extension that follows it.
	memoryRegion *region;
} memoryFreeList;


void *memFreeListInit(memoryFreeList *const restrict freeList, void *const restrict memory, const size_t memorySize, const size_t blockSize);

void *memFreeListAlloc(memoryFreeList *const restrict freeList);

void memFreeListFree(memoryFreeList *const restrict freeList, void *const restrict data);

void memFreeListClearRegion(memoryFreeList *const restrict freeList, memoryRegion *const restrict region, void *const restrict next);
void memFreeListClearLastRegion(memoryFreeList *const restrict freeList, memoryRegion *const restrict region);
void memFreeListClear(memoryFreeList *const restrict freeList);

#ifdef MEMORYREGION_EXTEND_ALLOCATORS
void *memFreeListExtend(memoryFreeList *const restrict freeList, void *memory, const size_t memorySize);
#endif

void memFreeListDelete(memoryFreeList *const restrict freeList, void (*freeFunc)(void *block));


#endif