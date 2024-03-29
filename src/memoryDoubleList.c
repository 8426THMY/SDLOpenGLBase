#include "memoryDoubleList.h"


#include "utilTypes.h"


// These functions all do the same thing, but
// they, but make the code a bit easier to read.
#define memDoubleListBlockFreeGetNext(block) memDoubleListBlockGetValue(block)
#define memDoubleListBlockUsedGetNext(block) memDoubleListBlockGetValue(block)
#define memDoubleListBlockUsedGetPrev(block) memDoubleListBlockGetValue(block)
#define memDoubleListBlockUsedGetData(block) memDoubleListBlockGetValue(block)

// Write the flag specified by "flag" to the pointer "next".
#define memDoubleListBlockSetFlag(next, flag) ((void *)(((uintptr_t)memDoubleListBlockGetValue(next)) | (flag)))
// Add the "active" flag to the pointer specified by "next".
#define memDoubleListBlockMakeActive(next) memDoubleListBlockSetFlag(next, MEMDOUBLELIST_FLAG_ACTIVE)
// Add the "inactive" flag to the pointer specified by "next".
#define memDoubleListBlockMakeInactive(next) memDoubleListBlockSetFlag(next, MEMDOUBLELIST_FLAG_INACTIVE)
// Add the "invalid" flag to the pointer specified by "next".
#define memDoubleListBlockMakeInvalid(next) memDoubleListBlockSetFlag(next, MEMDOUBLELIST_FLAG_INVALID)


void *memDoubleListInit(
	memoryDoubleList *const restrict doubleList,
	void *const memory, const size_t memorySize, const size_t blockSize
){

	// Make sure the user isn't being difficult.
	if(memory != NULL){
		memoryRegion *region;

		doubleList->blockSize = memDoubleListGetBlockSize(blockSize);

		region = memoryGetRegionFromSize(memory, memorySize);
		//region->start = (void *)memDoubleListBlockFreeFlagGetNext(memory);
		region->start = memory;
		region->next = NULL;
		memDoubleListClearLastRegion(doubleList, region);

		doubleList->nextFreeBlock = (void *)memDoubleListBlockFreeFlagGetNext(memory);
		#ifdef MEMDOUBLELIST_COUNT_USED_BLOCKS
		doubleList->usedBlocks = 0;
		#endif
		doubleList->region = region;
	}

	return(memory);
}


// Used to create a new array list.
void *memDoubleListAlloc(memoryDoubleList *const restrict doubleList){
	void *const newBlock = doubleList->nextFreeBlock;

	if(newBlock != NULL){
		// Move the free pointer to the next free block.
		doubleList->nextFreeBlock = memDoubleListBlockFreeGetNext(newBlock);
		#ifdef MEMDOUBLELIST_COUNT_USED_BLOCKS
		++doubleList->usedBlocks;
		#endif

		// Change this block's pointers.
		*memDoubleListBlockUsedDataGetPrev(newBlock) = NULL;
		*memDoubleListBlockUsedDataGetNext(newBlock) = NULL;
	}

	return(newBlock);
}

// Prepend a new block at the start of an array list.
void *memDoubleListPrepend(memoryDoubleList *const restrict doubleList, void **const restrict start){
	void *const newBlock = doubleList->nextFreeBlock;

	if(newBlock != NULL){
		// Move the free pointer to the next free block.
		doubleList->nextFreeBlock = memDoubleListBlockFreeGetNext(newBlock);
		#ifdef MEMDOUBLELIST_COUNT_USED_BLOCKS
		++doubleList->usedBlocks;
		#endif

		// Make sure the new block points
		// to the start of the array list.
		if(start != NULL){
			*memDoubleListBlockUsedDataGetNext(newBlock) = *start;
			*memDoubleListBlockUsedDataGetPrev(newBlock) = NULL;
			// The block that used to be at the beginning
			// should point back to the new starting block.
			if(*start != NULL){
				*memDoubleListBlockUsedDataGetPrev(*start) = newBlock;
			}
			*start = newBlock;
		}else{
			*memDoubleListBlockUsedDataGetNext(newBlock) = NULL;
			*memDoubleListBlockUsedDataGetPrev(newBlock) = NULL;
		}
	}

	return(newBlock);
}

