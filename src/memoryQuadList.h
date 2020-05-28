#ifndef memoryQuadList_h
#define memoryQuadList_h


#include <stdlib.h>
#include <stdint.h>

#include "utilTypes.h"

#include "settingsMemory.h"
#include "utilMemory.h"


#define MEMQUADLIST_BLOCK_FREE_FLAG_SIZE  MEMQUADLIST_BLOCK_HEADER_SIZE
#define MEMQUADLIST_BLOCK_FREE_NEXT_SIZE  sizeof(void *)
#define MEMQUADLIST_BLOCK_USED_NEXT_SIZE  sizeof(void *)
#define MEMQUADLIST_BLOCK_USED_PREV_SIZE  sizeof(void *)
#define MEMQUADLIST_BLOCK_USED_LIST_SIZE  (MEMQUADLIST_BLOCK_USED_NEXT_SIZE + MEMQUADLIST_BLOCK_USED_PREV_SIZE)
// Length from "nextA" to "prevB".
#define MEMQUADLIST_BLOCK_USED_END_LENGTH (MEMQUADLIST_BLOCK_USED_NEXT_SIZE + MEMQUADLIST_BLOCK_USED_PREV_SIZE + MEMQUADLIST_BLOCK_USED_NEXT_SIZE)

#ifndef MEMQUADLIST_MEMORY_LEAN

#define MEMQUADLIST_BLOCK_HEADER_SIZE ((size_t)memoryAlign(sizeof(void *) + sizeof(void *) + sizeof(void *) + sizeof(void *)))
#define MEMQUADLIST_BLOCK_MIN_SIZE MEMQUADLIST_BLOCK_HEADER_SIZE
// Return the minimum block size for an element of "size" bytes.
#define memQuadListGetBlockSize(size) ((size_t)memoryAlign( \
	((size) > MEMQUADLIST_BLOCK_FREE_NEXT_SIZE) ? \
	(MEMQUADLIST_BLOCK_HEADER_SIZE + (size)) : \
	MEMQUADLIST_BLOCK_MIN_SIZE \
))

#else

#define MEMQUADLIST_BLOCK_HEADER_SIZE (sizeof(void *) + sizeof(void *) + sizeof(void *) + sizeof(void *))
#define MEMQUADLIST_BLOCK_MIN_SIZE MEMQUADLIST_BLOCK_HEADER_SIZE
// Return the minimum block size for an element of "size" bytes.
#define memQuadListGetBlockSize(size) ( \
	((size) > MEMQUADLIST_BLOCK_FREE_NEXT_SIZE) ? \
	(MEMQUADLIST_BLOCK_HEADER_SIZE + (size)) : \
	MEMQUADLIST_BLOCK_MIN_SIZE \
)

#endif

// We define the active flag as "0x00" so
// it's easier to loop through sub-arrays.
#define MEMQUADLIST_FLAG_ACTIVE   0x00
#define MEMQUADLIST_FLAG_INACTIVE 0x01
// This is used if there are no active elements after the block.
#define MEMQUADLIST_FLAG_INVALID  0x02


// Get the value of the current segment pointed to by "block".
#define memQuadListBlockGetValue(block) *((void **)block)
// Get the value of the current segment pointed to by "block".
// Unlike the previous definition, this casts to a unsigned integer.
#define memQuadListBlockFreeGetFlag(block) *((uintptr_t *)block)

// We'll need to remove the active flag from
// the pointer if we want to get its real value.
#define memQuadListBlockGetNext(next) ((void *)(((uintptr_t)memQuadListBlockGetValue(next)) & MEMORY_DATA_MASK))
// The active flag is stored in the
// least significant bit of the pointer.
#define memQuadListBlockGetFlag(next) (((uintptr_t)memQuadListBlockGetValue(next)) & MEMORY_FLAG_MASK)

// Return the address of the next block in the list.
#define memQuadListBlockGetNextBlock(block, size) memoryAddPointer(block, size)
// Return the address of the previous block in the list.
#define memQuadListBlockGetPrevBlock(block, size) memorySubPointer(block, size)

