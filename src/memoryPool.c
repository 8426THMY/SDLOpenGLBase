#include "memoryPool.h"


#define MEMPOOL_FLAG_ACTIVE   0x00
#define MEMPOOL_FLAG_INACTIVE 0x01
//This is used if there are no active elements after the block.
#define MEMPOOL_FLAG_INVALID  0x02

//Get the value of the current segment pointed to by "block".
#define MEMPOOL_BLOCK_GET_VALUE(block) *((void **)block)
//These functions all do the same thing, but
//they, but make the code a bit easier to read.
#define MEMPOOL_BLOCK_GET_FLAG(block) *((byte_t *)block)
#define MEMPOOL_BLOCK_FREE_GET_NEXT(block) MEMPOOL_BLOCK_GET_VALUE(block)
#define MEMPOOL_BLOCK_USED_GET_NEXT(block) MEMPOOL_BLOCK_GET_VALUE(block)
#define MEMPOOL_BLOCK_USED_GET_DATA(block) MEMPOOL_BLOCK_GET_VALUE(block)

//Return the address of the next block in the pool.
#define MEMPOOL_BLOCK_GET_NEXT_BLOCK(block, size) ((void *)(((byte_t *)(block)) + (size)))
//Return the address of the previous block in the pool.
#define MEMPOOL_BLOCK_GET_PREV_BLOCK(block, size) ((void *)(((byte_t *)(block)) - (size)))

//Get the block's next pointer from its flags segment.
#define MEMPOOL_BLOCK_FREE_FLAG_GET_NEXT(block) ((void **)(((byte_t *)(block)) + MEMPOOL_BLOCK_HEADER_SIZE))
//Get the block's flags from its next pointer segment.
#define MEMPOOL_BLOCK_FREE_NEXT_GET_FLAG(block) (((byte_t *)(block)) - MEMPOOL_BLOCK_HEADER_SIZE)

//Get the block's data from its flags segment.
#define MEMPOOL_BLOCK_USED_FLAG_GET_DATA(block) ((void **)(((byte_t *)(block)) + MEMPOOL_BLOCK_HEADER_SIZE))
//Get the block's flags from its data segment.
#define MEMPOOL_BLOCK_USED_DATA_GET_FLAG(block) (((byte_t *)(block)) + MEMPOOL_BLOCK_HEADER_SIZE)

//Return whether or not the block is active.
#define MEMPOOL_BLOCK_IS_ACTIVE(block) (MEMPOOL_BLOCK_GET_FLAG(block) == MEMPOOL_FLAG_ACTIVE)
//Return whether or not the block is inactive.
#define MEMPOOL_BLOCK_IS_INACTIVE(block) (MEMPOOL_BLOCK_GET_FLAG(block) == MEMPOOL_FLAG_INACTIVE)
//Return whether or not the block is invalid.
#define MEMPOOL_BLOCK_IS_INVALID(block) (MEMPOOL_BLOCK_GET_FLAG(block) == MEMPOOL_FLAG_INVALID)

//Return whether or not the block is free.
#define MEMPOOL_BLOCK_IS_FREE(block) !MEMPOOL_BLOCK_IS_ACTIVE(block)
//Return whether or not the block is in use.
#define MEMPOOL_BLOCK_IS_USED(block) MEMPOOL_BLOCK_IS_ACTIVE(block)


void *memPoolInit(memoryPool *pool, void *memory, const size_t memorySize, const size_t blockSize){
	//Make sure the user isn't being difficult.
	if(memory != NULL){
		pool->blockSize = memPoolGetBlockSize(blockSize);
		pool->region = memoryGetRegion(memory, memorySize);
		pool->region->start = memory;
		pool->region->next = NULL;

		memPoolClear(pool);

	//Otherwise, just prevent them from allocating anything.
	}else{
		pool->blockSize = 0;
		pool->nextFreeBlock = NULL;
		pool->region = NULL;
	}

	return(memory);
}


void *memPoolAlloc(memoryPool *pool){
	void *newBlock = pool->nextFreeBlock;

	//Make sure the pool isn't full.
	if(newBlock != NULL){
		//Remove this node from the free list.
		pool->nextFreeBlock = MEMPOOL_BLOCK_FREE_GET_NEXT(newBlock);
		//Set its active flag.
		*MEMPOOL_BLOCK_USED_DATA_GET_FLAG(newBlock) = MEMPOOL_FLAG_ACTIVE;
	}

	return(newBlock);
}


void memPoolFree(memoryPool *pool, void *data){
	//Set the block's active flag and move
	//it to the beginning of the free list.
	*MEMPOOL_BLOCK_FREE_NEXT_GET_FLAG(data) = MEMPOOL_FLAG_INACTIVE;
	MEMPOOL_BLOCK_FREE_GET_NEXT(data) = pool->nextFreeBlock;
	pool->nextFreeBlock = data;
}

