#include "memoryFreeList.h"


#include "utilTypes.h"


#warning "What if the beginning or end of a memory region is not aligned?"
#warning "We should go back to using the total memory size as input for init and extend, too."


void *memFreeListInit(memoryFreeList *const restrict freeList, void *const restrict memory, const size_t memorySize, const size_t blockSize){
	// Make sure the user isn't being difficult.
	if(memory != NULL){
		memoryRegion *region;

		freeList->blockSize = memFreeListGetBlockSize(blockSize);

		region = memoryGetRegionFromSize(memory, memorySize);
		region->start = memory;
		region->next = NULL;
		memFreeListClearLastRegion(freeList, region);

		freeList->nextFreeBlock = memory;
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
	}

	return(newBlock);
}


void memFreeListFree(memoryFreeList *const restrict freeList, void *const restrict data){
	// Make the block pointer to the next free
	// block in the list then add it to the front!
	memFreeListBlockFreeGetNext(data) = freeList->nextFreeBlock;
	freeList->nextFreeBlock = data;
}


/*
** Initialise every block in a region, setting
** the last block's next pointer to "next".
*/
void memFreeListClearRegion(memoryFreeList *freeList, memoryRegion *const restrict region, void *const restrict next){
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
	memFreeListBlockFreeGetNext(currentBlock) = next;
}

/*
** Initialise every block in a region, setting the
** flag to invalid and the next pointer to NULL.
*/
void memFreeListClearLastRegion(memoryFreeList *const restrict freeList, memoryRegion *const restrict region){
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

void memFreeListClear(memoryFreeList *const restrict freeList){
	memoryRegion *region = freeList->region;
	freeList->nextFreeBlock = region->start;

	// Loop through every region in the allocator.
	for(;;){
		memoryRegion *nextRegion = region->next;

		// If this is not the last region, make this region's
		// last block point to the first in the next region.
		if(nextRegion != NULL){
			memFreeListClearRegion(freeList, region, nextRegion->start);

		// Otherwise, make it point to NULL and break the loop.
		}else{
			memFreeListClearLastRegion(freeList, region);
			break;
		}

		region = nextRegion;
	}
}


void *memFreeListExtend(memoryFreeList *const restrict freeList, void *const restrict memory, const size_t memorySize){
	if(memory != NULL){
		memoryRegion *newRegion = memoryGetRegionFromSize(memory, memorySize);
		// Add the new region to the end of the list!
		memoryRegionAppend(&freeList->region, newRegion, memory);
		// Set up its memory!
		memFreeListClearLastRegion(freeList, newRegion);

		freeList->nextFreeBlock = memory;
	}

	return(memory);
}


void memFreeListDelete(memoryFreeList *const restrict freeList){
	memoryAllocatorDelete(freeList->region);
}