// Get the block's next pointer from its flags segment.
#define memQuadListBlockFreeFlagGetNext(block) ((void **)memoryAddPointer(block, MEMQUADLIST_BLOCK_HEADER_SIZE))
// Get the block's flags from its next pointer segment.
#define memQuadListBlockFreeNextGetFlag(block) ((uintptr_t *)memorySubPointer(block, MEMQUADLIST_BLOCK_HEADER_SIZE))

// Get the block's first previous pointer from its first next pointer segment.
#define memQuadListBlockUsedNextAGetPrevA(block) ((void **)memoryAddPointer(block, MEMQUADLIST_BLOCK_USED_NEXT_SIZE))
// Get the block's second next pointer from its first next pointer segment.
#define memQuadListBlockUsedNextAGetNextB(block) ((void **)memoryAddPointer(block, MEMQUADLIST_BLOCK_USED_LIST_SIZE))
// Get the block's second previous pointer from its first next pointer segment.
#define memQuadListBlockUsedNextAGetPrevB(block) ((void **)memoryAddPointer(block, MEMQUADLIST_BLOCK_USED_END_LENGTH))
// Get the block's data from its first next pointer segment.
#define memQuadListBlockUsedNextAGetData(block) ((void **)memoryAddPointer(block, MEMQUADLIST_BLOCK_HEADER_SIZE))

// Get the block's first next pointer from its first previous pointer segment.
#define memQuadListBlockUsedPrevAGetNextA(block) ((void **)memorySubPointer(block, MEMQUADLIST_BLOCK_USED_NEXT_SIZE))
// Get the block's second next pointer from its first previous pointer segment.
#define memQuadListBlockUsedPrevAGetNextB(block) ((void **)memoryAddPointer(block, MEMQUADLIST_BLOCK_USED_PREV_SIZE))
// Get the block's second previous pointer from its first previous pointer segment.
#define memQuadListBlockUsedPrevAGetPrevB(block) ((void **)memoryAddPointer(block, MEMQUADLIST_BLOCK_USED_LIST_SIZE))
// Get the block's data from its first previous pointer segment.
#define memQuadListBlockUsedPrevAGetData(block) ((void **)memoryAddPointer(block, MEMQUADLIST_BLOCK_USED_END_LENGTH))

// Get the block's first next pointer from its second next pointer segment.
#define memQuadListBlockUsedNextBGetNextA(block) ((void **)memorySubPointer(block, MEMQUADLIST_BLOCK_USED_LIST_SIZE))
// Get the block's first previous pointer from its second next pointer segment.
#define memQuadListBlockUsedNextBGetPrevA(block) ((void **)memorySubPointer(block, MEMQUADLIST_BLOCK_USED_PREV_SIZE))
// Get the block's second previous pointer from its second next pointer segment.
#define memQuadListBlockUsedNextBGetPrevB(block) ((void **)memoryAddPointer(block, MEMQUADLIST_BLOCK_USED_NEXT_SIZE))
// Get the block's data from its second next pointer segment.
#define memQuadListBlockUsedNextBGetData(block) ((void **)memoryAddPointer(block, MEMQUADLIST_BLOCK_USED_LIST_SIZE))

// Get the block's first next pointer from its second previous pointer segment.
#define memQuadListBlockUsedPrevBGetNextA(block) ((void **)memorySubPointer(block, MEMQUADLIST_BLOCK_USED_END_LENGTH))
// Get the block's first previous pointer from its second previous pointer segment.
#define memQuadListBlockUsedPrevBGetPrevA(block) ((void **)memorySubPointer(block, MEMQUADLIST_BLOCK_USED_LIST_SIZE))
// Get the block's second next pointer from its second previous pointer segment.
#define memQuadListBlockUsedPrevBGetNextB(block) ((void **)memorySubPointer(block, MEMQUADLIST_BLOCK_USED_NEXT_SIZE))
// Get the block's data from its second previous pointer segment.
#define memQuadListBlockUsedPrevBGetData(block) ((void **)memoryAddPointer(block, MEMQUADLIST_BLOCK_USED_PREV_SIZE))

