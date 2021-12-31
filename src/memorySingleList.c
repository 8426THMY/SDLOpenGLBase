#include "memorySingleList.h"


#include "utilTypes.h"


// These functions all do the same thing, but
// they, but make the code a bit easier to read.
#define memSingleListBlockFreeGetNext(block) memSingleListBlockGetValue(block)
#define memSingleListBlockUsedGetNext(block) memSingleListBlockGetValue(block)

// Write the flag specified by "flag" to the pointer "next".
#define memSingleListBlockSetFlag(next, flag) ((void *)(((uintptr_t)memSingleListBlockGetValue(next)) | (flag)))
// Add the "active" flag to the pointer specified by "next".
#define memSingleListBlockMakeActive(next) memSingleListBlockSetFlag(next, MEMSINGLELIST_FLAG_ACTIVE)
// Add the "inactive" flag to the pointer specified by "next".
#define memSingleListBlockMakeInactive(next) memSingleListBlockSetFlag(next, MEMSINGLELIST_FLAG_INACTIVE)
// Add the "invalid" flag to the pointer specified by "next".
#define memSingleListBlockMakeInvalid(next) memSingleListBlockSetFlag(next, MEMSINGLELIST_FLAG_INVALID)


void *memSingleListInit(
	memorySingleList *const restrict singleList,
	void *const restrict memory, const size_t memorySize, const size_t blockSize
){

	// Make sure the user isn't being difficult.
	if(memory != NULL){
		memoryRegion *region;

		singleList->blockSize = memSingleListGetBlockSize(blockSize);

		region = memoryGetRegionFromSize(memory, memorySize);
		//region->start = (void *)memSingleListBlockFreeFlagGetNext(memory);
		region->start = memory;
		region->next = NULL;
		memSingleListClearLastRegion(singleList, region);

		singleList->nextFreeBlock = (void *)memSingleListBlockFreeFlagGetNext(memory);
		#ifdef MEMSINGLELIST_COUNT_USED_BLOCKS
		singleList->usedBlocks = 0;
		#endif
		singleList->region = region;
	}

	return(memory);
}


// Used to create a new array list.
void *memSingleListAlloc(memorySingleList *const restrict singleList){
	void *const newBlock = singleList->nextFreeBlock;

	if(newBlock != NULL){
		// Move the free pointer to the next free block.
		singleList->nextFreeBlock = memSingleListBlockFreeGetNext(newBlock);
		#ifdef MEMSINGLELIST_COUNT_USED_BLOCKS
		++singleList->usedBlocks;
		#endif

		// Change this block's pointer.
		*memSingleListBlockUsedDataGetNext(newBlock) = NULL;
	}

	return(newBlock);
}

/*
** Prepend a new block at the start of an array list.
** This assumes that "start" points to the beginning.
*/
void *memSingleListPrepend(memorySingleList *const restrict singleList, void **const restrict start){
	void *newBlock = singleList->nextFreeBlock;

	if(newBlock != NULL){
		// Move the free pointer to the next free block.
		singleList->nextFreeBlock = memSingleListBlockFreeGetNext(newBlock);
		#ifdef MEMSINGLELIST_COUNT_USED_BLOCKS
		++singleList->usedBlocks;
		#endif

		// Make sure the new block points
		// to the start of the array list.
		if(start != NULL){
			*memSingleListBlockUsedDataGetNext(newBlock) = *start;
			*start = newBlock;

		// If there's no starting block,
		// just create a new array list.
		}else{
			*memSingleListBlockUsedDataGetNext(newBlock) = NULL;
		}
	}

	return(newBlock);
}

// Append a new block to the end of an array list.
void *memSingleListAppend(memorySingleList *const restrict singleList, void **const restrict start){
	void *const newBlock = singleList->nextFreeBlock;

	if(newBlock != NULL){
		void **nextBlock = start;
		// Find the last block in the array list.
		while(*nextBlock != NULL){
			nextBlock = memSingleListBlockUsedDataGetNext(*nextBlock);
		}

		// Move the free pointer to the next free block.
		singleList->nextFreeBlock = memSingleListBlockFreeGetNext(newBlock);
		#ifdef MEMSINGLELIST_COUNT_USED_BLOCKS
		++singleList->usedBlocks;
		#endif

		// Make our new block the last in the array list!
		*nextBlock = newBlock;
		*memSingleListBlockUsedDataGetNext(newBlock) = NULL;
	}

	return(newBlock);
}

