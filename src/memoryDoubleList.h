#ifndef memoryDoubleList_h
#define memoryDoubleList_h


#include <stdlib.h>
#include <stdint.h>

#include "memorySettings.h"
#include "memoryShared.h"


#define MEMDOUBLELIST_BLOCK_FREE_FLAG_SIZE MEMDOUBLELIST_BLOCK_HEADER_SIZE
#define MEMDOUBLELIST_BLOCK_FREE_NEXT_SIZE sizeof(void *)
#define MEMDOUBLELIST_BLOCK_USED_NEXT_SIZE sizeof(void *)
#define MEMDOUBLELIST_BLOCK_USED_PREV_SIZE (MEMDOUBLELIST_BLOCK_HEADER_SIZE - MEMDOUBLELIST_BLOCK_USED_NEXT_SIZE)

#ifndef MEMDOUBLELIST_MEMORY_LEAN

#define MEMDOUBLELIST_BLOCK_HEADER_SIZE ((size_t)memoryAlign(sizeof(void *) + sizeof(void *)))
#define MEMDOUBLELIST_BLOCK_MIN_SIZE (MEMDOUBLELIST_BLOCK_HEADER_SIZE + MEMDOUBLELIST_BLOCK_FREE_NEXT_SIZE)
//Return the minimum block size for an element of "size" bytes.
#define memDoubleListGetBlockSize(size) ((size_t)memoryAlign( \
	((size) > MEMDOUBLELIST_BLOCK_FREE_NEXT_SIZE) ? \
	(MEMDOUBLELIST_BLOCK_HEADER_SIZE + (size)) : \
	MEMDOUBLELIST_BLOCK_MIN_SIZE \
))

#else

#define MEMDOUBLELIST_BLOCK_HEADER_SIZE (sizeof(void *) + sizeof(void *))
#define MEMDOUBLELIST_BLOCK_MIN_SIZE (MEMDOUBLELIST_BLOCK_HEADER_SIZE + MEMDOUBLELIST_BLOCK_FREE_NEXT_SIZE)
//Return the minimum block size for an element of "size" bytes.
#define memDoubleListGetBlockSize(size) ( \
	((size) > MEMDOUBLELIST_BLOCK_FREE_NEXT_SIZE) ? \
	(MEMDOUBLELIST_BLOCK_HEADER_SIZE + (size)) : \
	MEMDOUBLELIST_BLOCK_MIN_SIZE \
)

#endif

//We define the active flag as "0x00" so
//it's easier to loop through sub-arrays.
#define MEMDOUBLELIST_FLAG_ACTIVE   0x00
#define MEMDOUBLELIST_FLAG_INACTIVE 0x01
//This is used if there are no active elements after the block.
#define MEMDOUBLELIST_FLAG_INVALID  0x02


//Get the value of the current segment pointed to by "block".
#define memDoubleListBlockGetValue(block) *((void **)block)
//Get the value of the current segment pointed to by "block".
//Unlike the previous definition, this casts to a unsigned integer.
#define memDoubleListBlockFreeGetFlag(block) *((uintptr_t *)block)

//Return the address of the next block in the list.
#define memDoubleListBlockGetNextBlock(block, size) memoryAddPointer(block, size)
//Return the address of the previous block in the list.
#define memDoubleListBlockGetPrevBlock(block, size) memorySubPointer(block, size)

//Get the block's next pointer from its flags segment.
#define memDoubleListBlockFreeFlagGetNext(block) ((void **)memoryAddPointer(block, MEMDOUBLELIST_BLOCK_HEADER_SIZE))
//Get the block's flags from its next pointer segment.
#define memDoubleListBlockFreeNextGetFlag(block) ((uintptr_t *)memorySubPointer(block, MEMDOUBLELIST_BLOCK_HEADER_SIZE))