// Get the block's first next pointer from its data segment.
#define memQuadListBlockUsedDataGetNextA(block) ((void **)memorySubPointer(block, MEMQUADLIST_BLOCK_HEADER_SIZE))
// Get the block's first previous pointer from its data segment.
#define memQuadListBlockUsedDataGetPrevA(block) ((void **)memorySubPointer(block, MEMQUADLIST_BLOCK_USED_END_LENGTH))
// Get the block's second next pointer from its data segment.
#define memQuadListBlockUsedDataGetNextB(block) ((void **)memorySubPointer(block, MEMQUADLIST_BLOCK_USED_LIST_SIZE))
// Get the block's second previous pointer from its data segment.
#define memQuadListBlockUsedDataGetPrevB(block) ((void **)memorySubPointer(block, MEMQUADLIST_BLOCK_USED_PREV_SIZE))

// Get the block's flag value from its data segment.
#define memDoubleListBlockUsedDataGetFlagValue(block) memQuadListBlockGetFlag(memQuadListBlockUsedDataGetNextA(block))

// Return whether or not the block is active.
#define memQuadListBlockIsActive(block) (memQuadListBlockGetFlag(block) == MEMQUADLIST_FLAG_ACTIVE)
// Return whether or not the block is inactive.
#define memQuadListBlockIsInactive(block) (memQuadListBlockFreeGetFlag(block) == MEMQUADLIST_FLAG_INACTIVE)
// Return whether or not the block is invalid.
#define memQuadListBlockIsInvalid(block) (memQuadListBlockFreeGetFlag(block) == MEMQUADLIST_FLAG_INVALID)

// Return whether or not the block is free.
#define memQuadListBlockIsFree(block) !memQuadListBlockIsActive(block)
// Return whether or not the block is in use.
#define memQuadListBlockIsUsed(block) memQuadListBlockIsActive(block)

// Get the first next element in a an array list. Active elements
// have no flags, so we can simply dereference the next pointer.
#define memQuadListNextA(element) memQuadListBlockGetValue(memQuadListBlockUsedDataGetNextA(element))
// Get the first previous element in a an array list. Active elements
// have no flags, so we can simply dereference the next pointer.
#define memQuadListPrevA(element) memQuadListBlockGetValue(memQuadListBlockUsedDataGetPrevA(element))
// Get the second next element in a an array list. Active elements
// have no flags, so we can simply dereference the next pointer.
#define memQuadListNextB(element) memQuadListBlockGetValue(memQuadListBlockUsedDataGetNextB(element))
// Get the second previous element in a an array list. Active elements
// have no flags, so we can simply dereference the next pointer.
#define memQuadListPrevB(element) memQuadListBlockGetValue(memQuadListBlockUsedDataGetPrevB(element))

#define memQuadListRegionStart(region) memQuadListBlockFreeNextGetFlag(((memoryRegion *)(region))->start)


// Return the amount of memory required
// for "num" many blocks of "size" bytes.
#define memQuadListMemoryForBlocks(num, size) ((num) * memQuadListGetBlockSize(size))
// Return the amount of memory required for a
// region of "num" many blocks of "size" bytes.
#define memQuadListMemoryForBlocksRegion(num, size) memoryGetRequiredSize(memQuadListMemoryForBlocks(num, size))


