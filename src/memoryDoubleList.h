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


#warning "These functions are kind of bad, is there a better way to stop them from looping through everything?"
#warning "What if we count the number of blocks we've used, and stop looping as soon as we find them all?"
#define MEMDOUBLELIST_LOOP_BEGIN(allocator, node, type)                                   \
{                                                                                         \
	const memoryRegion *__region_##node = allocator.region;                               \
	do {                                                                                  \
		type node = (type)(allocator.region->start);                                      \
		do {                                                                              \
			const uintptr_t __flag_##node = memDoubleListBlockUsedDataGetFlagValue(node); \
			if(__flag_##node == MEMDOUBLELIST_FLAG_ACTIVE){                               \

#define MEMDOUBLELIST_LOOP_END(allocator, node, type, earlyexit)              \
			}else if(__flag_##node == MEMDOUBLELIST_FLAG_INVALID){            \
				earlyexit;                                                    \
			}                                                                 \
			node = memDoubleListBlockGetNextBlock(node, allocator.blockSize); \
		} while(node < (type)__region_##node);                                \
		__region_##node = __region_##node->next;                              \
	} while(__region_##node != NULL);                                         \
}                                                                             \


// Block data usage diagrams:
// Used: [next][prev][    data    ]
// Free: [flag][fill][next][ fill ]

// For used blocks, the next pointer points to the next block's data segment.
// For free blocks, the next pointer points to the next block's next pointer.


typedef struct memoryDoubleList {
	size_t blockSize;
	// Points to the next pointer of a free block.
	void *nextFreeBlock;

	// This is stored at the very end of the allocated memory,
	// meaning it can be used as a pointer to the end. The
	// structure contains a pointer to the start of the region
	// as well as a pointer to the extension that follows it.
	memoryRegion *region;
} memoryDoubleList;


void *memDoubleListInit(memoryDoubleList *doubleList, void *memory, const size_t memorySize, const size_t blockSize);

void *memDoubleListAlloc(memoryDoubleList *doubleList);
void *memDoubleListPrepend(memoryDoubleList *doubleList, void **start);
void *memDoubleListAppend(memoryDoubleList *doubleList, void **start);
void *memDoubleListInsertBefore(memoryDoubleList *doubleList, void **start, void *prevData);
void *memDoubleListInsertAfter(memoryDoubleList *doubleList, void **start, void *data);

void memDoubleListFree(memoryDoubleList *doubleList, void **start, void *data);
void memDoubleListFreeArray(memoryDoubleList *doubleList, void *start);

void memDoubleListClearRegion(memoryDoubleList *doubleList, memoryRegion *region, const byte_t flag, void *next);
void memDoubleListClearLastRegion(memoryDoubleList *doubleList, memoryRegion *region);
void memDoubleListClear(memoryDoubleList *doubleList);

void *memDoubleListExtend(memoryDoubleList *doubleList, void *memory, const size_t memorySize);

void memDoubleListDelete(memoryDoubleList *doubleList);


#endif