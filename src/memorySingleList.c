#include "memorySingleList.h"


#include "utilTypes.h"


//These functions all do the same thing, but
//they, but make the code a bit easier to read.
#define memSingleListBlockFreeGetNext(block) memSingleListBlockGetValue(block)
#define memSingleListBlockUsedGetNext(block) memSingleListBlockGetValue(block)

//Write the flag specified by "flag" to the pointer "next".
#define memSingleListBlockSetFlag(next, flag) ((void *)(((uintptr_t)memSingleListBlockGetValue(next)) | (flag)))
//Add the "active" flag to the pointer specified by "next".
#define memSingleListBlockMakeActive(next) memSingleListBlockSetFlag(next, MEMSINGLELIST_FLAG_ACTIVE)
//Add the "inactive" flag to the pointer specified by "next".
#define memSingleListBlockMakeInactive(next) memSingleListBlockSetFlag(next, MEMSINGLELIST_FLAG_INACTIVE)
//Add the "invalid" flag to the pointer specified by "next".
#define memSingleListBlockMakeInvalid(next) memSingleListBlockSetFlag(next, MEMSINGLELIST_FLAG_INVALID)


#warning "What if the beginning or end of a memory region is not aligned?"
#warning "We should go back to using the total memory size as input for init and extend, too."


void *memSingleListInit(memorySingleList *singleList, void *memory, const size_t memorySize, const size_t blockSize){
	//Make sure the user isn't being difficult.
	if(memory != NULL){
		memoryRegion *region;

		singleList->blockSize = memSingleListGetBlockSize(blockSize);

		region = memoryGetRegionFromSize(memory, memorySize);
		region->start = memSingleListBlockFreeFlagGetNext(memory);
		region->next = NULL;
		memSingleListClearLastRegion(singleList, region);

		singleList->nextFreeBlock = memSingleListBlockFreeFlagGetNext(memory);
		singleList->region = region;
	}

	return(memory);
}


//Used to create a new array list.
void *memSingleListAlloc(memorySingleList *singleList){
	void *newBlock = singleList->nextFreeBlock;

	if(newBlock != NULL){
		//Move the free pointer to the next free block.
		singleList->nextFreeBlock = memSingleListBlockFreeGetNext(newBlock);

		//Change this block's pointer.
		*memSingleListBlockUsedDataGetNext(newBlock) = NULL;
	}

	return(newBlock);
}

/*
** Prepend a new block at the start of an array list.
** This assumes that "start" points to the beginning.
*/
void *memSingleListPrepend(memorySingleList *singleList, void **start){
	void *newBlock = singleList->nextFreeBlock;

	if(newBlock != NULL){
		//Move the free pointer to the next free block.
		singleList->nextFreeBlock = memSingleListBlockFreeGetNext(newBlock);

		//Make sure the new block points
		//to the start of the array list.
		if(start != NULL){
			*memSingleListBlockUsedDataGetNext(newBlock) = *start;
			*start = newBlock;

		//If there's no starting block,
		//just create a new array list.
		}else{
			*memSingleListBlockUsedDataGetNext(newBlock) = NULL;
		}
	}

	return(newBlock);
}

//Append a new block to the end of an array list.
void *memSingleListAppend(memorySingleList *singleList, void **start){
	void *newBlock = singleList->nextFreeBlock;

	if(newBlock != NULL){
		void **nextBlock = start;
		//Find the last block in the array list.
		while(*nextBlock != NULL){
			nextBlock = memSingleListBlockUsedDataGetNext(*nextBlock);
		}

		//Move the free pointer to the next free block.
		singleList->nextFreeBlock = memSingleListBlockFreeGetNext(newBlock);

		//Make our new block the last in the array list!
		*nextBlock = newBlock;
		*memSingleListBlockUsedDataGetNext(newBlock) = NULL;
	}

	return(newBlock);
}

//Prepend a new block before another in an array list.
void *memSingleListInsertBefore(memorySingleList *singleList, void **start, void *prevData){
	void *newBlock = singleList->nextFreeBlock;

	if(newBlock != NULL){
		//Move the free pointer to the next free block.
		singleList->nextFreeBlock = memSingleListBlockFreeGetNext(newBlock);

		//If a previous block exists, make it point to the new block.
		if(prevData != NULL){
			prevData = memSingleListBlockUsedDataGetNext(prevData);

			*memSingleListBlockUsedDataGetNext(newBlock) = memSingleListBlockUsedGetNext(prevData);
			memSingleListBlockUsedGetNext(prevData) = newBlock;

		//Otherwise, the new block should be at the beginning of the list.
		}else{
			*memSingleListBlockUsedDataGetNext(newBlock) = *start;
			//The pointer to the beginning of the
			//list should now point to the new block.
			*start = newBlock;
		}
	}

	return(newBlock);
}

//Append a new block after another in an array list.
void *memSingleListInsertAfter(memorySingleList *singleList, void **start, void *data){
	void *newBlock = singleList->nextFreeBlock;

	if(newBlock != NULL){
		//Move the free pointer to the next free block.
		singleList->nextFreeBlock = memSingleListBlockFreeGetNext(newBlock);

		//Fix up the previous block's pointer if it exists.
		if(data != NULL){
			data = memSingleListBlockUsedDataGetNext(data);

			*memSingleListBlockUsedDataGetNext(newBlock) = memSingleListBlockUsedGetNext(data);
			memSingleListBlockUsedGetNext(data) = newBlock;

		//Otherwise, use the starting block to insert the new one.
		}else{
			void *startBlock = *start;

			//If there's a starting block, insert the new one after it.
			if(startBlock != NULL){
				startBlock = memSingleListBlockUsedDataGetNext(startBlock);

				*memSingleListBlockUsedDataGetNext(newBlock) = memSingleListBlockUsedGetNext(startBlock);
				memSingleListBlockUsedGetNext(startBlock) = newBlock;

			//Otherwise, just start a new array list.
			}else{
				*memSingleListBlockUsedDataGetNext(newBlock) = NULL;
				*start = newBlock;
			}
		}
	}

	return(newBlock);
}


