#include "memoryFreeList.h"


#include "utilTypes.h"


//Get the block after the one specified.
#define MEMFREELIST_BLOCK_GET_NEXT_BLOCK(block, size) ((void *)(((byte_t *)(block)) + (size)))
//Get the block before the one specified.
#define MEMFREELIST_BLOCK_GET_PREV_BLOCK(block, size) ((void *)(((byte_t *)(block)) - (size)))

//Get the next pointer of a free block.
#define MEMFREELIST_BLOCK_FREE_GET_NEXT(block) *((void **)block)


void *memFreeListInit(memoryFreeList *freeList, void *memory, const size_t memorySize, const size_t blockSize){
	//Make sure the user isn't being difficult.
	if(memory != NULL){
		freeList->blockSize = memFreeListGetBlockSize(blockSize);
		freeList->region = memoryGetRegion(memory, memorySize);
		freeList->region->start = memory;
		freeList->region->next = NULL;

		memFreeListClear(freeList);

	//Otherwise, just prevent them from allocating anything.
	}else{
		freeList->blockSize = 0;
		freeList->nextFreeBlock = NULL;
		freeList->region = NULL;
	}

	return(memory);
}


void *memFreeListAlloc(memoryFreeList *freeList){
	void *newBlock = freeList->nextFreeBlock;

	//If a free block exists, change
	//the list's free block pointer!
	if(newBlock != NULL){
		freeList->nextFreeBlock = MEMFREELIST_BLOCK_FREE_GET_NEXT(newBlock);
	}

	return(newBlock);
}


void memFreeListFree(memoryFreeList *freeList, void *data){
	//Make the block pointer to the next free
	//block in the list then add it to the front!
	MEMFREELIST_BLOCK_FREE_GET_NEXT(data) = freeList->nextFreeBlock;
	freeList->nextFreeBlock = data;
}


void *memFreeListExtend(memoryFreeList *freeList, memoryRegion *region, void *memory, const size_t memorySize){
	if(memory != NULL){
		//Add the new memory region to the allocator.
		memoryRegion *newRegion = memoryGetRegion(memory, memorySize);
		//If no region was specified, add it to the beginning of the list.
		if(region == NULL){
			memoryRegionInsertBefore(&(freeList->region), newRegion, memory);

		//Otherwise, insert the new region after the one specified.
		}else{
			memoryRegionInsertAfter(region, newRegion, memory);
		}

		//Setup the pointers for each block.
		memFreeListClearRegion(freeList, newRegion);
	}

	return(memory);
}

void memFreeListClearRegion(memoryFreeList *freeList, memoryRegion *region){
	void *currentBlock = region->start;
	void *nextBlock;
	void *oldFree = freeList->nextFreeBlock;

	//The next free block should now be the beginning of the memory region.
	freeList->nextFreeBlock = currentBlock;

	nextBlock = MEMFREELIST_BLOCK_GET_NEXT_BLOCK(currentBlock, freeList->blockSize);
	//Make every block in the linked list point to the next.
	while(nextBlock < (void *)region){
		MEMFREELIST_BLOCK_FREE_GET_NEXT(currentBlock) = nextBlock;
		currentBlock = nextBlock;

		nextBlock = MEMFREELIST_BLOCK_GET_NEXT_BLOCK(currentBlock, freeList->blockSize);
	}
	//We set the last block's pointer to the next free block.
	MEMFREELIST_BLOCK_FREE_GET_NEXT(currentBlock) = oldFree;
}

void memFreeListClear(memoryFreeList *freeList){
	memoryRegion *memoryEnd = freeList->region;
	void *currentBlock = memoryEnd->start;

	//The next free block should be the beginning of the first memory region.
	freeList->nextFreeBlock = currentBlock;

	//Clear every block in every region.
	do {
		void *nextBlock;

		//This will make the last block of the previous region
		//point to the first block of the current one. This
		//value will be overwritten with the first region.
		MEMFREELIST_BLOCK_FREE_GET_NEXT(currentBlock) = memoryEnd->start;
		currentBlock = memoryEnd->start;

		nextBlock = MEMFREELIST_BLOCK_GET_NEXT_BLOCK(currentBlock, freeList->blockSize);
		//Make every block in the linked list point to the next.
		while(nextBlock < (void *)memoryEnd){
			MEMFREELIST_BLOCK_FREE_GET_NEXT(currentBlock) = nextBlock;
			currentBlock = nextBlock;

			nextBlock = MEMFREELIST_BLOCK_GET_NEXT_BLOCK(currentBlock, freeList->blockSize);
		}

		//Move to the next region.
		memoryEnd = memoryEnd->next;
	} while(memoryEnd != NULL);

	//We set the last block's pointer to NULL as nothing comes after it.
	MEMFREELIST_BLOCK_FREE_GET_NEXT(currentBlock) = NULL;
}


void memFreeListDelete(memoryFreeList *freeList){
	memoryAllocatorDelete(freeList->region);
}