#ifndef memorySingleList_h
#define memorySingleList_h


#include <stdlib.h>
#include <stdint.h>

#include "settingsMemory.h"
#include "utilMemory.h"


#define MEMSINGLELIST_BLOCK_FREE_FLAG_SIZE MEMSINGLELIST_BLOCK_HEADER_SIZE
#define MEMSINGLELIST_BLOCK_FREE_NEXT_SIZE sizeof(void *)
#define MEMSINGLELIST_BLOCK_USED_NEXT_SIZE MEMSINGLELIST_BLOCK_HEADER_SIZE

#ifndef MEMSINGLELIST_MEMORY_LEAN

#define MEMSINGLELIST_BLOCK_HEADER_SIZE ((uintptr_t)memoryAlign(sizeof(void *)))
#define MEMSINGLELIST_BLOCK_MIN_SIZE (MEMSINGLELIST_BLOCK_HEADER_SIZE + MEMSINGLELIST_BLOCK_FREE_NEXT_SIZE)
// Return the minimum block size for an element of "size" bytes.
#define memSingleListGetBlockSize(size) ((size_t)memoryAlign( \
	((size) > MEMSINGLELIST_BLOCK_FREE_NEXT_SIZE) ? \
	(MEMSINGLELIST_BLOCK_HEADER_SIZE + (size)) : \
	MEMSINGLELIST_BLOCK_MIN_SIZE \
))

#else

#define MEMSINGLELIST_BLOCK_HEADER_SIZE sizeof(void *)
#define MEMSINGLELIST_BLOCK_MIN_SIZE (MEMSINGLELIST_BLOCK_HEADER_SIZE + MEMSINGLELIST_BLOCK_FREE_NEXT_SIZE)
// Return the minimum block size for an element of "size" bytes.
#define memSingleListGetBlockSize(size) ( \
	((size) > MEMSINGLELIST_BLOCK_FREE_NEXT_SIZE) ? \
	(MEMSINGLELIST_BLOCK_HEADER_SIZE + (size)) : \
	MEMSINGLELIST_BLOCK_MIN_SIZE \
)

#endif

// We define the active flag as "0x00" so
// it's easier to loop through sub-arrays.
#define MEMSINGLELIST_FLAG_ACTIVE   0x00
#define MEMSINGLELIST_FLAG_INACTIVE 0x01
// This is used if there are no active elements after the block.
#define MEMSINGLELIST_FLAG_INVALID  0x02


// Get the value of the current segment pointed to by "block".
#define memSingleListBlockGetValue(block) *((void **)block)
// Get the value of the current segment pointed to by "block".
// Unlike the previous definition, this casts to a unsigned integer.
#define memSingleListBlockFreeGetFlag(block) *((uintptr_t *)block)

// We'll need to remove the active flag from
// the pointer if we want to get its real value.
#define memSingleListBlockGetNext(next) ((void *)(((uintptr_t)memSingleListBlockGetValue(next)) & MEMORY_DATA_MASK))
// The active flag is stored in the
// least significant bit of the pointer.
#define memSingleListBlockGetFlag(next) (((uintptr_t)memSingleListBlockGetValue(next)) & MEMORY_FLAG_MASK)

// Return the address of the next block in the list.
#define memSingleListBlockGetNextBlock(block, size) memoryAddPointer(block, size)
// Return the address of the previous block in the list.
#define memSingleListBlockGetPrevBlock(block, size) memorySubPointer(block, size)

// Get the block's next pointer from its flags segment.
#define memSingleListBlockFreeFlagGetNext(block) ((void **)memoryAddPointer(block, MEMSINGLELIST_BLOCK_FREE_FLAG_SIZE))
// Get the block's flags from its next pointer segment.
#define memSingleListBlockFreeNextGetFlag(block) ((uintptr_t *)memorySubPointer(block, MEMSINGLELIST_BLOCK_FREE_FLAG_SIZE))

// Get the block's data from its next pointer segment.
#define memSingleListBlockUsedNextGetData(block) ((void **)memoryAddPointer(block, MEMSINGLELIST_BLOCK_USED_NEXT_SIZE))
// Get the block's next pointer from its data segment.
#define memSingleListBlockUsedDataGetNext(block) ((void **)memorySubPointer(block, MEMSINGLELIST_BLOCK_USED_NEXT_SIZE))
// Get the block's flag value from its data segment.
#define memSingleListBlockUsedDataGetFlagValue(block) memSingleListBlockGetFlag(memSingleListBlockUsedDataGetNext(block))

// Return whether or not the block is active.
#define memSingleListBlockIsActive(block) (memSingleListBlockGetFlag(block) == MEMSINGLELIST_FLAG_ACTIVE)
// Return whether or not the block is inactive.
#define memSingleListBlockIsInactive(block) (memSingleListBlockFreeGetFlag(block) == MEMSINGLELIST_FLAG_INACTIVE)
// Return whether or not the block is invalid.
#define memSingleListBlockIsInvalid(block) (memSingleListBlockFreeGetFlag(block) == MEMSINGLELIST_FLAG_INVALID)

// Return whether or not the block is free.
#define memSingleListBlockIsFree(block) !memSingleListBlockIsActive(block)
// Return whether or not the block is in use.
#define memSingleListBlockIsUsed(block) memSingleListBlockIsActive(block)

// Get the next element in a an array list. Active elements
// have no flags, so we can simply dereference the next pointer.
#define memSingleListNext(element) memSingleListBlockGetValue(memSingleListBlockUsedDataGetNext(element))

