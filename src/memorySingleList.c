#include "memorySingleList.h"


#include "utilTypes.h"


//We define the active flag as "0x00" so
//it's easier to loop through sub-arrays.
#define MEMSINGLELIST_FLAG_ACTIVE   0x00
#define MEMSINGLELIST_FLAG_INACTIVE 0x01
//This is used if there are no active elements after the block.
#define MEMSINGLELIST_FLAG_INVALID  0x02

//Get the value of the current segment pointed to by "block".
#define MEMSINGLELIST_BLOCK_GET_VALUE(block) *((void **)block)
//These functions all do the same thing, but
//they, but make the code a bit easier to read.
#define MEMSINGLELIST_BLOCK_FREE_GET_FLAG(block) *((uintptr_t *)block)
#define MEMSINGLELIST_BLOCK_FREE_GET_NEXT(block) MEMSINGLELIST_BLOCK_GET_VALUE(block)
#define MEMSINGLELIST_BLOCK_USED_GET_NEXT(block) MEMSINGLELIST_BLOCK_GET_VALUE(block)
#define MEMSINGLELIST_BLOCK_USED_GET_DATA(block) MEMSINGLELIST_BLOCK_GET_VALUE(block)

//Return the address of the next block in the list.
#define MEMSINGLELIST_BLOCK_GET_NEXT_BLOCK(block, size) ((void *)(((byte_t *)(block)) + (size)))
//Return the address of the previous block in the list.
#define MEMSINGLELIST_BLOCK_GET_PREV_BLOCK(block, size) ((void *)(((byte_t *)(block)) - (size)))

//Get the block's next pointer from its flags segment.
#define MEMSINGLELIST_BLOCK_FREE_FLAG_GET_NEXT(block) ((void **)(((byte_t *)(block)) + MEMSINGLELIST_BLOCK_FREE_FLAG_SIZE))
//Get the block's flags from its next pointer segment.
#define MEMSINGLELIST_BLOCK_FREE_NEXT_GET_FLAG(block) ((uintptr_t *)(((byte_t *)(block)) - MEMSINGLELIST_BLOCK_FREE_FLAG_SIZE))

//Get the block's data from its next pointer segment.
#define MEMSINGLELIST_BLOCK_USED_NEXT_GET_DATA(block) ((void **)(((byte_t *)(block)) + MEMSINGLELIST_BLOCK_USED_NEXT_SIZE))
//Get the block's next pointer from its data segment.
#define MEMSINGLELIST_BLOCK_USED_DATA_GET_NEXT(block) ((void **)(((byte_t *)(block)) - MEMSINGLELIST_BLOCK_USED_NEXT_SIZE))

//We'll need to remove the active flag from
//the pointer if we want to get its real value.
#define MEMSINGLELIST_BLOCK_GET_NEXT(next) ((void *)(((uintptr_t)MEMSINGLELIST_BLOCK_GET_VALUE(next)) & MEMORY_DATA_MASK))
//The active flag is stored in the
//least significant bit of the pointer.
#define MEMSINGLELIST_BLOCK_GET_FLAG(next) (((uintptr_t)MEMSINGLELIST_BLOCK_GET_VALUE(next)) & MEMORY_FLAG_MASK)

//Write the flag specified by "flag" to the pointer "next".
#define MEMSINGLELIST_BLOCK_SET_FLAG(next, flag) ((void *)(((uintptr_t)MEMSINGLELIST_BLOCK_GET_VALUE(next)) | (flag)))
//Add the "active" flag to the pointer specified by "next".
#define MEMSINGLELIST_BLOCK_MAKE_ACTIVE(next) MEMSINGLELIST_BLOCK_SET_FLAG(next, MEMSINGLELIST_FLAG_ACTIVE)
//Add the "inactive" flag to the pointer specified by "next".
#define MEMSINGLELIST_BLOCK_MAKE_INACTIVE(next) MEMSINGLELIST_BLOCK_SET_FLAG(next, MEMSINGLELIST_FLAG_INACTIVE)
//Add the "invalid" flag to the pointer specified by "next".
#define MEMSINGLELIST_BLOCK_MAKE_INVALID(next) MEMSINGLELIST_BLOCK_SET_FLAG(next, MEMSINGLELIST_FLAG_INVALID)

