#include "memoryFreeList.h"


#include "utilTypes.h"


/*
** Initialise every block in a region, setting the
** flag to invalid and the next pointer to NULL.
*/
void memFreeListInitRegion(memoryFreeList *const restrict freeList, memoryRegion *const restrict region){
	const size_t blockSize = freeList->blockSize;
	void *currentBlock = region->start;
	void *nextBlock = memFreeListBlockGetNextBlock(currentBlock, blockSize);

	// Set the next pointer for each block!
	while(nextBlock < (void *)region){
		memFreeListBlockFreeGetNext(currentBlock) = nextBlock;

		currentBlock = nextBlock;
		nextBlock = memFreeListBlockGetNextBlock(currentBlock, blockSize);
	}

	// Set the next pointer for the last block!
	memFreeListBlockFreeGetNext(currentBlock) = NULL;
}

void *memFreeListInit(memoryFreeList *const restrict freeList, void *const restrict memory, const size_t memorySize, const size_t blockSize){
	// Make sure the user isn't being difficult.
	if(memory != NULL){
		memoryRegion *region;

		freeList->blockSize = memFreeListGetBlockSize(blockSize);

		region = memoryGetRegionFromSize(memory, memorySize);
		region->start = memory;
		region->next = NULL;
		memFreeListInitRegion(freeList, region);

		freeList->nextFreeBlock = memory;
		#ifdef MEMFREELIST_COUNT_USED_BLOCKS
		freeList->usedBlocks = 0;
		#endif
		freeList->region = region;
	}

	return(memory);
}


void *memFreeListAlloc(memoryFreeList *const restrict freeList){
	void *const newBlock = freeList->nextFreeBlock;

	// If a free block exists, change
	// the list's free block pointer!
	if(newBlock != NULL){
		freeList->nextFreeBlock = memFreeListBlockFreeGetNext(newBlock);
		#ifdef MEMFREELIST_COUNT_USED_BLOCKS
		++freeList->usedBlocks;
		#endif
	}

	return(newBlock);
}


void memFreeListFree(memoryFreeList *const restrict freeList, void *const restrict data){
	// Make the block pointer to the next free
	// block in the list then add it to the front!
	memFreeListBlockFreeGetNext(data) = freeList->nextFreeBlock;
	#ifdef MEMFREELIST_COUNT_USED_BLOCKS
	--freeList->usedBlocks;
	#endif
	freeList->nextFreeBlock = data;
}

void memFreeListClear(memoryFreeList *const restrict freeList){
	const size_t blockSize = freeList->blockSize;
	memoryRegion *region = freeList->region;
	freeList->nextFreeBlock = region->start;
	#ifdef MEMFREELIST_COUNT_USED_BLOCKS
	freeList->usedBlocks = 0;
	#endif

	// Loop through every region in the allocator.
	for(;;){
		memoryRegion *const nextRegion = region->next;
		void *currentBlock = region->start;
		void *nextBlock = memFreeListBlockGetNextBlock(currentBlock, blockSize);

		// Set the next pointer for each block!
		do {
			memFreeListBlockFreeGetNext(currentBlock) = nextBlock;

			currentBlock = nextBlock;
			nextBlock = memFreeListBlockGetNextBlock(currentBlock, blockSize);
		} while(nextBlock < (void *)region);

		// Set the next pointer for the last block!
		if(nextRegion == NULL){
			memFreeListBlockFreeGetNext(currentBlock) = NULL;
			break;
		}else{
			memFreeListBlockFreeGetNext(currentBlock) = nextRegion->start;
		}
		region = nextRegion;
	}
}


#ifdef MEMORYREGION_EXTEND_ALLOCATORS
void *memFreeListExtend(memoryFreeList *const restrict freeList, void *const restrict memory, const size_t memorySize){
	if(memory != NULL){
		memoryRegion *const newRegion = memoryGetRegionFromSize(memory, memorySize);
		// Add the new region to the end of the list!
		memoryRegionAppend(&freeList->region, newRegion, memory);
		// Set up its memory!
		memFreeListInitRegion(freeList, newRegion);

		freeList->nextFreeBlock = memory;
	}

	return(memory);
}
#endif