// Append a new block after another in an array list.
void *memDoubleListAppend(memoryDoubleList *const restrict doubleList, void **const restrict start){
	void *const newBlock = doubleList->nextFreeBlock;

	if(newBlock != NULL){
		void *lastBlock = NULL;
		void **nextBlock = start;
		// Find the last block in the array list.
		while(*nextBlock != NULL){
			lastBlock = *nextBlock;
			nextBlock = memDoubleListBlockUsedDataGetNext(*nextBlock);
		}

		// Move the free pointer to the next free block.
		doubleList->nextFreeBlock = memDoubleListBlockFreeGetNext(newBlock);
		#ifdef MEMDOUBLELIST_COUNT_USED_BLOCKS
		++doubleList->usedBlocks;
		#endif

		// Make our new block the last in the array list!
		*nextBlock = newBlock;
		*memDoubleListBlockUsedDataGetPrev(newBlock) = lastBlock;
		*memDoubleListBlockUsedDataGetNext(newBlock) = NULL;
	}

	return(newBlock);
}

/*
** Append a new block before the element with data segment "next"
** of the array list that begins at "start". If "next" is a NULL
** pointer, we should insert at the end of the array list.
*/
void *memDoubleListInsertBefore(
	memoryDoubleList *const restrict doubleList,
	void **const restrict start, void *const next
){

	if(next != NULL){
		void *const newBlock = doubleList->nextFreeBlock;

		if(newBlock != NULL){
			void **const prevBlock = memDoubleListBlockUsedDataGetPrev(next);

			// Move the free pointer to the next free block.
			doubleList->nextFreeBlock = memDoubleListBlockFreeGetNext(newBlock);
			#ifdef MEMDOUBLELIST_COUNT_USED_BLOCKS
			++doubleList->usedBlocks;
			#endif

			// Make the new block point to the one after it.
			*memDoubleListBlockUsedDataGetNext(newBlock) = next;
			// Make the new block point to the one before it.
			*memDoubleListBlockUsedDataGetPrev(newBlock) = *prevBlock;

			// Make the block we're inserting after point to the new block.
			if(*prevBlock != NULL){
				*memDoubleListBlockUsedDataGetNext(*prevBlock) = newBlock;
			}else{
				*start = newBlock;
			}
			// Make the block we're inserting after point to the new block.
			*prevBlock = newBlock;
		}

		return(newBlock);
	}

	// If a block to insert before wasn't specified,
	// append an element to the end of the array list.
	return(memDoubleListAppend(doubleList, start));
}

/*
** Append a new block after the element with data segment "prev"
** of the array list that begins at "start". If "prev" is a NULL
** pointer, we should insert at the beginning of the array list.
*/
void *memDoubleListInsertAfter(
	memoryDoubleList *const restrict doubleList,
	void **const restrict start, void *const prev
){

	void *const newBlock = doubleList->nextFreeBlock;

	if(newBlock != NULL){
		// Move the free pointer to the next free block.
		doubleList->nextFreeBlock = memDoubleListBlockFreeGetNext(newBlock);
		#ifdef MEMDOUBLELIST_COUNT_USED_BLOCKS
		++doubleList->usedBlocks;
		#endif

		// Make the new block point back to the one before it.
		*memDoubleListBlockUsedDataGetPrev(newBlock) = prev;

		// Insert after "prev" if it exists.
		if(prev != NULL){
			void **const nextBlock = memDoubleListBlockUsedDataGetNext(prev);

			// Make the new block point to the one after it.
			*memDoubleListBlockUsedDataGetNext(newBlock) = *nextBlock;
			// Make the block we're inserting before point back to the new block.
			if(*nextBlock != NULL){
				*memDoubleListBlockUsedDataGetPrev(*nextBlock) = newBlock;
			}
			// Make the block we're inserting after point to the new block.
			*nextBlock = newBlock;

		// If no element to insert after was specified,
		// insert a block at the beginning of the array list.
		}else{
			void *const startBlock = *start;

			*memDoubleListBlockUsedDataGetNext(newBlock) = startBlock;
			// Make sure the old start block points back to the new one.
			if(startBlock != NULL){
				*memDoubleListBlockUsedDataGetPrev(startBlock) = newBlock;
			}
			*start = newBlock;
		}
	}

	return(newBlock);
}


