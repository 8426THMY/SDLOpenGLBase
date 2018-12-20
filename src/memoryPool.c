#include "memoryPool.h"


//These functions all do the same thing, but
//they, but make the code a bit easier to read.
#define memPoolBlockFreeGetNext(block) memPoolBlockGetValue(block)
#define memPoolBlockUsedGetData(block) memPoolBlockGetValue(block)

//Get the block's next pointer from its flags segment.
#define memPoolBlockFreeFlagGetNext(block) memPoolBlockFlagGetData(block)
//Get the block's flags from its next pointer segment.
#define memPoolBlockFreeNextGetFlag(block) memPoolBlockDataGetFlag(block)

//Get the block's data from its flags segment.
#define memPoolBlockUsedFlagGetData(block) memPoolBlockFlagGetData(block)
//Get the block's flags from its data segment.
#define memPoolBlockUsedDataGetFlag(block) memPoolBlockDataGetFlag(block)


#warning "What if the beginning or end of a memory region is not aligned?"
#warning "We should go back to using the total memory size as input for init and extend, too."


void *memPoolInit(memoryPool *pool, void *memory, const size_t numBlocks, const size_t blockSize){
	//Make sure the user isn't being difficult.
	if(memory != NULL){
		memoryRegion *region;

		pool->blockSize = memPoolGetBlockSize(blockSize);

		region = memoryGetRegionFromBlocks(memory, numBlocks, pool->blockSize);
		region->start = memory;
		region->next = NULL;
		memPoolClearLastRegion(pool, region);

		pool->nextFreeBlock = memPoolBlockFreeFlagGetNext(memory);
		pool->region = region;
	}

	return(memory);
}


void *memPoolAlloc(memoryPool *pool){
	void *newBlock = pool->nextFreeBlock;

	//Make sure the pool isn't full.
	if(newBlock != NULL){
		//Remove this node from the free list.
		pool->nextFreeBlock = memPoolBlockFreeGetNext(newBlock);
		//Set its active flag.
		*memPoolBlockUsedDataGetFlag(newBlock) = MEMPOOL_FLAG_ACTIVE;
	}

	return(newBlock);
}


void memPoolFree(memoryPool *pool, void *data){
	//Set the block's active flag and move
	//it to the beginning of the free list.
	*memPoolBlockFreeNextGetFlag(data) = MEMPOOL_FLAG_INACTIVE;
	memPoolBlockFreeGetNext(data) = pool->nextFreeBlock;
	pool->nextFreeBlock = data;
}


//Initialise every block in a region, setting the flag
//to "flag" and the last block's next pointer to "next".
void memPoolClearRegion(memoryPool *pool, memoryRegion *region, const byte_t flag, void *next){
	const size_t blockSize = pool->blockSize;
	void *currentBlock = memPoolBlockFreeFlagGetNext(region->start);
	void *nextBlock = memPoolBlockGetNextBlock(currentBlock, blockSize);

	//Set the flag and next pointer for each block!
	while(nextBlock < (void *)region){
		*memPoolBlockFreeNextGetFlag(currentBlock) = flag;
		memPoolBlockFreeGetNext(currentBlock) = nextBlock;

		currentBlock = nextBlock;
		nextBlock = memPoolBlockGetNextBlock(currentBlock, blockSize);
	}

	//Set the flag and next pointer for the last block!
	*memPoolBlockFreeNextGetFlag(currentBlock) = flag;
	memPoolBlockFreeGetNext(currentBlock) = next;
}

//Initialise every block in a region, setting the flag
//to invalid and the last block's next pointer to NULL.
void memPoolClearLastRegion(memoryPool *pool, memoryRegion *region){
	const size_t blockSize = pool->blockSize;
	void *currentBlock = memPoolBlockFreeFlagGetNext(region->start);
	void *nextBlock = memPoolBlockGetNextBlock(currentBlock, blockSize);

	//Set the flag and next pointer for each block!
	while(nextBlock < (void *)region){
		*memPoolBlockFreeNextGetFlag(currentBlock) = MEMPOOL_FLAG_INVALID;
		memPoolBlockFreeGetNext(currentBlock) = nextBlock;

		currentBlock = nextBlock;
		nextBlock = memPoolBlockGetNextBlock(currentBlock, blockSize);
	}

	//Set the flag and next pointer for the last block!
	*memPoolBlockFreeNextGetFlag(currentBlock) = MEMPOOL_FLAG_INVALID;
	memPoolBlockFreeGetNext(currentBlock) = NULL;
}

//Clear every memory region in the allocator.
//This assumes that there is at least one region.
void memPoolClear(memoryPool *pool){
	memoryRegion *region = pool->region;
	pool->nextFreeBlock = memPoolBlockFreeFlagGetNext(region->start);

	//Loop through every region in the allocator.
	for(;;){
		memoryRegion *nextRegion = region->next;

		//If this is not the last region, make this region's
		//last block point to the first in the next region.
		if(nextRegion != NULL){
			memPoolClearRegion(pool, region, MEMPOOL_FLAG_INVALID, memPoolBlockFreeFlagGetNext(nextRegion->start));

		//Otherwise, make it point to NULL and break the loop.
		}else{
			memPoolClearLastRegion(pool, region);
			break;
		}

		region = nextRegion;
	}
}


//Append a new memory region to the end of our allocator's region list!
void *memPoolExtend(memoryPool *pool, void *memory, const size_t numBlocks){
	if(memory != NULL){
		memoryRegion *newRegion = memoryGetRegionFromBlocks(memory, numBlocks, pool->blockSize);
		//Add the new region to the end of the list!
		memoryRegionAppend(&pool->region, newRegion, memory);
		//Set up its memory!
		memPoolClearLastRegion(pool, newRegion);

		pool->nextFreeBlock = memPoolBlockFreeFlagGetNext(memory);
	}

	return(memory);
}


void memPoolDelete(memoryPool *pool){
	memoryAllocatorDelete(pool->region);
}