#ifdef MEMQUADLIST_COUNT_USED_BLOCKS
#define MEMQUADLIST_LOOP_BEGIN(allocator, node, type)                                         \
{                                                                                             \
	memoryRegion *allocator##_region_##node = allocator.region;                               \
	size_t allocator##_remaining_##node = allocator.usedBlocks;                               \
	type *node = (void *)(allocator##_region_##node->start);                                  \
	for(;;){                                                                                  \
		const uintptr_t allocator##_flag_##node = memQuadListBlockUsedDataGetFlagValue(node); \
		if(allocator##_flag_##node == MEMQUADLIST_FLAG_ACTIVE){                               \
			--allocator##_remaining_##node;

#define MEMQUADLIST_LOOP_INACTIVE(node)                                 \
		}else if(allocator##_flag_##node == MEMQUADLIST_FLAG_INACTIVE){ \

#define MEMQUADLIST_LOOP_END(allocator, node)                            \
		}                                                                \
		if(allocator##_remaining_##node <= 0){                           \
			break;                                                       \
		}                                                                \
		node = memQuadListBlockGetNextBlock(node, allocator.blockSize);  \
		if((void *)node >= (void *)allocator##_region_##node){           \
			allocator##_region_##node = allocator##_region_##node->next; \
			if(allocator##_region_##node == NULL){                       \
				break;                                                   \
			}                                                            \
			node = (void *)allocator##_region_##node->start;             \
		}                                                                \
	}                                                                    \
}
#else
#define MEMQUADLIST_LOOP_BEGIN(allocator, node, type)                                         \
{                                                                                             \
	memoryRegion *allocator##_region_##node = allocator.region;                               \
	type *node = (void *)(allocator##_region_##node->start);                                  \
	for(;;){                                                                                  \
		const uintptr_t allocator##_flag_##node = memQuadListBlockUsedDataGetFlagValue(node); \
		if(allocator##_flag_##node == MEMQUADLIST_FLAG_ACTIVE){

#define MEMQUADLIST_LOOP_INACTIVE(node)                                 \
		}else if(allocator##_flag_##node == MEMQUADLIST_FLAG_INACTIVE){ \

#define MEMQUADLIST_LOOP_END(allocator, node)                            \
		}else if(allocator##_flag_##node == MEMQUADLIST_FLAG_INVALID){   \
			break;                                                       \
		}                                                                \
		node = memQuadListBlockGetNextBlock(node, allocator.blockSize);  \
		if((void *)node >= (void *)allocator##_region_##node){           \
			allocator##_region_##node = allocator##_region_##node->next; \
			if(allocator##_region_##node == NULL){                       \
				break;                                                   \
			}                                                            \
			node = (void *)allocator##_region_##node->start;             \
		}                                                                \
	}                                                                    \
}
#endif


// Block data usage diagrams:
// Used: [nextA][prevA][nextB][prevB][    data    ]
// Free: [next + flag][            fill           ]

// For used blocks, the next pointer points to the next block's data segment.
// For free blocks, the next pointer points to the next block's next pointer.


/*
** This allocator aims to be something of a quadruply-linked list.
** Each node can have two parents and two children. This is generally
** used if we want to have a single object that has two owners.
*/
typedef struct memoryQuadList {
	size_t blockSize;
	// Points to the next pointer of a free block.
	void *nextFreeBlock;
	#ifdef MEMQUADLIST_COUNT_USED_BLOCKS
	size_t usedBlocks;
	#endif

	// This is stored at the very end of the allocated memory,
	// meaning it can be used as a pointer to the end. The
	// structure contains a pointer to the start of the region
	// as well as a pointer to the extension that follows it.
	memoryRegion *region;
} memoryQuadList;


void *memQuadListInit(memoryQuadList *const restrict quadList, void *const restrict memory, const size_t memorySize, const size_t blockSize);

void *memQuadListAlloc(memoryQuadList *const restrict quadList);
void *memQuadListInsertSorted(
	memoryQuadList *const restrict quadList, void **const restrict startA, void **const restrict startB,
	void *const restrict nextA, void *const restrict prevA, const unsigned int swapA,
	void *const restrict nextB, void *const restrict prevB, const unsigned int swapB
);

void memQuadListFree(memoryQuadList *const restrict quadList, void **const restrict start, void *const restrict data);
void memQuadListFreeArray(memoryQuadList *const restrict quadList, void *const restrict start);

void memQuadListClearRegion(memoryQuadList *const restrict quadList, memoryRegion *const restrict region, const byte_t flag, void *next);
void memQuadListClearLastRegion(memoryQuadList *const restrict quadList, memoryRegion *const restrict region);
void memQuadListClear(memoryQuadList *const restrict quadList);

#ifdef MEMORYREGION_EXTEND_ALLOCATORS
void *memQuadListExtend(memoryQuadList *const restrict quadList, void *const restrict memory, const size_t memorySize);
#endif

void memQuadListDelete(memoryQuadList *const restrict quadList, void (*freeFunc)(void *block));


#endif