#ifndef memoryDoubleList_h
#define memoryDoubleList_h


#include <stdlib.h>
#include <stdint.h>

#include "settingsMemory.h"
#include "utilMemory.h"


#define MEMDOUBLELIST_BLOCK_FREE_FLAG_SIZE MEMDOUBLELIST_BLOCK_HEADER_SIZE
#define MEMDOUBLELIST_BLOCK_FREE_NEXT_SIZE sizeof(void *)
#define MEMDOUBLELIST_BLOCK_USED_NEXT_SIZE sizeof(void *)
#define MEMDOUBLELIST_BLOCK_USED_PREV_SIZE sizeof(void *)

#ifndef MEMDOUBLELIST_MEMORY_LEAN

#define MEMDOUBLELIST_BLOCK_HEADER_SIZE ((size_t)memoryAlign(sizeof(void *) + sizeof(void *)))
#define MEMDOUBLELIST_BLOCK_MIN_SIZE (MEMDOUBLELIST_BLOCK_HEADER_SIZE + MEMDOUBLELIST_BLOCK_FREE_NEXT_SIZE)
// Return the minimum block size for an element of "size" bytes.
#define memDoubleListGetBlockSize(size) ((size_t)memoryAlign( \
	((size) > MEMDOUBLELIST_BLOCK_FREE_NEXT_SIZE) ? \
	(MEMDOUBLELIST_BLOCK_HEADER_SIZE + (size)) : \
	MEMDOUBLELIST_BLOCK_MIN_SIZE \
))

#else

#define MEMDOUBLELIST_BLOCK_HEADER_SIZE (sizeof(void *) + sizeof(void *))
#define MEMDOUBLELIST_BLOCK_MIN_SIZE (MEMDOUBLELIST_BLOCK_HEADER_SIZE + MEMDOUBLELIST_BLOCK_FREE_NEXT_SIZE)
// Return the minimum block size for an element of "size" bytes.
#define memDoubleListGetBlockSize(size) ( \
	((size) > MEMDOUBLELIST_BLOCK_FREE_NEXT_SIZE) ? \
	(MEMDOUBLELIST_BLOCK_HEADER_SIZE + (size)) : \
	MEMDOUBLELIST_BLOCK_MIN_SIZE \
)

#endif

// We define the active flag as "0x00" so
// it's easier to loop through sub-arrays.
#define MEMDOUBLELIST_FLAG_ACTIVE   0x00
#define MEMDOUBLELIST_FLAG_INACTIVE 0x01
// This is used if there are no active elements after the block.
#define MEMDOUBLELIST_FLAG_INVALID  0x02


// Get the value of the current segment pointed to by "block".
#define memDoubleListBlockGetValue(block) *((void **)block)
// Get the value of the current segment pointed to by "block".
// Unlike the previous definition, this casts to a unsigned integer.
#define memDoubleListBlockFreeGetFlag(block) *((uintptr_t *)block)

// We'll need to remove the active flag from
// the pointer if we want to get its real value.
#define memDoubleListBlockGetNext(next) ((void *)(((uintptr_t)memDoubleListBlockGetValue(next)) & MEMORY_DATA_MASK))
// The active flag is stored in the
// least significant bit of the pointer.
#define memDoubleListBlockGetFlag(next) (((uintptr_t)memDoubleListBlockGetValue(next)) & MEMORY_FLAG_MASK)

// Return the address of the next block in the list.
#define memDoubleListBlockGetNextBlock(block, size) memoryAddPointer(block, size)
// Return the address of the previous block in the list.
#define memDoubleListBlockGetPrevBlock(block, size) memorySubPointer(block, size)

// Get the block's next pointer from its flags segment.
#define memDoubleListBlockFreeFlagGetNext(block) ((void **)memoryAddPointer(block, MEMDOUBLELIST_BLOCK_HEADER_SIZE))
// Get the block's flags from its next pointer segment.
#define memDoubleListBlockFreeNextGetFlag(block) ((uintptr_t *)memorySubPointer(block, MEMDOUBLELIST_BLOCK_HEADER_SIZE))