//Performs identically to the previous function, but if you're
//freeing the last used block, it will loop back through the
//memory region and invalidate every free block it finds.
void memPoolFreeInvalidate(memoryPool *pool, memoryRegion *region, void *data){
	void *block = MEMPOOL_BLOCK_FREE_NEXT_GET_FLAG(data);
	void *nextBlock = MEMPOOL_BLOCK_GET_NEXT_BLOCK(block, pool->blockSize);


	//If there is an invalid block after the one we're freeing,
	//make sure it and any inactive blocks before it are invalid.
	if(region->next == NULL && (nextBlock >= (void *)region || MEMPOOL_BLOCK_IS_INVALID(nextBlock))){
		void *memoryStart = region->start;

		MEMPOOL_BLOCK_GET_FLAG(block) = MEMPOOL_FLAG_INVALID;

		nextBlock = MEMPOOL_BLOCK_GET_PREV_BLOCK(nextBlock, pool->blockSize);
		//Make every consecutive inactive block
		//before the one we're freeing invalid.
		while(nextBlock >= memoryStart && MEMPOOL_BLOCK_IS_INACTIVE(nextBlock)){
			MEMPOOL_BLOCK_GET_FLAG(nextBlock) = MEMPOOL_FLAG_INVALID;
			nextBlock = MEMPOOL_BLOCK_GET_PREV_BLOCK(nextBlock, pool->blockSize);
		}

	//Otherwise, just make the block inactive.
	}else{
		MEMPOOL_BLOCK_GET_FLAG(block) = MEMPOOL_FLAG_INACTIVE;
	}


	//Move the block to the beginning of the free list.
	MEMPOOL_BLOCK_FREE_GET_NEXT(data) = pool->nextFreeBlock;
	pool->nextFreeBlock = data;
}


void *memPoolExtend(memoryPool *pool, memoryRegion *region, void *memory, const size_t memorySize){
	if(memory != NULL){
		//Add the new memory region to the allocator.
		memoryRegion *newRegion = memoryGetRegion(memory, memorySize);
		//If no region was specified, add it to the beginning of the list.
		if(region == NULL){
			//Setup the pointers and flags for each block.
			memPoolClearRegion(pool, newRegion, MEMPOOL_FLAG_INACTIVE);

			memoryRegionInsertBefore(&(pool->region), newRegion, memory);

		//Otherwise, insert the new region after the one specified.
		}else{
			//If we're inserting after the last region, we can use the invalid flag.
			if(region->next == NULL){
				memPoolClearRegion(pool, newRegion, MEMPOOL_FLAG_INVALID);

			//Otherwise, we'd better use the inactive flag.
			}else{
				memPoolClearRegion(pool, newRegion, MEMPOOL_FLAG_INACTIVE);
			}

			memoryRegionInsertAfter(region, newRegion, memory);
		}
	}

	return(memory);
}

void memPoolClearRegion(memoryPool *pool, memoryRegion *region, byte_t flag){
	void *currentBlock;
	void *nextBlock;
	void *oldFree = pool->nextFreeBlock;

	//Set the first block's flag.
	MEMPOOL_BLOCK_GET_FLAG(region->start) = flag;
	currentBlock = MEMPOOL_BLOCK_FREE_FLAG_GET_NEXT(region->start);
	//Set the next free block to the first block in the new region.
	pool->nextFreeBlock = currentBlock;

	nextBlock = MEMPOOL_BLOCK_GET_NEXT_BLOCK(currentBlock, pool->blockSize);
	//Make every block in the linked list point to the next.
	while(nextBlock < (void *)region){
		//Set the block's next pointer to the next block in the array.
		MEMPOOL_BLOCK_FREE_GET_NEXT(currentBlock) = nextBlock;
		currentBlock = nextBlock;
		//Make the next block invalid.
		*MEMPOOL_BLOCK_FREE_NEXT_GET_FLAG(currentBlock) = flag;

		nextBlock = MEMPOOL_BLOCK_GET_NEXT_BLOCK(nextBlock, pool->blockSize);
	}

	//We set the last block's pointer to the old next free block.
	MEMPOOL_BLOCK_FREE_GET_NEXT(currentBlock) = oldFree;
}

void memPoolClear(memoryPool *pool){
	memoryRegion *memoryEnd = pool->region;
	void *currentBlock = MEMPOOL_BLOCK_FREE_FLAG_GET_NEXT(memoryEnd->start);

	//The next free block should be the beginning of the first memory region.
	pool->nextFreeBlock = currentBlock;

	//Clear every block in every region.
	do {
		void *nextBlock;

		//This will make the last block of the previous region
		//point to the first block of the current one. This
		//value will be overwritten with the first region.
		MEMPOOL_BLOCK_FREE_GET_NEXT(currentBlock) = memoryEnd->start;
		MEMPOOL_BLOCK_GET_FLAG(memoryEnd->start) = MEMPOOL_FLAG_INVALID;
		currentBlock = MEMPOOL_BLOCK_FREE_FLAG_GET_NEXT(memoryEnd->start);

		nextBlock = MEMPOOL_BLOCK_GET_NEXT_BLOCK(currentBlock, pool->blockSize);
		//Make every block in the linked list point to the next.
		while(nextBlock < (void *)memoryEnd){
			//Set the block's next pointer to the next block in the array.
			MEMPOOL_BLOCK_FREE_GET_NEXT(currentBlock) = nextBlock;
			currentBlock = nextBlock;
			//Make the next block invalid.
			*MEMPOOL_BLOCK_FREE_NEXT_GET_FLAG(currentBlock) = MEMPOOL_FLAG_INVALID;

			nextBlock = MEMPOOL_BLOCK_GET_NEXT_BLOCK(nextBlock, pool->blockSize);
		}

		//Move to the next region.
		memoryEnd = memoryEnd->next;
	} while(memoryEnd != NULL);

	//We set the last block's pointer to NULL as nothing comes after it.
	MEMPOOL_BLOCK_FREE_GET_NEXT(currentBlock) = NULL;
}


void memPoolDelete(memoryPool *pool){
	memoryAllocatorDelete(pool->region);
}