//Return whether or not the block is active.
#define MEMSINGLELIST_BLOCK_IS_ACTIVE(block) (MEMSINGLELIST_BLOCK_GET_FLAG(block) == MEMSINGLELIST_FLAG_ACTIVE)
//Return whether or not the block is inactive.
#define MEMSINGLELIST_BLOCK_IS_INACTIVE(block) (MEMSINGLELIST_BLOCK_FREE_GET_FLAG(block) == MEMSINGLELIST_FLAG_INACTIVE)
//Return whether or not the block is invalid.
#define MEMSINGLELIST_BLOCK_IS_INVALID(block) (MEMSINGLELIST_BLOCK_FREE_GET_FLAG(block) == MEMSINGLELIST_FLAG_INVALID)

//Return whether or not the block is free.
#define MEMSINGLELIST_BLOCK_IS_FREE(block) !MEMSINGLELIST_BLOCK_IS_ACTIVE(block)
//Return whether or not the block is in use.
#define MEMSINGLELIST_BLOCK_IS_USED(block) MEMSINGLELIST_BLOCK_IS_ACTIVE(block)


void *memSingleListInit(memorySingleList *singleList, void *memory, const size_t memorySize, const size_t blockSize){
	//Make sure the user isn't being difficult.
	if(memory != NULL){
		singleList->blockSize = memSingleListGetBlockSize(blockSize);
		singleList->region = memoryGetRegion(memory, memorySize);
		singleList->region->start = memory;
		singleList->region->next = NULL;

		memSingleListClear(singleList);

	//Otherwise, just prevent them from allocating anything.
	}else{
		singleList->blockSize = 0;
		singleList->nextFreeBlock = NULL;
		singleList->region = NULL;
	}

	return(memory);
}


//Used to create a new array list.
void *memSingleListAlloc(memorySingleList *singleList){
	void *newBlock = singleList->nextFreeBlock;

	if(newBlock != NULL){
		//Move the free pointer to the next free block.
		singleList->nextFreeBlock = MEMSINGLELIST_BLOCK_FREE_GET_NEXT(newBlock);

		//Change this block's pointer.
		*MEMSINGLELIST_BLOCK_USED_DATA_GET_NEXT(newBlock) = NULL;
	}

	return(newBlock);
}

//Prepend a new block at the start of an array list.
//This assumes that "start" points to the beginning.
void *memSingleListPrepend(memorySingleList *singleList, void **start){
	void *newBlock = singleList->nextFreeBlock;

	if(newBlock != NULL){
		//Move the free pointer to the next free block.
		singleList->nextFreeBlock = MEMSINGLELIST_BLOCK_FREE_GET_NEXT(newBlock);

		//Make sure the new block points
		//to the start of the array list.
		if(start != NULL){
			*MEMSINGLELIST_BLOCK_USED_DATA_GET_NEXT(newBlock) = *start;
			*start = newBlock;

		//If there's no starting block,
		//just create a new array list.
		}else{
			*MEMSINGLELIST_BLOCK_USED_DATA_GET_NEXT(newBlock) = NULL;
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
			nextBlock = MEMSINGLELIST_BLOCK_USED_DATA_GET_NEXT(*nextBlock);
		}

		//Move the free pointer to the next free block.
		singleList->nextFreeBlock = MEMSINGLELIST_BLOCK_FREE_GET_NEXT(newBlock);

		//Make our new block the last in the array list!
		*nextBlock = newBlock;
		*MEMSINGLELIST_BLOCK_USED_DATA_GET_NEXT(newBlock) = NULL;
	}

	return(newBlock);
}