// Get the block's previous pointer from its next pointer segment.
#define memDoubleListBlockUsedNextGetPrev(block) ((void **)memoryAddPointer(block, MEMDOUBLELIST_BLOCK_USED_NEXT_SIZE))
// Get the block's data from its next pointer segment.
#define memDoubleListBlockUsedNextGetData(block) ((void **)memoryAddPointer(block, MEMDOUBLELIST_BLOCK_HEADER_SIZE))
// Get the block's next pointer from its previous pointer segment.
#define memDoubleListBlockUsedPrevGetNext(block) ((void **)memorySubPointer(block, MEMDOUBLELIST_BLOCK_USED_NEXT_SIZE))
// Get the block's data from its previous pointer segment.
#define memDoubleListBlockUsedPrevGetData(block) ((void **)memoryAddPointer(block, MEMDOUBLELIST_BLOCK_USED_PREV_SIZE))
// Get the block's next pointer from its data segment.
#define memDoubleListBlockUsedDataGetNext(block) ((void **)memorySubPointer(block, MEMDOUBLELIST_BLOCK_HEADER_SIZE))
// Get the block's previous pointer from its data segment.
#define memDoubleListBlockUsedDataGetPrev(block) ((void **)memorySubPointer(block, MEMDOUBLELIST_BLOCK_USED_PREV_SIZE))
// Get the block's flag value from its data segment.
#define memDoubleListBlockUsedDataGetFlagValue(block) memDoubleListBlockGetFlag(memDoubleListBlockUsedDataGetNext(block))

// Return whether or not the block is active.
#define memDoubleListBlockIsActive(block) (memDoubleListBlockGetFlag(block) == MEMDOUBLELIST_FLAG_ACTIVE)
// Return whether or not the block is inactive.
#define memDoubleListBlockIsInactive(block) (memDoubleListBlockFreeGetFlag(block) == MEMDOUBLELIST_FLAG_INACTIVE)
// Return whether or not the block is invalid.
#define memDoubleListBlockIsInvalid(block) (memDoubleListBlockFreeGetFlag(block) == MEMDOUBLELIST_FLAG_INVALID)

// Return whether or not the block is free.
#define memDoubleListBlockIsFree(block) !memDoubleListBlockIsActive(block)
// Return whether or not the block is in use.
#define memDoubleListBlockIsUsed(block) memDoubleListBlockIsActive(block)

// Get the next element in a an array list. Active elements
// have no flags, so we can simply dereference the next pointer.
#define memDoubleListNext(element) memDoubleListBlockGetValue(memDoubleListBlockUsedDataGetNext(element))
// Get the previous element in a an array list. Active elements
// have no flags, so we can simply dereference the next pointer.
#define memDoubleListPrev(element) memDoubleListBlockGetValue(memDoubleListBlockUsedDataGetPrev(element))

#define memDoubleListRegionStart(region) memDoubleListBlockFreeNextGetFlag(((memoryRegion *)(region))->start)


// Return the amount of memory required
// for "num" many blocks of "size" bytes.
#define memDoubleListMemoryForBlocks(num, size) ((num) * memDoubleListGetBlockSize(size))
// Return the amount of memory required for a
// region of "num" many blocks of "size" bytes.
#define memDoubleListMemoryForBlocksRegion(num, size) memoryGetRequiredSize(memDoubleListMemoryForBlocks(num, size))


