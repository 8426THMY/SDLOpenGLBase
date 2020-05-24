#include "memoryQuadList.h"


#include "utilTypes.h"


// These functions all do the same thing, but
// they, but make the code a bit easier to read.
#define memQuadListBlockFreeGetNext(block) memQuadListBlockGetValue(block)
#define memQuadListBlockUsedGetNextA(block) memQuadListBlockGetValue(block)
#define memQuadListBlockUsedGetPrevA(block) memQuadListBlockGetValue(block)
#define memQuadListBlockUsedGetNextB(block) memQuadListBlockGetValue(block)
#define memQuadListBlockUsedGetPrevB(block) memQuadListBlockGetValue(block)
#define memQuadListBlockUsedGetData(block) memQuadListBlockGetValue(block)

// Write the flag specified by "flag" to the pointer "next".
#define memQuadListBlockSetFlag(next, flag) ((void *)(((uintptr_t)memQuadListBlockGetValue(next)) | (flag)))
// Add the "active" flag to the pointer specified by "next".
#define memQuadListBlockMakeActive(next) memQuadListBlockSetFlag(next, MEMQUADLIST_FLAG_ACTIVE)
// Add the "inactive" flag to the pointer specified by "next".
#define memQuadListBlockMakeInactive(next) memQuadListBlockSetFlag(next, MEMQUADLIST_FLAG_INACTIVE)
// Add the "invalid" flag to the pointer specified by "next".
#define memQuadListBlockMakeInvalid(next) memQuadListBlockSetFlag(next, MEMQUADLIST_FLAG_INVALID)


#warning "What if the beginning or end of a memory region is not aligned?"
#warning "We should go back to using the total memory size as input for init and extend, too."


void *memQuadListInit(memoryQuadList *const restrict quadList, void *const restrict memory, const size_t memorySize, const size_t blockSize){
	// Make sure the user isn't being difficult.
	if(memory != NULL){
		memoryRegion *region;

		quadList->blockSize = memQuadListGetBlockSize(blockSize);

		region = memoryGetRegionFromSize(memory, memorySize);
		region->start = memQuadListBlockFreeFlagGetNext(memory);
		region->next = NULL;
		memQuadListClearLastRegion(quadList, region);

		quadList->nextFreeBlock = memQuadListBlockFreeFlagGetNext(memory);
		#ifdef MEMQUADLIST_COUNT_USED_BLOCKS
		quadList->usedBlocks = 0;
		#endif
		quadList->region = region;
	}

	return(memory);
}


// Used to create a new array list.
void *memQuadListAlloc(memoryQuadList *const restrict quadList){
	void *const newBlock = quadList->nextFreeBlock;

	if(newBlock != NULL){
		// Move the free pointer to the next free block.
		quadList->nextFreeBlock = memQuadListBlockFreeGetNext(newBlock);
		#ifdef MEMQUADLIST_COUNT_USED_BLOCKS
		++quadList->usedBlocks;
		#endif

		// Change this block's pointers.
		*memQuadListBlockUsedDataGetPrevB(newBlock) = NULL;
		*memQuadListBlockUsedDataGetNextB(newBlock) = NULL;
		*memQuadListBlockUsedDataGetPrevA(newBlock) = NULL;
		*memQuadListBlockUsedDataGetNextA(newBlock) = NULL;
	}

	return(newBlock);
}

