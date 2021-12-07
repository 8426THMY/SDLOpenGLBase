#include "memoryPool.h"


// These functions all do the same thing, but
// they, but make the code a bit easier to read.
#define memPoolBlockFreeGetNext(block) memPoolBlockGetValue(block)
#define memPoolBlockUsedGetData(block) memPoolBlockGetValue(block)

// Get the block's data from its flags segment.
#define memPoolBlockUsedFlagGetData(block) memPoolBlockFlagGetData(block)
// Get the block's flags from its data segment.
#define memPoolBlockUsedDataGetFlag(block) memPoolBlockDataGetFlag(block)


#warning "What if the beginning or end of a memory region is not aligned?"
#warning "We should go back to using the total memory size as input for init and extend, too."


void *memPoolInit(memoryPool *const restrict pool, void *const restrict memory, const size_t memorySize, const size_t blockSize){
	// Make sure the user isn't being difficult.
	if(memory != NULL){
		memoryRegion *region;

		pool->blockSize = memPoolGetBlockSize(blockSize);

		region = memoryGetRegionFromSize(memory, memorySize);
		//region->start = memPoolBlockFreeFlagGetNext(memory);
		region->start = memory;
		region->next = NULL;
		memPoolClearLastRegion(pool, region);

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
** Initialise every block in a region, setting the flag
** to "flag" and the last block's next pointer to "next".
*/
void memPoolClearRegion(memoryPool *const restrict pool, memoryRegion *const restrict region, const byte_t flag, void *const restrict next){
	const size_t blockSize = pool->blockSize;
	void *currentBlock = region->start;
	void *nextBlock = memPoolBlockGetNextBlock(currentBlock, blockSize);

	// Set the flag and next pointer for each block!
	while(nextBlock < (void *)region){
		//*memPoolBlockFreeNextGetFlag(currentBlock) = flag;
		//memPoolBlockFreeGetNext(currentBlock) = nextBlock;
		memPoolBlockGetFlag(currentBlock) = flag;
		*memPoolBlockFreeFlagGetNext(currentBlock) = (void *)memPoolBlockFreeFlagGetNext(nextBlock);

		currentBlock = nextBlock;
		nextBlock = memPoolBlockGetNextBlock(currentBlock, blockSize);
	}

	// Set the flag and next pointer for the last block!
	//*memPoolBlockFreeNextGetFlag(currentBlock) = flag;
	//memPoolBlockFreeGetNext(currentBlock) = next;
	memPoolBlockGetFlag(currentBlock) = flag;
	*memPoolBlockFreeFlagGetNext(currentBlock) = next;
}

/*
** Initialise every block in a region, setting the flag
** to invalid and the last block's next pointer to NULL.
*/
void memPoolClearLastRegion(memoryPool *const restrict pool, memoryRegion *const restrict region){
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

/*
** Clear every memory region in the allocator.
** This assumes that there is at least one region.
*/
void memPoolClear(memoryPool *const restrict pool){
	memoryRegion *region = pool->region;
	//pool->nextFreeBlock = region->start;
	pool->nextFreeBlock = (void *)memPoolBlockFreeFlagGetNext(region->start);
	#ifdef MEMPOOL_COUNT_USED_BLOCKS
	pool->usedBlocks = 0;
	#endif

	// Loop through every region in the allocator.
	for(;;){
		memoryRegion *const nextRegion = region->next;

		// If this is not the last region, make this region's
		// last block point to the first in the next region.
		if(nextRegion != NULL){
			//memPoolClearRegion(pool, region, MEMPOOL_FLAG_INVALID, nextRegion->start);
			memPoolClearRegion(pool, region, MEMPOOL_FLAG_INVALID, (void *)memPoolBlockFreeFlagGetNext(nextRegion->start));

		// Otherwise, make it point to NULL and break the loop.
		}else{
			memPoolClearLastRegion(pool, region);
			break;
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
		memPoolClearLastRegion(pool, newRegion);

		pool->nextFreeBlock = (void *)memPoolBlockFreeFlagGetNext(memory);
	}

	return(memory);
}
#endif