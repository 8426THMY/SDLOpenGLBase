#ifndef memorySingleList_h
#define memorySingleList_h


#include <stdlib.h>
#include <stdint.h>

#include "settingsMemory.h"
#include "memoryShared.h"


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


#warning "These functions are kind of bad, is there a better way to stop them from looping through everything?"
#warning "What if we count the number of blocks we've used, and stop looping as soon as we find them all?"
#define MEMSINGLELIST_LOOP_BEGIN(allocator, node, type)                                   \
{                                                                                         \
	const memoryRegion *__region_##node = allocator.region;                               \
	do {                                                                                  \
		type node = (type)(allocator.region->start);                                      \
		do {                                                                              \
			const uintptr_t __flag_##node = memSingleListBlockUsedDataGetFlagValue(node); \
			if(__flag_##node == MEMSINGLELIST_FLAG_ACTIVE){                               \

#define MEMSINGLELIST_LOOP_END(allocator, node, type, earlyexit)              \
			}else if(__flag_##node == MEMSINGLELIST_FLAG_INVALID){            \
				earlyexit;                                                    \
			}                                                                 \
			node = memSingleListBlockGetNextBlock(node, allocator.blockSize); \
		} while(node < (type)__region_##node);                                \
		__region_##node = __region_##node->next;                              \
	} while(__region_##node != NULL);                                         \
}                                                                             \


// Block data usage diagrams:
// Used: [next][    data    ]
// Free: [flag][next][ fill ]

// For used blocks, the next pointer points to the next block's data segment.
// For free blocks, the next pointer points to the next block's next pointer.


typedef struct memorySingleList {
	size_t blockSize;
	// Points to the next pointer of a free block.
	void *nextFreeBlock;

	// This is stored at the very end of the allocated memory,
	// meaning it can be used as a pointer to the end. The
	// structure contains a pointer to the start of the region
	// as well as a pointer to the extension that follows it.
	memoryRegion *region;
} memorySingleList;


void *memSingleListInit(memorySingleList *singleList, void *memory, const size_t memorySize, const size_t blockSize);

void *memSingleListAlloc(memorySingleList *singleList);
void *memSingleListPrepend(memorySingleList *singleList, void **start);
void *memSingleListAppend(memorySingleList *singleList, void **start);
void *memSingleListInsertBefore(memorySingleList *singleList, void **start, void *prevData);
void *memSingleListInsertAfter(memorySingleList *singleList, void **start, void *data);

void memSingleListFree(memorySingleList *singleList, void **start, void *data, void *prevData);
void memSingleListFreeArray(memorySingleList *singleList, void *start);

void memSingleListClearRegion(memorySingleList *singleList, memoryRegion *region, const byte_t flag, void *next);
void memSingleListClearLastRegion(memorySingleList *singleList, memoryRegion *region);
void memSingleListClear(memorySingleList *singleList);

void *memSingleListExtend(memorySingleList *singleList, void *memory, const size_t memorySize);

void memSingleListDelete(memorySingleList *singleList);


#endif