/*
** Insert a new block into a sorted array list. In sorted lists,
** owner A is only guaranteed to be the same for previous elements.
*/
void *memQuadListInsertSorted(
	memoryQuadList *const restrict quadList, void **const restrict startA, void **const restrict startB,
	void *const restrict nextA, void *const restrict prevA, const unsigned int swapA,
	void *const restrict nextB, void *const restrict prevB, const unsigned int swapB
){

	void *const newBlock = quadList->nextFreeBlock;

	if(newBlock != NULL){
		// Move the free pointer to the next free block.
		quadList->nextFreeBlock = memQuadListBlockFreeGetNext(newBlock);
		#ifdef MEMQUADLIST_COUNT_USED_BLOCKS
		++quadList->usedBlocks;
		#endif


		// Fix up the pointers for the first
		// owner's next and previous blocks.
		if(prevA != NULL){
			*memQuadListBlockUsedDataGetNextA(prevA) = newBlock;
		}else{
			*startA = newBlock;
		}
		if(nextA != NULL){
			// Owner A in the new block isn't guaranteed
			// to be the same as owner A in the next block.
			if(swapA){
				*memQuadListBlockUsedDataGetPrevB(nextA) = newBlock;
			}else{
				*memQuadListBlockUsedDataGetPrevA(nextA) = newBlock;
			}
		}

		// Fix up the pointers for the second
		// owner's next and previous blocks.
		if(prevB != NULL){
			*memQuadListBlockUsedDataGetNextB(prevB) = newBlock;
		}else{
			*startB = newBlock;
		}
		if(nextB != NULL){
			// Owner B in the new block isn't guaranteed
			// to be the same as owner B in the next block.
			if(swapB){
				*memQuadListBlockUsedDataGetPrevA(nextA) = newBlock;
			}else{
				*memQuadListBlockUsedDataGetPrevB(nextA) = newBlock;
			}
		}


		// Change this block's pointers.
		*memQuadListBlockUsedDataGetPrevB(newBlock) = prevB;
		*memQuadListBlockUsedDataGetNextB(newBlock) = nextB;
		*memQuadListBlockUsedDataGetPrevA(newBlock) = prevA;
		*memQuadListBlockUsedDataGetNextA(newBlock) = nextA;
	}

	return(newBlock);
}


void memQuadListFree(memoryQuadList *const restrict quadList, void **const restrict start, void *const restrict data){
	*memQuadListBlockFreeNextGetFlag(data) = MEMQUADLIST_FLAG_INACTIVE;
	memQuadListBlockFreeGetNext(data) = quadList->nextFreeBlock;
	quadList->nextFreeBlock = data;
	#ifdef MEMQUADLIST_COUNT_USED_BLOCKS
	--quadList->usedBlocks;
	#endif
}

// Free a block and fix up the pointers for any previous and next blocks.
void memQuadListFreeSorted(memoryQuadList *const restrict quadList, void **const restrict startA, void **const restrict startB, void *const restrict data){
	void *const block = memQuadListBlockUsedDataGetNextA(data);

	void *const nextA = memQuadListBlockUsedGetNextA(data);
	void *const prevA = *memQuadListBlockUsedDataGetPrevA(data);
	void *const nextB = *memQuadListBlockUsedDataGetNextA(data);
	void *const prevB = *memQuadListBlockUsedDataGetPrevA(data);
	void *temp;
	// Fix up the previous blocks' pointers.
	if(prevA != NULL){
		*memQuadListBlockUsedDataGetNextA(prevA) = nextA;
	}else{
		*startA = nextA;
	}
	if(prevB != NULL){
		temp = *memQuadListBlockUsedDataGetNextA(prevB);
		if(temp == data){
			temp = nextB;
		}else{
			*memQuadListBlockUsedDataGetNextB(prevB) = nextB;
		}
	}else{
		*startB = nextB;
	}

	// Fix up the next blocks' pointers.
	if(nextA != NULL){
		temp = *memQuadListBlockUsedDataGetPrevA(nextA);
		if(temp == data){
			temp = prevA;
		}else{
			*memQuadListBlockUsedDataGetPrevB(nextA) = prevA;
		}
	}
	if(nextB != NULL){
		*memQuadListBlockUsedDataGetPrevB(nextB) = prevB;
	}


	memQuadListBlockFreeGetFlag(block) = MEMQUADLIST_FLAG_INACTIVE;
	memQuadListBlockFreeGetNext(data) = quadList->nextFreeBlock;
	quadList->nextFreeBlock = data;
	#ifdef MEMQUADLIST_COUNT_USED_BLOCKS
	--quadList->usedBlocks;
	#endif
}

// Free every block in a single array.
void memQuadListFreeArray(memoryQuadList *const restrict quadList, void *const restrict start){
	if(start != NULL){
		void *block = start;
		do {
			void *const blockNext = memQuadListBlockFreeGetNext(block);

			memQuadListBlockFreeGetFlag(memQuadListBlockFreeNextGetFlag(block)) = MEMQUADLIST_FLAG_INACTIVE;
			memQuadListBlockFreeGetNext(block) = NULL;

			#ifdef MEMQUADLIST_COUNT_USED_BLOCKS
			--quadList->usedBlocks;
			#endif

			block = blockNext;
		} while(block != NULL);
	}
}