#define memSingleListRegionStart(region) memSingleListBlockFreeNextGetFlag(((memoryRegion *)(region))->start)


// Return the amount of memory required
// for "num" many blocks of "size" bytes.
#define memSingleListMemoryForBlocks(num, size) ((num) * memSingleListGetBlockSize(size))
// Return the amount of memory required for a
// region of "num" many blocks of "size" bytes.
#define memSingleListMemoryForBlocksRegion(num, size) memoryGetRequiredSize(memSingleListMemoryForBlocks(num, size))


#ifdef MEMSINGLELIST_COUNT_USED_BLOCKS
#define MEMSINGLELIST_LOOP_BEGIN(allocator, node, type)                                         \
{                                                                                               \
	memoryRegion *allocator##_region_##node = allocator.region;                                 \
	size_t allocator##_remaining_##node = allocator.usedBlocks;                                 \
	type *node = (void *)memSingleListBlockFreeFlagGetNext(allocator##_region_##node->start);   \
	for(;;){                                                                                    \
		const uintptr_t allocator##_flag_##node = memSingleListBlockUsedDataGetFlagValue(node); \
		if(allocator##_flag_##node == MEMSINGLELIST_FLAG_ACTIVE){                               \
			--allocator##_remaining_##node;

#define MEMSINGLELIST_LOOP_INACTIVE(node)                                 \
		}else if(allocator##_flag_##node == MEMSINGLELIST_FLAG_INACTIVE){ \

#define MEMSINGLELIST_LOOP_END(allocator, node)                                                 \
		}                                                                                       \
		if(allocator##_remaining_##node <= 0){                                                  \
			break;                                                                              \
		}                                                                                       \
		node = memSingleListBlockGetNextBlock(node, allocator.blockSize);                       \
		if((void *)node >= (void *)allocator##_region_##node){                                  \
			allocator##_region_##node = allocator##_region_##node->next;                        \
			if(allocator##_region_##node == NULL){                                              \
				break;                                                                          \
			}                                                                                   \
			node = (void *)memSingleListBlockFreeFlagGetNext(allocator##_region_##node->start); \
		}                                                                                       \
	}                                                                                           \
}
#else
#define MEMSINGLELIST_LOOP_BEGIN(allocator, node, type)                                         \
{                                                                                               \
	memoryRegion *allocator##_region_##node = allocator.region;                                 \
	type *node = (void *)memSingleListBlockFreeFlagGetNext(allocator##_region_##node->start);   \
	for(;;){                                                                                    \
		const uintptr_t allocator##_flag_##node = memSingleListBlockUsedDataGetFlagValue(node); \
		if(allocator##_flag_##node == MEMSINGLELIST_FLAG_ACTIVE){

#define MEMSINGLELIST_LOOP_INACTIVE(node)                                 \
		}else if(allocator##_flag_##node == MEMSINGLELIST_FLAG_INACTIVE){ \

#define MEMSINGLELIST_LOOP_END(allocator, node)                                                 \
		}else if(allocator##_flag_##node == MEMSINGLELIST_FLAG_INVALID){                        \
			break;                                                                              \
		}                                                                                       \
		node = memSingleListBlockGetNextBlock(node, allocator.blockSize);                       \
		if((void *)node >= (void *)allocator##_region_##node){                                  \
			allocator##_region_##node = allocator##_region_##node->next;                        \
			if(allocator##_region_##node == NULL){                                              \
				break;                                                                          \
			}                                                                                   \
			node = (void *)memSingleListBlockFreeFlagGetNext(allocator##_region_##node->start); \
		}                                                                                       \
	}                                                                                           \
}
#endif


// Block data usage diagrams:
// Used: [next][    data    ]
// Free: [flag][next][ fill ]

// For used blocks, the next pointer points to the next block's data segment.
// For free blocks, the next pointer points to the next block's next pointer.


typedef struct memorySingleList {
	size_t blockSize;
	// Points to the next pointer of a free block.
	void *nextFreeBlock;
	#ifdef MEMSINGLELIST_COUNT_USED_BLOCKS
	size_t usedBlocks;
	#endif

	// This is stored at the very end of the allocated memory,
	// meaning it can be used as a pointer to the end. The
	// structure contains a pointer to the start of the region
	// as well as a pointer to the extension that follows it.
	memoryRegion *region;
} memorySingleList;


void *memSingleListInit(
	memorySingleList *const restrict singleList,
	void *const restrict memory, const size_t memorySize, const size_t blockSize
);

void *memSingleListAlloc(memorySingleList *const restrict singleList);
void *memSingleListPrepend(memorySingleList *const restrict singleList, void **const restrict start);
void *memSingleListAppend(memorySingleList *const restrict singleList, void **const restrict start);
void *memSingleListInsertAfter(
	memorySingleList *const restrict singleList,
	void **const restrict start, void *const prev
);

void memSingleListFree(
	memorySingleList *const restrict singleList,
	void **const restrict start, void *const restrict data, void *const restrict prev
);
void memSingleListFreeArray(memorySingleList *const restrict singleList, void *const restrict start);

void memSingleListClearRegion(
	memorySingleList *const restrict singleList,
	memoryRegion *const restrict region, const byte_t flag, void *const restrict next
);
void memSingleListClearLastRegion(memorySingleList *const restrict singleList, memoryRegion *const restrict region);
void memSingleListClear(memorySingleList *const restrict singleList);

#ifdef MEMORYREGION_EXTEND_ALLOCATORS
void *memSingleListExtend(memorySingleList *const restrict singleList, void *const restrict memory, const size_t memorySize);
#endif


#endif