//Prepend a new block before another in an array list.
void *memSingleListInsertBefore(memorySingleList *singleList, void **start, void *prevData){
	void *newBlock = singleList->nextFreeBlock;

	if(newBlock != NULL){
		//Move the free pointer to the next free block.
		singleList->nextFreeBlock = MEMSINGLELIST_BLOCK_FREE_GET_NEXT(newBlock);

		//If a previous block exists, make it point to the new block.
		if(prevData != NULL){
			prevData = MEMSINGLELIST_BLOCK_USED_DATA_GET_NEXT(prevData);

			*MEMSINGLELIST_BLOCK_USED_DATA_GET_NEXT(newBlock) = MEMSINGLELIST_BLOCK_USED_GET_NEXT(prevData);
			MEMSINGLELIST_BLOCK_USED_GET_NEXT(prevData) = newBlock;

		//Otherwise, the new block should be at the beginning of the list.
		}else{
			*MEMSINGLELIST_BLOCK_USED_DATA_GET_NEXT(newBlock) = *start;
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
		singleList->nextFreeBlock = MEMSINGLELIST_BLOCK_FREE_GET_NEXT(newBlock);

		//Fix up the previous block's pointer if it exists.
		if(data != NULL){
			data = MEMSINGLELIST_BLOCK_USED_DATA_GET_NEXT(data);

			*MEMSINGLELIST_BLOCK_USED_DATA_GET_NEXT(newBlock) = MEMSINGLELIST_BLOCK_USED_GET_NEXT(data);
			MEMSINGLELIST_BLOCK_USED_GET_NEXT(data) = newBlock;

		//Otherwise, use the starting block to insert the new one.
		}else{
			void *startBlock = *start;

			//If there's a starting block, insert the new one after it.
			if(startBlock != NULL){
				startBlock = MEMSINGLELIST_BLOCK_USED_DATA_GET_NEXT(startBlock);

				*MEMSINGLELIST_BLOCK_USED_DATA_GET_NEXT(newBlock) = MEMSINGLELIST_BLOCK_USED_GET_NEXT(startBlock);
				MEMSINGLELIST_BLOCK_USED_GET_NEXT(startBlock) = newBlock;

			//Otherwise, just start a new array list.
			}else{
				*MEMSINGLELIST_BLOCK_USED_DATA_GET_NEXT(newBlock) = NULL;
				*start = newBlock;
			}
		}
	}

	return(newBlock);
}


//This function assumes that the user is looping
//through the list and thus knows the previous block.
void memSingleListFree(memorySingleList *singleList, void **start, void *data, void *prevData){
	void *block = MEMSINGLELIST_BLOCK_USED_DATA_GET_NEXT(data);

	//If there is a previous block, fix its pointer.
	if(prevData != NULL){
		*MEMSINGLELIST_BLOCK_USED_DATA_GET_NEXT(prevData) = MEMSINGLELIST_BLOCK_USED_GET_NEXT(block);
	}else{
		*start = MEMSINGLELIST_BLOCK_USED_GET_NEXT(block);
	}

	//Now we can add the block to the free list!
	MEMSINGLELIST_BLOCK_FREE_GET_FLAG(block) = MEMSINGLELIST_FLAG_INACTIVE;
	MEMSINGLELIST_BLOCK_FREE_GET_NEXT(data) = singleList->nextFreeBlock;
	singleList->nextFreeBlock = data;
}

//Performs identically to the previous function, but if you're
//freeing the last used block, it will loop back through the
//memory region and invalidate every free block it finds.
void memSingleListFreeInvalidate(memorySingleList *singleList, memoryRegion *region, void **start, void *data, void *prevData){
	void *block = MEMSINGLELIST_BLOCK_USED_DATA_GET_NEXT(data);
	void *nextBlock = MEMSINGLELIST_BLOCK_GET_NEXT_BLOCK(block, singleList->blockSize);

	//If there is a previous block, fix its pointer.
	if(prevData != NULL){
		*MEMSINGLELIST_BLOCK_USED_DATA_GET_NEXT(prevData) = MEMSINGLELIST_BLOCK_USED_GET_NEXT(block);
	}else{
		*start = MEMSINGLELIST_BLOCK_USED_GET_NEXT(block);
	}

	//We should only make the blocks inactive if this is the last region.
	//We can only start invalidating blocks if the one after the block
	//we're freeing is invalid or we're freeing the last block in the list.
	if(region->next == NULL && (nextBlock >= (void *)region || MEMSINGLELIST_BLOCK_IS_INVALID(nextBlock))){
		void *memoryStart = region->start;

		MEMSINGLELIST_BLOCK_FREE_GET_FLAG(block) = MEMSINGLELIST_FLAG_INVALID;

		nextBlock = MEMSINGLELIST_BLOCK_GET_PREV_BLOCK(nextBlock, singleList->blockSize);
		//Make every consecutive inactive block
		//before the one we're freeing invalid.
		while(nextBlock >= memoryStart && MEMSINGLELIST_BLOCK_IS_INACTIVE(nextBlock)){
			MEMSINGLELIST_BLOCK_FREE_GET_FLAG(nextBlock) = MEMSINGLELIST_FLAG_INVALID;
			nextBlock = MEMSINGLELIST_BLOCK_GET_PREV_BLOCK(nextBlock, singleList->blockSize);
		}

	//Otherwise, just make the block inactive.
	}else{
		MEMSINGLELIST_BLOCK_FREE_GET_FLAG(block) = MEMSINGLELIST_FLAG_INVALID;
	}


	//Now we can add the block to the free list!
	MEMSINGLELIST_BLOCK_FREE_GET_NEXT(data) = singleList->nextFreeBlock;
	singleList->nextFreeBlock = data;
}


void *memSingleListExtend(memorySingleList *singleList, memoryRegion *region, void *memory, const size_t memorySize){
	if(memory != NULL){
		//Add the new memory region to the allocator.
		memoryRegion *newRegion = memoryGetRegion(memory, memorySize);

		//If no region was specified, add it to the beginning of the list.
		if(region == NULL){
			//Setup the pointers and flags for each block.
			memSingleListClearRegion(singleList, newRegion, MEMSINGLELIST_FLAG_INACTIVE);

			memoryRegionInsertBefore(&(singleList->region), newRegion, memory);

		//Otherwise, insert the new region after the one specified.
		}else{
			//If we're inserting after the last region, we can use the invalid flag.
			if(region->next == NULL){
				memSingleListClearRegion(singleList, newRegion, MEMSINGLELIST_FLAG_INVALID);

			//Otherwise, we'd better use the inactive flag.
			}else{
				memSingleListClearRegion(singleList, newRegion, MEMSINGLELIST_FLAG_INACTIVE);
			}

			memoryRegionInsertAfter(region, newRegion, memory);
		}
	}

	return(memory);
}

void memSingleListClearRegion(memorySingleList *singleList, memoryRegion *region, uintptr_t flag){
	void *currentBlock;
	void *nextBlock;
	void *oldFree = singleList->nextFreeBlock;

	//Set the first block's flag.
	MEMSINGLELIST_BLOCK_FREE_GET_FLAG(region->start) = flag;
	currentBlock = MEMSINGLELIST_BLOCK_FREE_FLAG_GET_NEXT(region->start);
	//Set the next free block to the first block in the new region.
	singleList->nextFreeBlock = currentBlock;

	nextBlock = MEMSINGLELIST_BLOCK_GET_NEXT_BLOCK(currentBlock, singleList->blockSize);
	//Make every block in the linked list point to the next.
	while(nextBlock < (void *)region){
		//Set the block's next pointer to the next block in the array.
		MEMSINGLELIST_BLOCK_FREE_GET_NEXT(currentBlock) = nextBlock;
		currentBlock = nextBlock;
		//Make the next block invalid.
		*MEMSINGLELIST_BLOCK_FREE_NEXT_GET_FLAG(currentBlock) = flag;

		nextBlock = MEMSINGLELIST_BLOCK_GET_NEXT_BLOCK(nextBlock, singleList->blockSize);
	}

	//We set the last block's pointer to the old next free block.
	MEMSINGLELIST_BLOCK_FREE_GET_NEXT(currentBlock) = oldFree;
}

void memSingleListClear(memorySingleList *singleList){
	memoryRegion *memoryEnd = singleList->region;
	void *currentBlock = MEMSINGLELIST_BLOCK_FREE_FLAG_GET_NEXT(memoryEnd->start);

	//The next free block should be the beginning of the first memory region.
	singleList->nextFreeBlock = currentBlock;

	//Clear every block in every region.
	do {
		void *nextBlock;

		//This will make the last block of the previous region
		//point to the first block of the current one. This
		//value will be overwritten with the first region.
		MEMSINGLELIST_BLOCK_FREE_GET_NEXT(currentBlock) = memoryEnd->start;
		MEMSINGLELIST_BLOCK_FREE_GET_FLAG(memoryEnd->start) = MEMSINGLELIST_FLAG_INVALID;
		currentBlock = MEMSINGLELIST_BLOCK_FREE_FLAG_GET_NEXT(memoryEnd->start);

		nextBlock = MEMSINGLELIST_BLOCK_GET_NEXT_BLOCK(currentBlock, singleList->blockSize);
		//Make every block in the linked list point to the next.
		while(nextBlock < (void *)memoryEnd){
			//Set the block's next pointer to the next block in the array.
			MEMSINGLELIST_BLOCK_FREE_GET_NEXT(currentBlock) = nextBlock;
			currentBlock = nextBlock;
			//Make the next block invalid.
			*MEMSINGLELIST_BLOCK_FREE_NEXT_GET_FLAG(currentBlock) = MEMSINGLELIST_FLAG_INVALID;

			nextBlock = MEMSINGLELIST_BLOCK_GET_NEXT_BLOCK(nextBlock, singleList->blockSize);
		}

		//Move to the next region.
		memoryEnd = memoryEnd->next;
	} while(memoryEnd != NULL);

	//We set the last block's pointer to NULL as nothing comes after it.
	MEMSINGLELIST_BLOCK_FREE_GET_NEXT(currentBlock) = NULL;
}


void memSingleListDelete(memorySingleList *singleList){
	memoryAllocatorDelete(singleList->region);
}