void memDoubleListFree(
	memoryDoubleList *const restrict doubleList,
	void **const restrict start, void *const restrict data
){

	void *const block = memDoubleListBlockUsedDataGetNext(data);

	void *const nextBlock = memDoubleListBlockUsedGetNext(block);
	void *const prevBlock = *memDoubleListBlockUsedNextGetPrev(block);
	// We'll need to fix the pointers for
	// any preceding or succeeding nodes.
	if(prevBlock != NULL){
		*memDoubleListBlockUsedDataGetNext(prevBlock) = nextBlock;

	// Otherwise, set the start pointer if it was specified.
	}else if(start != NULL){
		*start = nextBlock;
	}
	if(nextBlock != NULL){
		*memDoubleListBlockUsedDataGetPrev(nextBlock) = prevBlock;
	}

	// Now we can add the block to the free list!
	memDoubleListBlockFreeGetFlag(block) = MEMDOUBLELIST_FLAG_INACTIVE;
	memDoubleListBlockFreeGetNext(data) = doubleList->nextFreeBlock;
	doubleList->nextFreeBlock = data;
	#ifdef MEMDOUBLELIST_COUNT_USED_BLOCKS
	--doubleList->usedBlocks;
	#endif
}

// Free every block in a single array.
void memDoubleListFreeArray(memoryDoubleList *const restrict doubleList, void *const restrict start){
	if(start != NULL){
		void *block = start;
		do {
			void *blockNext = memDoubleListBlockFreeGetNext(block);

			memDoubleListBlockFreeGetFlag(memDoubleListBlockFreeNextGetFlag(block)) = MEMDOUBLELIST_FLAG_INACTIVE;
			memDoubleListBlockFreeGetNext(block) = NULL;

			#ifdef MEMDOUBLELIST_COUNT_USED_BLOCKS
			--doubleList->usedBlocks;
			#endif

			block = blockNext;
		} while(block != NULL);
	}
}


/*
** Initialise every block in a region, setting the flag
** to "flag" and the last block's next pointer to "next".
*/
void memDoubleListClearRegion(
	memoryDoubleList *const restrict doubleList,
	memoryRegion *const restrict region, const byte_t flag, void *const next
){

	const size_t blockSize = doubleList->blockSize;
	void *currentBlock = region->start;
	void *nextBlock = memDoubleListBlockGetNextBlock(currentBlock, blockSize);

	// Set the flag and next pointer for each block!
	while(nextBlock < (void *)region){
		//*memDoubleListBlockFreeNextGetFlag(currentBlock) = flag;
		//memDoubleListBlockFreeGetNext(currentBlock) = nextBlock;
		memDoubleListBlockFreeGetFlag(currentBlock) = flag;
		*memDoubleListBlockFreeFlagGetNext(currentBlock) = (void *)memDoubleListBlockFreeFlagGetNext(nextBlock);

		currentBlock = nextBlock;
		nextBlock = memDoubleListBlockGetNextBlock(currentBlock, blockSize);
	}

	// Set the flag and next pointer for the last block!
	//*memDoubleListBlockFreeNextGetFlag(currentBlock) = flag;
	//memDoubleListBlockFreeGetNext(currentBlock) = next;
	memDoubleListBlockFreeGetFlag(currentBlock) = flag;
	*memDoubleListBlockFreeFlagGetNext(currentBlock) = next;
}