/*
** Append a new block after the element with data segment "prev"
** of the array list that begins at "start". If "prev" is a NULL
** pointer, we should insert at the beginning of the array list.
*/
void *memSingleListInsertAfter(
	memorySingleList *const restrict singleList,
	void **const restrict start, void *const prev
){

	void *const newBlock = singleList->nextFreeBlock;

	if(newBlock != NULL){
		// Move the free pointer to the next free block.
		singleList->nextFreeBlock = memSingleListBlockFreeGetNext(newBlock);
		#ifdef MEMSINGLELIST_COUNT_USED_BLOCKS
		++singleList->usedBlocks;
		#endif

		// Insert after "prev" if it exists.
		if(prev != NULL){
			void **const nextBlock = memSingleListBlockUsedDataGetNext(prev);

			// Make the new block point to the one after it.
			*memSingleListBlockUsedDataGetNext(newBlock) = *nextBlock;
			// Make the block we're inserting after point to the new block.
			*nextBlock = newBlock;

		// If no element to insert after was specified,
		// insert a block at the beginning of the array list.
		}else{
			*memSingleListBlockUsedDataGetNext(newBlock) = *start;
			*start = newBlock;
		}
	}

	return(newBlock);
}


/*
** This function assumes that the user is looping
** through the list and thus knows the previous block.
*/
void memSingleListFree(
	memorySingleList *const restrict singleList,
	void **const restrict start, void *const restrict data, void *const restrict prev
){

	void *const block = memSingleListBlockUsedDataGetNext(data);

	// If there is a previous block, fix its pointer.
	if(prev != NULL){
		*memSingleListBlockUsedDataGetNext(prev) = memSingleListBlockUsedGetNext(block);

	// Otherwise, set the start pointer if it was specified.
	}else if(start != NULL){
		*start = memSingleListBlockUsedGetNext(block);
	}

	// Now we can add the block to the free list!
	memSingleListBlockFreeGetFlag(block) = MEMSINGLELIST_FLAG_INACTIVE;
	memSingleListBlockFreeGetNext(data) = singleList->nextFreeBlock;
	singleList->nextFreeBlock = data;
	#ifdef MEMSINGLELIST_COUNT_USED_BLOCKS
	--singleList->usedBlocks;
	#endif
}

// Free every block in a single array.
void memSingleListFreeArray(memorySingleList *const restrict singleList, void *const restrict start){
	if(start != NULL){
		void *block = start;
		do {
			void *const blockNext = memSingleListBlockFreeGetNext(block);

			memSingleListBlockFreeGetFlag(memSingleListBlockFreeNextGetFlag(block)) = MEMSINGLELIST_FLAG_INACTIVE;
			memSingleListBlockFreeGetNext(block) = NULL;

			#ifdef MEMSINGLELIST_COUNT_USED_BLOCKS
			--singleList->usedBlocks;
			#endif

			block = blockNext;
		} while(block != NULL);
	}
}


/*
** Initialise every block in a region, setting the flag
** to "flag" and the last block's next pointer to "next".
*/
void memSingleListClearRegion(
	memorySingleList *const restrict singleList,
	memoryRegion *const restrict region, const byte_t flag, void *const restrict next
){

	const size_t blockSize = singleList->blockSize;
	void *currentBlock = region->start;
	void *nextBlock = memSingleListBlockGetNextBlock(currentBlock, blockSize);

	// Set the flag and next pointer for each block!
	while(nextBlock < (void *)region){
		//*memSingleListBlockFreeNextGetFlag(currentBlock) = flag;
		//memSingleListBlockFreeGetNext(currentBlock) = nextBlock;
		memSingleListBlockFreeGetFlag(currentBlock) = flag;
		*memSingleListBlockFreeFlagGetNext(currentBlock) = (void *)memSingleListBlockFreeFlagGetNext(nextBlock);

		currentBlock = nextBlock;
		nextBlock = memSingleListBlockGetNextBlock(currentBlock, blockSize);
	}

	// Set the flag and next pointer for the last block!
	//*memSingleListBlockFreeNextGetFlag(currentBlock) = flag;
	//memSingleListBlockFreeGetNext(currentBlock) = next;
	memSingleListBlockFreeGetFlag(currentBlock) = flag;
	*memSingleListBlockFreeFlagGetNext(currentBlock) = next;
}

