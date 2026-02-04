#include "memoryPool.h"


// These functions all do the same thing, but
// they, but make the code a bit easier to read.
#define memPoolBlockFreeGetNext(block) memPoolBlockGetValue(block)
#define memPoolBlockUsedGetData(block) memPoolBlockGetValue(block)

// Get the block's data from its flags segment.
#define memPoolBlockUsedFlagGetData(block) memPoolBlockFlagGetData(block)
// Get the block's flags from its data segment.
#define memPoolBlockUsedDataGetFlag(block) memPoolBlockDataGetFlag(block)


/*
** Initialise every block in a region, setting the flag
** to invalid and the last block's next pointer to NULL.
*/
void memPoolInitRegion(memoryPool *const restrict pool, memoryRegion *const restrict region){
	const size_t blockSize = pool->blockSize;
	void *currentBlock = region->start;
	void *nextBlock = memPoolBlockGetNextBlock(currentBlock, blockSize);

	// Set the flag and next pointer for each block!
	while(nextBlock < (void *)region){
		//*memPoolBlockFreeNextGetFlag(currentBlock) = MEMPOOL_FLAG_INVALID;
		//memPoolBlockFreeGetNext(currentBlock) = nextBlock;
		memPoolBlockGetFlag(currentBlock) = MEMPOOL_FLAG_INVALID;
		*memPoolBlockFreeFlagGetNext(currentBlock) = (void *)memPoolBlockFreeFlagGetNext(nextBlock);

		currentBlock = nextBlock;
		nextBlock = memPoolBlockGetNextBlock(currentBlock, blockSize);
	}

	// Set the flag and next pointer for the last block!
	//*memPoolBlockFreeNextGetFlag(currentBlock) = MEMPOOL_FLAG_INVALID;
	//memPoolBlockFreeGetNext(currentBlock) = NULL;
	memPoolBlockGetFlag(currentBlock) = MEMPOOL_FLAG_INVALID;
	*memPoolBlockFreeFlagGetNext(currentBlock) = NULL;
}

void *memPoolInit(memoryPool *const restrict pool, void *const restrict memory, const size_t memorySize, const size_t blockSize){
	// Make sure the user isn't being difficult.
	if(memory != NULL){
		memoryRegion *region;

		pool->blockSize = memPoolGetBlockSize(blockSize);

		region = memoryGetRegionFromSize(memory, memorySize);
		//region->start = memPoolBlockFreeFlagGetNext(memory);
		region->start = memory;
		region->next = NULL;
		memPoolInitRegion(pool, region);

		pool->nextFreeBlock = (void *)memPoolBlockFreeFlagGetNext(memory);
		#ifdef MEMPOOL_COUNT_USED_BLOCKS
		pool->usedBlocks = 0;
		#endif
		pool->region = region;
	}

	return(memory);
}


void *memPoolAlloc(memoryPool *const restrict pool){
	void *const newBlock = pool->nextFreeBlock;

	// Make sure the pool isn't full.
	if(newBlock != NULL){
		// Remove this node from the free list.
		pool->nextFreeBlock = memPoolBlockFreeGetNext(newBlock);
		#ifdef MEMPOOL_COUNT_USED_BLOCKS
		++pool->usedBlocks;
		#endif
		// Set its active flag.
		*memPoolBlockUsedDataGetFlag(newBlock) = MEMPOOL_FLAG_ACTIVE;
	}

	return(newBlock);
}


void memPoolFree(memoryPool *const restrict pool, void *const restrict data){
	// Set the block's active flag and move
	// it to the beginning of the free list.
	*memPoolBlockFreeNextGetFlag(data) = MEMPOOL_FLAG_INACTIVE;
	memPoolBlockFreeGetNext(data) = pool->nextFreeBlock;
	pool->nextFreeBlock = data;
	#ifdef MEMPOOL_COUNT_USED_BLOCKS
	--pool->usedBlocks;
	#endif
}

/*
** Clear every memory region in the allocator.
** This assumes that there is at least one region.
*/
void memPoolClear(memoryPool *const restrict pool){
	const size_t blockSize = pool->blockSize;
	memoryRegion *region = pool->region;
	//pool->nextFreeBlock = region->start;
	pool->nextFreeBlock = (void *)memPoolBlockFreeFlagGetNext(region->start);
	#ifdef MEMPOOL_COUNT_USED_BLOCKS
	pool->usedBlocks = 0;
	#endif

	// Loop through every region in the allocator.
	for(;;){
		memoryRegion *const nextRegion = region->next;
		void *currentBlock = region->start;
		void *nextBlock = memPoolBlockGetNextBlock(currentBlock, blockSize);

		// Set the flag and next pointer for each block!
		do {
			// If we've found an invalid block,
			// we've finished clearing everything.
			if(memPoolBlockGetFlag(currentBlock) == MEMPOOL_FLAG_INVALID){
				return;
			}
			//*memPoolBlockFreeNextGetFlag(currentBlock) = MEMPOOL_FLAG_INVALID;
			//memPoolBlockFreeGetNext(currentBlock) = nextBlock;
			memPoolBlockGetFlag(currentBlock) = MEMPOOL_FLAG_INVALID;
			*memPoolBlockFreeFlagGetNext(currentBlock) = (void *)memPoolBlockFreeFlagGetNext(nextBlock);

			currentBlock = nextBlock;
			nextBlock = memPoolBlockGetNextBlock(currentBlock, blockSize);
		} while(nextBlock < (void *)region);

		// Set the flag and next pointer for the last block!
		//*memPoolBlockFreeNextGetFlag(currentBlock) = MEMPOOL_FLAG_INVALID;
		memPoolBlockGetFlag(currentBlock) = MEMPOOL_FLAG_INVALID;
		if(nextRegion == NULL){
			//memPoolBlockFreeGetNext(currentBlock) = NULL;
			*memPoolBlockFreeFlagGetNext(currentBlock) = NULL;
			break;
		}else{
			//memPoolBlockFreeGetNext(currentBlock) = nextRegion->start;
			*memPoolBlockFreeFlagGetNext(currentBlock) = memPoolBlockFreeFlagGetNext(nextRegion->start);
		}
		region = nextRegion;
	}
}


#ifdef MEMORYREGION_EXTEND_ALLOCATORS
// Append a new memory region to the end of our allocator's region list!
void *memPoolExtend(memoryPool *const restrict pool, void *const restrict memory, const size_t memorySize){
	if(memory != NULL){
		memoryRegion *const newRegion = memoryGetRegionFromSize(memory, memorySize);
		// Add the new region to the end of the list!
		memoryRegionAppend(&pool->region, newRegion, memory);
		//newRegion->start = memPoolBlockFreeFlagGetNext(memory);
		// Set up its memory!
		memPoolInitRegion(pool, newRegion);

		pool->nextFreeBlock = (void *)memPoolBlockFreeFlagGetNext(memory);
	}

	return(memory);
}
#endif