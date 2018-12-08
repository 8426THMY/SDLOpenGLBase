#include "memoryFreeList.h"


#include "utilTypes.h"


#warning "What if the beginning or end of a memory region is not aligned?"
#warning "We should go back to using the total memory size as input for init and extend, too."


void *memFreeListInit(memoryFreeList *freeList, void *memory, const size_t numBlocks, const size_t blockSize){
	//Make sure the user isn't being difficult.
	if(memory != NULL){
		memoryRegion *region;

		freeList->blockSize = memFreeListGetBlockSize(blockSize);

		region = memoryGetRegionFromBlocks(memory, numBlocks, freeList->blockSize);
		region->start = memory;
		region->next = NULL;
		memFreeListClearLastRegion(freeList, region);

		freeList->nextFreeBlock = memory;
		freeList->region = region;
	}

	return(memory);
}


void *memFreeListAlloc(memoryFreeList *freeList){
	void *newBlock = freeList->nextFreeBlock;

	//If a free block exists, change
	//the list's free block pointer!
	if(newBlock != NULL){
		freeList->nextFreeBlock = memFreeListBlockFreeGetNext(newBlock);
	}

	return(newBlock);
}


void memFreeListFree(memoryFreeList *freeList, void *data){
	//Make the block pointer to the next free
	//block in the list then add it to the front!
	memFreeListBlockFreeGetNext(data) = freeList->nextFreeBlock;
	freeList->nextFreeBlock = data;
}


//Initialise every block in a region, setting
//the last block's next pointer to "next".
void memFreeListClearRegion(memoryFreeList *freeList, memoryRegion *region, void *next){
	const size_t blockSize = freeList->blockSize;
	void *currentBlock = region->start;
	void *nextBlock = memFreeListBlockGetNextBlock(currentBlock, blockSize);

	//Set the next pointer for each block!
	while(nextBlock < (void *)region){
		memFreeListBlockFreeGetNext(currentBlock) = nextBlock;

		currentBlock = nextBlock;
		nextBlock = memFreeListBlockGetNextBlock(currentBlock, blockSize);
	}

	//Set the next pointer for the last block!
	memFreeListBlockFreeGetNext(currentBlock) = next;
}

//Initialise every block in a region, setting the
//flag to invalid and the next pointer to NULL.
void memFreeListClearLastRegion(memoryFreeList *freeList, memoryRegion *region){
	const size_t blockSize = freeList->blockSize;
	void *currentBlock = region->start;
	void *nextBlock = memFreeListBlockGetNextBlock(currentBlock, blockSize);

	//Set the next pointer for each block!
	while(nextBlock < (void *)region){
		memFreeListBlockFreeGetNext(currentBlock) = nextBlock;

		currentBlock = nextBlock;
		nextBlock = memFreeListBlockGetNextBlock(currentBlock, blockSize);
	}

	//Set the next pointer for the last block!
	memFreeListBlockFreeGetNext(currentBlock) = NULL;
}

void memFreeListClear(memoryFreeList *freeList){
	memoryRegion *region = freeList->region;
	freeList->nextFreeBlock = region->start;

	//Loop through every region in the allocator.
	while(1){
		memoryRegion *nextRegion = region->next;

		//If this is not the last region, make this region's
		//last block point to the first in the next region.
		if(nextRegion != NULL){
			memFreeListClearRegion(freeList, region, nextRegion->start);

		//Otherwise, make it point to NULL and break the loop.
		}else{
			memFreeListClearLastRegion(freeList, region);
			break;
		}

		region = nextRegion;
	}
}


void *memFreeListExtend(memoryFreeList *freeList, void *memory, const size_t numBlocks){
	if(memory != NULL){
		memoryRegion *newRegion = memoryGetRegionFromBlocks(memory, numBlocks, freeList->blockSize);
		//Add the new region to the end of the list!
		memoryRegionAppend(&freeList->region, newRegion, memory);
		//Set up its memory!
		memFreeListClearLastRegion(freeList, newRegion);

		freeList->nextFreeBlock = memory;
	}

	return(memory);
}


void memFreeListDelete(memoryFreeList *freeList){
	memoryAllocatorDelete(freeList->region);
}