/*
** This function assumes that the user is looping
** through the list and thus knows the previous block.
*/
void memSingleListFree(memorySingleList *singleList, void **start, void *data, void *prevData){
	void *block = memSingleListBlockUsedDataGetNext(data);

	//If there is a previous block, fix its pointer.
	if(prevData != NULL){
		*memSingleListBlockUsedDataGetNext(prevData) = memSingleListBlockUsedGetNext(block);

	//Otherwise, set the start pointer if it was specified.
	}else if(start != NULL){
		*start = memSingleListBlockUsedGetNext(block);
	}

	//Now we can add the block to the free list!
	memSingleListBlockFreeGetFlag(block) = MEMSINGLELIST_FLAG_INACTIVE;
	memSingleListBlockFreeGetNext(data) = singleList->nextFreeBlock;
	singleList->nextFreeBlock = data;
}

//Free every block in a single array.
void memSingleListFreeArray(memorySingleList *singleList, void *start){
	if(start != NULL){
		void *block = start;
		do {
			void *blockNext = memSingleListBlockFreeGetNext(block);

			memSingleListBlockFreeGetFlag(memSingleListBlockFreeNextGetFlag(block)) = MEMSINGLELIST_FLAG_INACTIVE;
			memSingleListBlockFreeGetNext(block) = NULL;

			block = blockNext;
		} while(block != NULL);
	}
}


/*
** Initialise every block in a region, setting the flag
** to "flag" and the last block's next pointer to "next".
*/
void memSingleListClearRegion(memorySingleList *singleList, memoryRegion *region, const byte_t flag, void *next){
	const size_t blockSize = singleList->blockSize;
	void *currentBlock = region->start;
	void *nextBlock = memSingleListBlockGetNextBlock(currentBlock, blockSize);

	//Set the flag and next pointer for each block!
	while(nextBlock < (void *)region){
		*memSingleListBlockFreeNextGetFlag(currentBlock) = flag;
		memSingleListBlockFreeGetNext(currentBlock) = nextBlock;

		currentBlock = nextBlock;
		nextBlock = memSingleListBlockGetNextBlock(currentBlock, blockSize);
	}

	//Set the flag and next pointer for the last block!
	*memSingleListBlockFreeNextGetFlag(currentBlock) = flag;
	memSingleListBlockFreeGetNext(currentBlock) = next;
}

/*
** Initialise every block in a region, setting the flag
** to invalid and the last block's next pointer to NULL.
*/
void memSingleListClearLastRegion(memorySingleList *singleList, memoryRegion *region){
	const size_t blockSize = singleList->blockSize;
	void *currentBlock = region->start;
	void *nextBlock = memSingleListBlockGetNextBlock(currentBlock, blockSize);

	//Set the flag and next pointer for each block!
	while(nextBlock < (void *)region){
		*memSingleListBlockFreeNextGetFlag(currentBlock) = MEMSINGLELIST_FLAG_INVALID;
		memSingleListBlockFreeGetNext(currentBlock) = nextBlock;

		currentBlock = nextBlock;
		nextBlock = memSingleListBlockGetNextBlock(currentBlock, blockSize);
	}

	//Set the flag and next pointer for the last block!
	*memSingleListBlockFreeNextGetFlag(currentBlock) = MEMSINGLELIST_FLAG_INVALID;
	memSingleListBlockFreeGetNext(currentBlock) = NULL;
}

/*
** Clear every memory region in the allocator.
** This assumes that there is at least one region.
*/
void memSingleListClear(memorySingleList *singleList){
	memoryRegion *region = singleList->region;
	singleList->nextFreeBlock = region->start;

	//Loop through every region in the allocator.
	for(;;){
		memoryRegion *nextRegion = region->next;

		//If this is not the last region, make this region's
		//last block point to the first in the next region.
		if(nextRegion != NULL){
			memSingleListClearRegion(singleList, region, MEMSINGLELIST_FLAG_INVALID, nextRegion->start);

		//Otherwise, make it point to NULL and break the loop.
		}else{
			memSingleListClearLastRegion(singleList, region);
			break;
		}

		region = nextRegion;
	}
}


//Append a new memory region to the end of our allocator's region list!
void *memSingleListExtend(memorySingleList *singleList, void *memory, const size_t memorySize){
	if(memory != NULL){
		memoryRegion *newRegion = memoryGetRegionFromSize(memory, memorySize);
		//Add the new region to the end of the list!
		memoryRegionAppend(&singleList->region, newRegion, memory);
		//Set up its memory!
		memSingleListClearLastRegion(singleList, newRegion);

		singleList->nextFreeBlock = memSingleListBlockFreeFlagGetNext(memory);
	}

	return(memory);
}


void memSingleListDelete(memorySingleList *singleList){
	memoryRegion *region = singleList->region;
	//Free every memory region in the allocator.
	while(region != NULL){
		memoryRegion *next = region->next;

		memoryFree(memSingleListRegionStart(region));
		region = next;
	}
}