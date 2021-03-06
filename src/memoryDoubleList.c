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


#warning "What if the beginning or end of a memory region is not aligned?"
#warning "We should go back to using the total memory size as input for init and extend, too."


void *memDoubleListInit(memoryDoubleList *const restrict doubleList, void *const memory, const size_t memorySize, const size_t blockSize){
	// Make sure the user isn't being difficult.
	if(memory != NULL){
		memoryRegion *region;

		doubleList->blockSize = memDoubleListGetBlockSize(blockSize);

		region = memoryGetRegionFromSize(memory, memorySize);
		region->start = memDoubleListBlockFreeFlagGetNext(memory);
		region->next = NULL;
		memDoubleListClearLastRegion(doubleList, region);

		doubleList->nextFreeBlock = memDoubleListBlockFreeFlagGetNext(memory);
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

// Prepend a new block before another in an array list.
void *memDoubleListInsertBefore(memoryDoubleList *const restrict doubleList, void **const restrict start, void *prevData){
	void *const newBlock = doubleList->nextFreeBlock;

	if(newBlock != NULL){
		// Move the free pointer to the next free block.
		doubleList->nextFreeBlock = memDoubleListBlockFreeGetNext(newBlock);
		#ifdef MEMDOUBLELIST_COUNT_USED_BLOCKS
		++doubleList->usedBlocks;
		#endif

		// Make the new block point back to the one before it.
		*memDoubleListBlockUsedDataGetPrev(newBlock) = prevData;

		// If a previous block exists, make it point to the new block.
		if(prevData != NULL){
			void *block;

			prevData = memDoubleListBlockUsedDataGetNext(prevData);
			block = memDoubleListBlockUsedGetNext(prevData);

			// Make the block we're inserting after point to the new block.
			memDoubleListBlockUsedGetNext(prevData) = newBlock;

			// Make the block we're inserting before point back to the new block.
			if(block != NULL){
				*memDoubleListBlockUsedDataGetPrev(block) = newBlock;
			}

			// Make the new block point to the one after it.
			*memDoubleListBlockUsedDataGetNext(newBlock) = block;

		// Otherwise, the new block should be at the beginning of the list.
		}else{
			void *const startBlock = *start;

			*memDoubleListBlockUsedDataGetNext(newBlock) = startBlock;

			// Make sure the old start block points back to the new one.
			if(startBlock != NULL){
				*memDoubleListBlockUsedDataGetPrev(startBlock) = newBlock;
			}
			// The pointer to the beginning of the
			// list should now point to the new block.
			*start = newBlock;
		}
	}

	return(newBlock);
}

// Append a new block after another in an array list.
void *memDoubleListInsertAfter(memoryDoubleList *const restrict doubleList, void **const restrict start, void *data){
	void *const newBlock = doubleList->nextFreeBlock;

	if(newBlock != NULL){
		// Move the free pointer to the next free block.
		doubleList->nextFreeBlock = memDoubleListBlockFreeGetNext(newBlock);
		#ifdef MEMDOUBLELIST_COUNT_USED_BLOCKS
		++doubleList->usedBlocks;
		#endif

		// Make the new block point back to the one before it.
		*memDoubleListBlockUsedDataGetPrev(newBlock) = data;

		// Fix up the previous block's pointer if it exists.
		if(data != NULL){
			void *nextBlock;

			data = memDoubleListBlockUsedDataGetNext(data);
			nextBlock = memDoubleListBlockUsedGetNext(data);

			// Make the block we're inserting after point to the new block.
			memDoubleListBlockUsedGetNext(data) = newBlock;

			// Make the block we're inserting before point back to the new block.
			if(nextBlock != NULL){
				*memDoubleListBlockUsedDataGetPrev(nextBlock) = newBlock;
			}

			// Make the new block point to the one after it.
			*memDoubleListBlockUsedDataGetNext(newBlock) = nextBlock;

		// Otherwise, use the starting block to insert the new one.
		}else{
			void *startBlock = *start;

			// If there's a starting block, insert the new one after it.
			if(startBlock != NULL){
				startBlock = memDoubleListBlockUsedDataGetNext(startBlock);

				*memDoubleListBlockUsedDataGetNext(newBlock) = memDoubleListBlockUsedGetNext(startBlock);
				memDoubleListBlockUsedGetNext(startBlock) = newBlock;

			// Otherwise, just start a new array list.
			}else{
				*memDoubleListBlockUsedDataGetNext(newBlock) = NULL;
				*start = newBlock;
			}
		}
	}

	return(newBlock);
}


void memDoubleListFree(memoryDoubleList *const restrict doubleList, void **const restrict start, void *const restrict data){
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
void memDoubleListClearRegion(memoryDoubleList *const restrict doubleList, memoryRegion *const restrict region, const byte_t flag, void *const next){
	const size_t blockSize = doubleList->blockSize;
	void *currentBlock = region->start;
	void *nextBlock = memDoubleListBlockGetNextBlock(currentBlock, blockSize);

	// Set the flag and next pointer for each block!
	while(nextBlock < (void *)region){
		*memDoubleListBlockFreeNextGetFlag(currentBlock) = flag;
		memDoubleListBlockFreeGetNext(currentBlock) = nextBlock;

		currentBlock = nextBlock;
		nextBlock = memDoubleListBlockGetNextBlock(currentBlock, blockSize);
	}

	// Set the flag and next pointer for the last block!
	*memDoubleListBlockFreeNextGetFlag(currentBlock) = flag;
	memDoubleListBlockFreeGetNext(currentBlock) = next;
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
		*memDoubleListBlockFreeNextGetFlag(currentBlock) = MEMDOUBLELIST_FLAG_INVALID;
		memDoubleListBlockFreeGetNext(currentBlock) = nextBlock;

		currentBlock = nextBlock;
		nextBlock = memDoubleListBlockGetNextBlock(currentBlock, blockSize);
	}

	// Set the flag and next pointer for the last block!
	*memDoubleListBlockFreeNextGetFlag(currentBlock) = MEMDOUBLELIST_FLAG_INVALID;
	memDoubleListBlockFreeGetNext(currentBlock) = NULL;
}

/*
** Clear every memory region in the allocator.
** This assumes that there is at least one region.
*/
void memDoubleListClear(memoryDoubleList *const restrict doubleList){
	memoryRegion *region = doubleList->region;
	doubleList->nextFreeBlock = region->start;
	#ifdef MEMDOUBLELIST_COUNT_USED_BLOCKS
	doubleList->usedBlocks = 0;
	#endif

	// Loop through every region in the allocator.
	for(;;){
		memoryRegion *nextRegion = region->next;

		// If this is not the last region, make this region's
		// last block point to the first in the next region.
		if(nextRegion != NULL){
			memDoubleListClearRegion(doubleList, region, MEMDOUBLELIST_FLAG_INVALID, nextRegion->start);

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
		memoryRegion *newRegion = memoryGetRegionFromSize(memory, memorySize);
		// Add the new region to the end of the list!
		memoryRegionAppend(&doubleList->region, newRegion, memory);
		newRegion->start = memDoubleListBlockFreeFlagGetNext(memory);
		// Set up its memory!
		memDoubleListClearLastRegion(doubleList, newRegion);

		doubleList->nextFreeBlock = memDoubleListBlockFreeFlagGetNext(memory);
	}

	return(memory);
}
#endif


void memDoubleListDelete(memoryDoubleList *const restrict doubleList, void (*const freeFunc)(void *block)){
	memoryRegion *region = doubleList->region;
	// Free every memory region in the allocator.
	while(region != NULL){
		memoryRegion *next = region->next;

		(*freeFunc)(memDoubleListRegionStart(region));
		region = next;
	}
}