//Get the block's data from its next pointer segment.
#define memDoubleListBlockUsedNextGetPrev(block) ((void **)memoryAddPointer(block, MEMDOUBLELIST_BLOCK_USED_NEXT_SIZE))
//Get the block's previous pointer from its next pointer segment.
#define memDoubleListBlockUsedNextGetData(block) ((void **)memoryAddPointer(block, MEMDOUBLELIST_BLOCK_HEADER_SIZE))
//Get the block's next pointer from its previous pointer segment.
#define memDoubleListBlockUsedPrevGetNext(block) ((void **)memorySubPointer(block, MEMDOUBLELIST_BLOCK_USED_NEXT_SIZE))
//Get the block's data from its previous pointer segment.
#define memDoubleListBlockUsedPrevGetData(block) ((void **)memoryAddPointer(block, MEMDOUBLELIST_BLOCK_USED_PREV_SIZE))
//Get the block's next pointer from its data segment.
#define memDoubleListBlockUsedDataGetNext(block) ((void **)memorySubPointer(block, MEMDOUBLELIST_BLOCK_HEADER_SIZE))
//Get the block's previous pointer from its data segment.
#define memDoubleListBlockUsedDataGetPrev(block) ((void **)memorySubPointer(block, MEMDOUBLELIST_BLOCK_USED_PREV_SIZE))

//Return whether or not the block is active.
#define memDoubleListBlockIsActive(block) (memDoubleListBlockGetFlag(block) == MEMDOUBLELIST_FLAG_ACTIVE)
//Return whether or not the block is inactive.
#define memDoubleListBlockIsInactive(block) (memDoubleListBlockFreeGetFlag(block) == MEMDOUBLELIST_FLAG_INACTIVE)
//Return whether or not the block is invalid.
#define memDoubleListBlockIsInvalid(block) (memDoubleListBlockFreeGetFlag(block) == MEMDOUBLELIST_FLAG_INVALID)

//Return whether or not the block is free.
#define memDoubleListBlockIsFree(block) !memDoubleListBlockIsActive(block)
//Return whether or not the block is in use.
#define memDoubleListBlockIsUsed(block) memDoubleListBlockIsActive(block)


//Return the amount of memory required
//for "num" many blocks of "size" bytes.
#define memDoubleListMemoryForBlocks(num, size) memoryGetRequiredSize((num) * memDoubleListGetBlockSize(size))


//Block data usage diagrams:
//Occupied:   [next][prev][    data    ]
//Unoccupied: [flag][fill][next][ fill ]


typedef struct memoryDoubleList {
	size_t blockSize;
	void *nextFreeBlock;

	//This is stored at the very end of the allocated memory,
	//meaning it can be used as a pointer to the end. The
	//structure contains a pointer to the start of the region
	//as well as a pointer to the extension that follows it.
	memoryRegion *region;
} memoryDoubleList;


void *memDoubleListInit(memoryDoubleList *doubleList, void *memory, const size_t numBlocks, const size_t blockSize);

void *memDoubleListAlloc(memoryDoubleList *doubleList);
void *memDoubleListPrepend(memoryDoubleList *doubleList, void **start);
void *memDoubleListAppend(memoryDoubleList *doubleList, void **start);
void *memDoubleListInsertBefore(memoryDoubleList *doubleList, void **start, void *prevData);
void *memDoubleListInsertAfter(memoryDoubleList *doubleList, void **start, void *data);

void memDoubleListFree(memoryDoubleList *doubleList, void **start, void *data);

void memDoubleListClearRegion(memoryDoubleList *doubleList, memoryRegion *region, const byte_t flag, void *next);
void memDoubleListClearLastRegion(memoryDoubleList *doubleList, memoryRegion *region);
void memDoubleListClear(memoryDoubleList *doubleList);

void *memDoubleListExtend(memoryDoubleList *doubleList, void *memory, const size_t numBlocks);

void memDoubleListDelete(memoryDoubleList *doubleList);


#endif