/*
** Initialise every block in a region, setting the flag
** to "flag" and the last block's next pointer to "next".
*/
void memQuadListClearRegion(memoryQuadList *const restrict quadList, memoryRegion *const restrict region, const byte_t flag, void *const restrict next){
	const size_t blockSize = quadList->blockSize;
	void *currentBlock = region->start;
	void *nextBlock = memQuadListBlockGetNextBlock(currentBlock, blockSize);

	// Set the flag and next pointer for each block!
	while(nextBlock < (void *)region){
		*memQuadListBlockFreeNextGetFlag(currentBlock) = flag;
		memQuadListBlockFreeGetNext(currentBlock) = nextBlock;

		currentBlock = nextBlock;
		nextBlock = memQuadListBlockGetNextBlock(currentBlock, blockSize);
	}

	// Set the flag and next pointer for the last block!
	*memQuadListBlockFreeNextGetFlag(currentBlock) = flag;
	memQuadListBlockFreeGetNext(currentBlock) = next;
}

/*
** Initialise every block in a region, setting the flag
** to invalid and the last block's next pointer to NULL.
*/
void memQuadListClearLastRegion(memoryQuadList *const restrict quadList, memoryRegion *const restrict region){
	const size_t blockSize = quadList->blockSize;
	void *currentBlock = region->start;
	void *nextBlock = memQuadListBlockGetNextBlock(currentBlock, blockSize);

	// Set the flag and next pointer for each block!
	while(nextBlock < (void *)region){
		*memQuadListBlockFreeNextGetFlag(currentBlock) = MEMQUADLIST_FLAG_INVALID;
		memQuadListBlockFreeGetNext(currentBlock) = nextBlock;

		currentBlock = nextBlock;
		nextBlock = memQuadListBlockGetNextBlock(currentBlock, blockSize);
	}

	// Set the flag and next pointer for the last block!
	*memQuadListBlockFreeNextGetFlag(currentBlock) = MEMQUADLIST_FLAG_INVALID;
	memQuadListBlockFreeGetNext(currentBlock) = NULL;
}

/*
** Clear every memory region in the allocator.
** This assumes that there is at least one region.
*/
void memQuadListClear(memoryQuadList *const restrict quadList){
	memoryRegion *region = quadList->region;
	quadList->nextFreeBlock = region->start;
	#ifdef MEMQUADLIST_COUNT_USED_BLOCKS
	quadList->usedBlocks = 0;
	#endif

	// Loop through every region in the allocator.
	for(;;){
		memoryRegion *const nextRegion = region->next;

		// If this is not the last region, make this region's
		// last block point to the first in the next region.
		if(nextRegion != NULL){
			memQuadListClearRegion(quadList, region, MEMQUADLIST_FLAG_INVALID, nextRegion->start);

		// Otherwise, make it point to NULL and break the loop.
		}else{
			memQuadListClearLastRegion(quadList, region);
			break;
		}

		region = nextRegion;
	}
}


#ifdef MEMORYREGION_EXTEND_ALLOCATORS
// Append a new memory region to the end of our allocator's region list!
void *memQuadListExtend(memoryQuadList *const restrict quadList, void *const restrict memory, const size_t memorySize){
	if(memory != NULL){
		memoryRegion *const newRegion = memoryGetRegionFromSize(memory, memorySize);
		// Add the new region to the end of the list!
		memoryRegionAppend(&quadList->region, newRegion, memory);
		newRegion->start = memQuadListBlockFreeFlagGetNext(memory);
		// Set up its memory!
		memQuadListClearLastRegion(quadList, newRegion);

		quadList->nextFreeBlock = memQuadListBlockFreeFlagGetNext(memory);
	}

	return(memory);
}
#endif


void memQuadListDelete(memoryQuadList *const restrict quadList, void (*freeFunc)(void *block)){
	memoryRegion *region = quadList->region;
	// Free every memory region in the allocator.
	while(region != NULL){
		memoryRegion *const next = region->next;

		(*freeFunc)(memQuadListRegionStart(region));
		region = next;
	}
}