#ifdef MEMDOUBLELIST_COUNT_USED_BLOCKS
#define MEMDOUBLELIST_LOOP_BEGIN(allocator, node, type)                                         \
{                                                                                               \
	memoryRegion *allocator##_region_##node = allocator.region;                                 \
	size_t allocator##_remaining_##node = allocator.usedBlocks;                                 \
	type *node = (void *)(allocator##_region_##node->start);                                    \
	for(;;){                                                                                    \
		const uintptr_t allocator##_flag_##node = memDoubleListBlockUsedDataGetFlagValue(node); \
		if(allocator##_flag_##node == MEMDOUBLELIST_FLAG_ACTIVE){                               \
			--allocator##_remaining_##node;

#define MEMDOUBLELIST_LOOP_INACTIVE(node)                                 \
		}else if(allocator##_flag_##node == MEMDOUBLELIST_FLAG_INACTIVE){ \

#define MEMDOUBLELIST_LOOP_END(allocator, node, earlyexit)                \
		}                                                                 \
		if(allocator##_remaining_##node <= 0){                            \
			earlyexit;                                                    \
		}                                                                 \
		node = memDoubleListBlockGetNextBlock(node, allocator.blockSize); \
		if((void *)node >= (void *)allocator##_region_##node){            \
			allocator##_region_##node = allocator##_region_##node->next;  \
			if(allocator##_region_##node == NULL){                        \
				break;                                                    \
			}                                                             \
			node = (void *)allocator##_region_##node->start;              \
		}                                                                 \
	}                                                                     \
}
#else
#define MEMDOUBLELIST_LOOP_BEGIN(allocator, node, type)                                         \
{                                                                                               \
	memoryRegion *allocator##_region_##node = allocator.region;                                 \
	type *node = (void *)(allocator##_region_##node->start);                                    \
	for(;;){                                                                                    \
		const uintptr_t allocator##_flag_##node = memDoubleListBlockUsedDataGetFlagValue(node); \
		if(allocator##_flag_##node == MEMDOUBLELIST_FLAG_ACTIVE){

#define MEMDOUBLELIST_LOOP_INACTIVE(node)                                 \
		}else if(allocator##_flag_##node == MEMDOUBLELIST_FLAG_INACTIVE){ \

#define MEMDOUBLELIST_LOOP_END(allocator, node, earlyexit)                \
		}else if(allocator##_flag_##node == MEMDOUBLELIST_FLAG_INVALID){  \
			earlyexit;                                                    \
		}                                                                 \
		node = memDoubleListBlockGetNextBlock(node, allocator.blockSize); \
		if((void *)node >= (void *)allocator##_region_##node){            \
			allocator##_region_##node = allocator##_region_##node->next;  \
			if(allocator##_region_##node == NULL){                        \
				break;                                                    \
			}                                                             \
			node = (void *)allocator##_region_##node->start;              \
		}                                                                 \
	}                                                                     \
}
#endif


// Block data usage diagrams:
// Used: [next][prev][    data    ]
// Free: [flag][fill][next][ fill ]

// For used blocks, the next pointer points to the next block's data segment.
// For free blocks, the next pointer points to the next block's next pointer.


typedef struct memoryDoubleList {
	size_t blockSize;
	// Points to the next pointer of a free block.
	void *nextFreeBlock;
	#ifdef MEMDOUBLELIST_COUNT_USED_BLOCKS
	size_t usedBlocks;
	#endif

	// This is stored at the very end of the allocated memory,
	// meaning it can be used as a pointer to the end. The
	// structure contains a pointer to the start of the region
	// as well as a pointer to the extension that follows it.
	memoryRegion *region;
} memoryDoubleList;


void *memDoubleListInit(memoryDoubleList *const restrict doubleList, void *const memory, const size_t memorySize, const size_t blockSize);

void *memDoubleListAlloc(memoryDoubleList *const restrict doubleList);
void *memDoubleListPrepend(memoryDoubleList *const restrict doubleList, void **const restrict start);
void *memDoubleListAppend(memoryDoubleList *const restrict doubleList, void **const restrict start);
void *memoryDoubleListInsertBefore(memoryDoubleList *const restrict doubleList, void **const restrict start, void *prevData);
void *memoryDoubleListInsertAfter(memoryDoubleList *const restrict doubleList, void **const restrict start, void *data);

void memDoubleListFree(memoryDoubleList *const restrict doubleList, void **const restrict start, void *const restrict data);
void memDoubleListFreeArray(memoryDoubleList *const restrict doubleList, void *const restrict start);

void memDoubleListClearRegion(memoryDoubleList *const restrict doubleList, memoryRegion *const restrict region, const byte_t flag, void *const next);
void memDoubleListClearLastRegion(memoryDoubleList *const restrict doubleList, memoryRegion *const restrict region);
void memDoubleListClear(memoryDoubleList *const restrict doubleList);

#ifdef MEMORYREGION_EXTEND_ALLOCATORS
void *memDoubleListExtend(memoryDoubleList *const restrict doubleList, void *const restrict memory, const size_t memorySize);
#endif

void memDoubleListDelete(memoryDoubleList *const restrict doubleList, void (*freeFunc)(void *block));


#endif