/*
** Initialise every block in a region, setting the flag
** to invalid and the last block's next pointer to NULL.
*/
void memDoubleListClearLastRegion(memoryDoubleList *const restrict doubleList, memoryRegion *const restrict region){
	const size_t blockSize = doubleList->blockSize;
	void *currentBlock = region->start;
	void *nextBlock = memDoubleListBlockGetNextBlock(currentBlock, blockSize);

	// Set the flag and next pointer for each block!
	while(nextBlock < (void *)region){
		//*memDoubleListBlockFreeNextGetFlag(currentBlock) = MEMDOUBLELIST_FLAG_INVALID;
		//memDoubleListBlockFreeGetNext(currentBlock) = nextBlock;
		memDoubleListBlockFreeGetFlag(currentBlock) = MEMDOUBLELIST_FLAG_INVALID;
		*memDoubleListBlockFreeFlagGetNext(currentBlock) = (void *)memDoubleListBlockFreeFlagGetNext(nextBlock);

		currentBlock = nextBlock;
		nextBlock = memDoubleListBlockGetNextBlock(currentBlock, blockSize);
	}

	// Set the flag and next pointer for the last block!
	//*memDoubleListBlockFreeNextGetFlag(currentBlock) = MEMDOUBLELIST_FLAG_INVALID;
	//memDoubleListBlockFreeGetNext(currentBlock) = NULL;
	memDoubleListBlockFreeGetFlag(currentBlock) = MEMDOUBLELIST_FLAG_INVALID;
	*memDoubleListBlockFreeFlagGetNext(currentBlock) = NULL;
}

/*
** Clear every memory region in the allocator.
** This assumes that there is at least one region.
*/
void memDoubleListClear(memoryDoubleList *const restrict doubleList){
	memoryRegion *region = doubleList->region;
	//doubleList->nextFreeBlock = region->start;
	doubleList->nextFreeBlock = (void *)memDoubleListBlockFreeFlagGetNext(region->start);
	#ifdef MEMDOUBLELIST_COUNT_USED_BLOCKS
	doubleList->usedBlocks = 0;
	#endif

	// Loop through every region in the allocator.
	for(;;){
		memoryRegion *const nextRegion = region->next;

		// If this is not the last region, make this region's
		// last block point to the first in the next region.
		if(nextRegion != NULL){
			//memDoubleListClearRegion(doubleList, region, MEMDOUBLELIST_FLAG_INVALID, nextRegion->start);
			memDoubleListClearRegion(doubleList, region, MEMDOUBLELIST_FLAG_INVALID, (void *)memDoubleListBlockFreeFlagGetNext(nextRegion->start));

		// Otherwise, make it point to NULL and break the loop.
		}else{
			memDoubleListClearLastRegion(doubleList, region);
			break;
		}

		region = nextRegion;
	}
}


#ifdef MEMORYREGION_EXTEND_ALLOCATORS
// Append a new memory region to the end of our allocator's region list!
void *memDoubleListExtend(memoryDoubleList *const restrict doubleList, void *const restrict memory, const size_t memorySize){
	if(memory != NULL){
		memoryRegion *const newRegion = memoryGetRegionFromSize(memory, memorySize);
		// Add the new region to the end of the list!
		memoryRegionAppend(&doubleList->region, newRegion, memory);
		//newRegion->start = (void *)memDoubleListBlockFreeFlagGetNext(memory);
		// Set up its memory!
		memDoubleListClearLastRegion(doubleList, newRegion);

		doubleList->nextFreeBlock = (void *)memDoubleListBlockFreeFlagGetNext(memory);
	}

	return(memory);
}
#endif