/*
** Initialise every block in a region, setting the flag
** to invalid and the last block's next pointer to NULL.
*/
void memSingleListClearLastRegion(memorySingleList *const restrict singleList, memoryRegion *const restrict region){
	const size_t blockSize = singleList->blockSize;
	void *currentBlock = region->start;
	void *nextBlock = memSingleListBlockGetNextBlock(currentBlock, blockSize);

	// Set the flag and next pointer for each block!
	while(nextBlock < (void *)region){
		//*memSingleListBlockFreeNextGetFlag(currentBlock) = MEMSINGLELIST_FLAG_INVALID;
		//memSingleListBlockFreeGetNext(currentBlock) = nextBlock;
		memSingleListBlockFreeGetFlag(currentBlock) = MEMSINGLELIST_FLAG_INVALID;
		*memSingleListBlockFreeFlagGetNext(currentBlock) = (void *)memSingleListBlockFreeFlagGetNext(nextBlock);

		currentBlock = nextBlock;
		nextBlock = memSingleListBlockGetNextBlock(currentBlock, blockSize);
	}

	// Set the flag and next pointer for the last block!
	//*memSingleListBlockFreeNextGetFlag(currentBlock) = MEMSINGLELIST_FLAG_INVALID;
	//memSingleListBlockFreeGetNext(currentBlock) = NULL;
	memSingleListBlockFreeGetFlag(currentBlock) = MEMSINGLELIST_FLAG_INVALID;
	*memSingleListBlockFreeFlagGetNext(currentBlock) = NULL;
}

/*
** Clear every memory region in the allocator.
** This assumes that there is at least one region.
*/
void memSingleListClear(memorySingleList *const restrict singleList){
	memoryRegion *region = singleList->region;
	//singleList->nextFreeBlock = region->start;
	singleList->nextFreeBlock = (void *)memSingleListBlockFreeFlagGetNext(region->start);
	#ifdef MEMSINGLELIST_COUNT_USED_BLOCKS
	singleList->usedBlocks = 0;
	#endif

	// Loop through every region in the allocator.
	for(;;){
		memoryRegion *const nextRegion = region->next;

		// If this is not the last region, make this region's
		// last block point to the first in the next region.
		if(nextRegion != NULL){
			//memSingleListClearRegion(singleList, region, MEMSINGLELIST_FLAG_INVALID, nextRegion->start);
			memSingleListClearRegion(singleList, region, MEMSINGLELIST_FLAG_INVALID, (void *)memSingleListBlockFreeFlagGetNext(nextRegion->start));

		// Otherwise, make it point to NULL and break the loop.
		}else{
			memSingleListClearLastRegion(singleList, region);
			break;
		}

		region = nextRegion;
	}
}


#ifdef MEMORYREGION_EXTEND_ALLOCATORS
// Append a new memory region to the end of our allocator's region list!
void *memSingleListExtend(memorySingleList *const restrict singleList, void *const restrict memory, const size_t memorySize){
	if(memory != NULL){
		memoryRegion *const newRegion = memoryGetRegionFromSize(memory, memorySize);
		// Add the new region to the end of the list!
		memoryRegionAppend(&singleList->region, newRegion, memory);
		//newRegion->start = (void *)memSingleListBlockFreeFlagGetNext(memory);
		// Set up its memory!
		memSingleListClearLastRegion(singleList, newRegion);

		singleList->nextFreeBlock = (void *)memSingleListBlockFreeFlagGetNext(memory);
	}

	return(memory);
}
#endif