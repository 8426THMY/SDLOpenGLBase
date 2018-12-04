#include "memoryDoubleList.h"


#include "utilTypes.h"


//We define the active flag as "0x00" so
//it's easier to loop through sub-arrays.
#define MEMDOUBLELIST_FLAG_ACTIVE   0x00
#define MEMDOUBLELIST_FLAG_INACTIVE 0x01
//This is used if there are no active elements after the block.
#define MEMDOUBLELIST_FLAG_INVALID  0x02

//Get the value of the current segment pointed to by "block".
#define MEMDOUBLELIST_BLOCK_GET_VALUE(block) *((void **)block)
//These functions all do the same thing, but
//they, but make the code a bit easier to read.
#define MEMDOUBLELIST_BLOCK_FREE_GET_FLAG(block) *((uintptr_t *)block)
#define MEMDOUBLELIST_BLOCK_FREE_GET_NEXT(block) MEMDOUBLELIST_BLOCK_GET_VALUE(block)
#define MEMDOUBLELIST_BLOCK_USED_GET_NEXT(block) MEMDOUBLELIST_BLOCK_GET_VALUE(block)
#define MEMDOUBLELIST_BLOCK_USED_GET_PREV(block) MEMDOUBLELIST_BLOCK_GET_VALUE(block)
#define MEMDOUBLELIST_BLOCK_USED_GET_DATA(block) MEMDOUBLELIST_BLOCK_GET_VALUE(block)

//Return the address of the next block in the list.
#define MEMDOUBLELIST_BLOCK_GET_NEXT_BLOCK(block, size) ((void *)(((byte_t *)(block)) + (size)))
//Return the address of the previous block in the list.
#define MEMDOUBLELIST_BLOCK_GET_PREV_BLOCK(block, size) ((void *)(((byte_t *)(block)) - (size)))

//Get the block's next pointer from its flags segment.
#define MEMDOUBLELIST_BLOCK_FREE_FLAG_GET_NEXT(block) ((void **)(((byte_t *)(block)) + MEMDOUBLELIST_BLOCK_HEADER_SIZE))
//Get the block's flags from its next pointer segment.
#define MEMDOUBLELIST_BLOCK_FREE_NEXT_GET_FLAG(block) ((uintptr_t *)(((byte_t *)(block)) - MEMDOUBLELIST_BLOCK_HEADER_SIZE))

//Get the block's data from its next pointer segment.
#define MEMDOUBLELIST_BLOCK_USED_NEXT_GET_PREV(block) ((void **)(((byte_t *)(block)) + MEMDOUBLELIST_BLOCK_USED_NEXT_SIZE))
//Get the block's previous pointer from its next pointer segment.
#define MEMDOUBLELIST_BLOCK_USED_NEXT_GET_DATA(block) ((void **)(((byte_t *)(block)) + MEMDOUBLELIST_BLOCK_HEADER_SIZE))
//Get the block's next pointer from its previous pointer segment.
#define MEMDOUBLELIST_BLOCK_USED_PREV_GET_NEXT(block) ((void **)(((byte_t *)(block)) - MEMDOUBLELIST_BLOCK_USED_NEXT_SIZE))
//Get the block's data from its previous pointer segment.
#define MEMDOUBLELIST_BLOCK_USED_PREV_GET_DATA(block) ((void **)(((byte_t *)(block)) + MEMDOUBLELIST_BLOCK_USED_PREV_SIZE))
//Get the block's next pointer from its data segment.
#define MEMDOUBLELIST_BLOCK_USED_DATA_GET_NEXT(block) ((void **)(((byte_t *)(block)) - MEMDOUBLELIST_BLOCK_HEADER_SIZE))
//Get the block's previous pointer from its data segment.
#define MEMDOUBLELIST_BLOCK_USED_DATA_GET_PREV(block) ((void **)(((byte_t *)(block)) - MEMDOUBLELIST_BLOCK_USED_PREV_SIZE))

//We'll need to remove the active flag from
//the pointer if we want to get its real value.
#define MEMDOUBLELIST_BLOCK_GET_NEXT(next) ((void *)(((uintptr_t)MEMDOUBLELIST_BLOCK_GET_VALUE(next)) & MEMORY_DATA_MASK))
//The active flag is stored in the
//least significant bit of the pointer.
#define MEMDOUBLELIST_BLOCK_GET_FLAG(next) (((uintptr_t)MEMDOUBLELIST_BLOCK_GET_VALUE(next)) & MEMORY_FLAG_MASK)

//Write the flag specified by "flag" to the pointer "next".
#define MEMDOUBLELIST_BLOCK_SET_FLAG(next, flag) ((void *)(((uintptr_t)MEMDOUBLELIST_BLOCK_GET_VALUE(next)) | (flag)))
//Add the "active" flag to the pointer specified by "next".
#define MEMDOUBLELIST_BLOCK_MAKE_ACTIVE(next) MEMDOUBLELIST_BLOCK_SET_FLAG(next, MEMDOUBLELIST_FLAG_ACTIVE)
//Add the "inactive" flag to the pointer specified by "next".
#define MEMDOUBLELIST_BLOCK_MAKE_INACTIVE(next) MEMDOUBLELIST_BLOCK_SET_FLAG(next, MEMDOUBLELIST_FLAG_INACTIVE)
//Add the "invalid" flag to the pointer specified by "next".
#define MEMDOUBLELIST_BLOCK_MAKE_INVALID(next) MEMDOUBLELIST_BLOCK_SET_FLAG(next, MEMDOUBLELIST_FLAG_INVALID)

//Return whether or not the block is active.
#define MEMDOUBLELIST_BLOCK_IS_ACTIVE(block) (MEMDOUBLELIST_BLOCK_GET_FLAG(block) == MEMDOUBLELIST_FLAG_ACTIVE)
//Return whether or not the block is inactive.
#define MEMDOUBLELIST_BLOCK_IS_INACTIVE(block) (MEMDOUBLELIST_BLOCK_FREE_GET_FLAG(block) == MEMDOUBLELIST_FLAG_INACTIVE)
//Return whether or not the block is invalid.
#define MEMDOUBLELIST_BLOCK_IS_INVALID(block) (MEMDOUBLELIST_BLOCK_FREE_GET_FLAG(block) == MEMDOUBLELIST_FLAG_INVALID)

//Return whether or not the block is free.
#define MEMDOUBLELIST_BLOCK_IS_FREE(block) !MEMDOUBLELIST_BLOCK_IS_ACTIVE(block)
//Return whether or not the block is in use.
#define MEMDOUBLELIST_BLOCK_IS_USED(block) MEMDOUBLELIST_BLOCK_IS_ACTIVE(block)


void *memDoubleListInit(memoryDoubleList *doubleList, void *memory, const size_t memorySize, const size_t blockSize){
	//Make sure the user isn't being difficult.
	if(memory != NULL){
		doubleList->blockSize = memDoubleListGetBlockSize(blockSize);
		doubleList->region = memoryGetRegion(memory, memorySize);
		doubleList->region->start = memory;
		doubleList->region->next = NULL;

		memDoubleListClear(doubleList);

	//Otherwise, just prevent them from allocating anything.
	}else{
		doubleList->blockSize = 0;
		doubleList->nextFreeBlock = NULL;
		doubleList->region = NULL;
	}

	return(memory);
}


//Used to create a new array list.
void *memDoubleListAlloc(memoryDoubleList *doubleList){
	void *newBlock = doubleList->nextFreeBlock;

	if(newBlock != NULL){
		//Move the free pointer to the next free block.
		doubleList->nextFreeBlock = MEMDOUBLELIST_BLOCK_FREE_GET_NEXT(newBlock);

		//Change this block's pointers.
		*MEMDOUBLELIST_BLOCK_USED_DATA_GET_PREV(newBlock) = NULL;
		*MEMDOUBLELIST_BLOCK_USED_DATA_GET_NEXT(newBlock) = NULL;
	}

	return(newBlock);
}

//Prepend a new block at the start of an array list.
void *memDoubleListPrepend(memoryDoubleList *doubleList, void **start){
	void *newBlock = doubleList->nextFreeBlock;

	if(newBlock != NULL){
		//Move the free pointer to the next free block.
		doubleList->nextFreeBlock = MEMDOUBLELIST_BLOCK_FREE_GET_NEXT(newBlock);

		//Make sure the new block points
		//to the start of the array list.
		if(start != NULL){
			*MEMDOUBLELIST_BLOCK_USED_DATA_GET_PREV(newBlock) = NULL;
			*MEMDOUBLELIST_BLOCK_USED_DATA_GET_NEXT(newBlock) = *start;
			*start = newBlock;
		}else{
			*MEMDOUBLELIST_BLOCK_USED_DATA_GET_PREV(newBlock) = NULL;
			*MEMDOUBLELIST_BLOCK_USED_DATA_GET_NEXT(newBlock) = NULL;
		}
	}

	return(newBlock);
}

//Append a new block after another in an array list.
void *memDoubleListAppend(memoryDoubleList *doubleList, void **start){
	void *newBlock = doubleList->nextFreeBlock;

	if(newBlock != NULL){
		void *lastBlock = NULL;
		void **nextBlock = start;
		//Find the last block in the array list.
		while(*nextBlock != NULL){
			lastBlock = *nextBlock;
			nextBlock = MEMDOUBLELIST_BLOCK_USED_DATA_GET_NEXT(*nextBlock);
		}

		//Move the free pointer to the next free block.
		doubleList->nextFreeBlock = MEMDOUBLELIST_BLOCK_FREE_GET_NEXT(newBlock);

		//Make our new block the last in the array list!
		*nextBlock = newBlock;
		*MEMDOUBLELIST_BLOCK_USED_DATA_GET_PREV(newBlock) = lastBlock;
		*MEMDOUBLELIST_BLOCK_USED_DATA_GET_NEXT(newBlock) = NULL;
	}

	return(newBlock);
}

//Prepend a new block before another in an array list.
void *memoryDoubleListInsertBefore(memoryDoubleList *doubleList, void **start, void *prevData){
	void *newBlock = doubleList->nextFreeBlock;

	if(newBlock != NULL){
		//Move the free pointer to the next free block.
		doubleList->nextFreeBlock = MEMDOUBLELIST_BLOCK_FREE_GET_NEXT(newBlock);

		//Make the new block point back to the one before it.
		*MEMDOUBLELIST_BLOCK_USED_DATA_GET_PREV(newBlock) = prevData;

		//If a previous block exists, make it point to the new block.
		if(prevData != NULL){
			void *block;

			prevData = MEMDOUBLELIST_BLOCK_USED_DATA_GET_NEXT(prevData);
			block = MEMDOUBLELIST_BLOCK_USED_GET_NEXT(prevData);

			//Make the block we're inserting after point to the new block.
			MEMDOUBLELIST_BLOCK_USED_GET_NEXT(prevData) = newBlock;

			//Make the block we're inserting before point back to the new block.
			if(block != NULL){
				*MEMDOUBLELIST_BLOCK_USED_DATA_GET_PREV(block) = newBlock;
			}

			//Make the new block point to the one after it.
			*MEMDOUBLELIST_BLOCK_USED_DATA_GET_NEXT(newBlock) = block;

		//Otherwise, the new block should be at the beginning of the list.
		}else{
			void *startBlock = *start;

			*MEMDOUBLELIST_BLOCK_USED_DATA_GET_NEXT(newBlock) = startBlock;

			//Make sure the old start block points back to the new one.
			if(startBlock != NULL){
				*MEMDOUBLELIST_BLOCK_USED_DATA_GET_PREV(startBlock) = newBlock;
			}
			//The pointer to the beginning of the
			//list should now point to the new block.
			*start = newBlock;
		}
	}

	return(newBlock);
}

//Append a new block after another in an array list.
void *memoryDoubleListInsertAfter(memoryDoubleList *doubleList, void **start, void *data){
	void *newBlock = doubleList->nextFreeBlock;

	if(newBlock != NULL){
		//Move the free pointer to the next free block.
		doubleList->nextFreeBlock = MEMDOUBLELIST_BLOCK_FREE_GET_NEXT(newBlock);

		//Make the new block point back to the one before it.
		*MEMDOUBLELIST_BLOCK_USED_DATA_GET_PREV(newBlock) = data;

		//Fix up the previous block's pointer if it exists.
		if(data != NULL){
			void *nextBlock;

			data = MEMDOUBLELIST_BLOCK_USED_DATA_GET_NEXT(data);
			nextBlock = MEMDOUBLELIST_BLOCK_USED_GET_NEXT(data);

			//Make the block we're inserting after point to the new block.
			MEMDOUBLELIST_BLOCK_USED_GET_NEXT(data) = newBlock;

			//Make the block we're inserting before point back to the new block.
			if(nextBlock != NULL){
				*MEMDOUBLELIST_BLOCK_USED_DATA_GET_PREV(nextBlock) = newBlock;
			}

			//Make the new block point to the one after it.
			*MEMDOUBLELIST_BLOCK_USED_DATA_GET_NEXT(newBlock) = nextBlock;

		//Otherwise, use the starting block to insert the new one.
		}else{
			void *startBlock = *start;

			//If there's a starting block, insert the new one after it.
			if(startBlock != NULL){
				startBlock = MEMDOUBLELIST_BLOCK_USED_DATA_GET_NEXT(startBlock);

				*MEMDOUBLELIST_BLOCK_USED_DATA_GET_NEXT(newBlock) = MEMDOUBLELIST_BLOCK_USED_GET_NEXT(startBlock);
				MEMDOUBLELIST_BLOCK_USED_GET_NEXT(startBlock) = newBlock;

			//Otherwise, just start a new array list.
			}else{
				*MEMDOUBLELIST_BLOCK_USED_DATA_GET_NEXT(newBlock) = NULL;
				*start = newBlock;
			}
		}
	}

	return(newBlock);
}


void memDoubleListFree(memoryDoubleList *doubleList, void **start, void *data){
	void *block = MEMDOUBLELIST_BLOCK_USED_DATA_GET_NEXT(data);

	void *nextBlock = MEMDOUBLELIST_BLOCK_USED_GET_NEXT(block);
	void *prevBlock = *MEMDOUBLELIST_BLOCK_USED_NEXT_GET_PREV(block);
	//We'll need to fix the pointers for
	//any preceding or succeeding nodes.
	if(prevBlock != NULL){
		*MEMDOUBLELIST_BLOCK_USED_DATA_GET_NEXT(prevBlock) = nextBlock;
	}else{
		*start = nextBlock;
	}
	if(nextBlock != NULL){
		*MEMDOUBLELIST_BLOCK_USED_DATA_GET_PREV(nextBlock) = prevBlock;
	}

	//Now we can add the block to the free list!
	MEMDOUBLELIST_BLOCK_FREE_GET_FLAG(block) = MEMDOUBLELIST_FLAG_INACTIVE;
	MEMDOUBLELIST_BLOCK_FREE_GET_NEXT(data) = doubleList->nextFreeBlock;
	doubleList->nextFreeBlock = data;
}

//Performs identically to the previous function, but if you're
//freeing the last used block, it will loop back through the
//memory region and invalidate every free block it finds.
void memDoubleListFreeInvalidate(memoryDoubleList *doubleList, memoryRegion *region, void **start, void *data){
	void *block = MEMDOUBLELIST_BLOCK_USED_DATA_GET_NEXT(data);

	void *nextBlock = MEMDOUBLELIST_BLOCK_USED_GET_NEXT(block);
	void *prevBlock = *MEMDOUBLELIST_BLOCK_USED_NEXT_GET_PREV(block);
	//We'll need to fix the pointers for
	//any preceding or succeeding nodes.
	if(prevBlock != NULL){
		*MEMDOUBLELIST_BLOCK_USED_DATA_GET_NEXT(prevBlock) = nextBlock;
	}else{
		*start = nextBlock;
	}
	if(nextBlock != NULL){
		*MEMDOUBLELIST_BLOCK_USED_DATA_GET_PREV(nextBlock) = prevBlock;
	}


	nextBlock = MEMDOUBLELIST_BLOCK_GET_NEXT_BLOCK(block, doubleList->blockSize);
	//We should only make the blocks inactive if this is the last region.
	//We can only start invalidating blocks if the one after the block
	//we're freeing is invalid or we're freeing the last block in the list.
	if(region->next == NULL && (nextBlock >= (void *)region || MEMDOUBLELIST_BLOCK_IS_INVALID(nextBlock))){
		void *memoryStart = region->start;

		MEMDOUBLELIST_BLOCK_FREE_GET_FLAG(block) = MEMDOUBLELIST_FLAG_INVALID;

		nextBlock = MEMDOUBLELIST_BLOCK_GET_PREV_BLOCK(block, doubleList->blockSize);
		//Make every consecutive inactive block
		//before the one we're freeing invalid.
		while(nextBlock >= memoryStart && MEMDOUBLELIST_BLOCK_IS_INACTIVE(nextBlock)){
			MEMDOUBLELIST_BLOCK_FREE_GET_FLAG(nextBlock) = MEMDOUBLELIST_FLAG_INVALID;
			nextBlock = MEMDOUBLELIST_BLOCK_GET_PREV_BLOCK(nextBlock, doubleList->blockSize);
		}

	//Otherwise, just make the block inactive.
	}else{
		MEMDOUBLELIST_BLOCK_FREE_GET_FLAG(block) = MEMDOUBLELIST_FLAG_INACTIVE;
	}


	//Now we can add the block to the free list!
	MEMDOUBLELIST_BLOCK_FREE_GET_NEXT(data) = doubleList->nextFreeBlock;
	doubleList->nextFreeBlock = data;
}


void *memDoubleListExtend(memoryDoubleList *doubleList, memoryRegion *region, void *memory, const size_t memorySize){
	if(memory != NULL){
		//Add the new memory region to the allocator.
		memoryRegion *newRegion = memoryGetRegion(memory, memorySize);

		//If no region was specified, add it to the beginning of the list.
		if(region == NULL){
			//Setup the pointers and flags for each block.
			memDoubleListClearRegion(doubleList, newRegion, MEMDOUBLELIST_FLAG_INACTIVE);

			memoryRegionInsertBefore(&(doubleList->region), newRegion, memory);

		//Otherwise, insert the new region after the one specified.
		}else{
			//If we're inserting after the last region, we can use the invalid flag.
			if(region->next == NULL){
				memDoubleListClearRegion(doubleList, newRegion, MEMDOUBLELIST_FLAG_INVALID);

			//Otherwise, we'd better use the inactive flag.
			}else{
				memDoubleListClearRegion(doubleList, newRegion, MEMDOUBLELIST_FLAG_INACTIVE);
			}

			memoryRegionInsertAfter(region, newRegion, memory);
		}
	}

	return(memory);
}

void memDoubleListClearRegion(memoryDoubleList *doubleList, memoryRegion *region, uintptr_t flag){
	void *currentBlock;
	void *nextBlock;
	void *oldFree = doubleList->nextFreeBlock;

	//Set the first block's flag.
	MEMDOUBLELIST_BLOCK_FREE_GET_FLAG(region->start) = flag;
	currentBlock = MEMDOUBLELIST_BLOCK_FREE_FLAG_GET_NEXT(region->start);
	//Set the next free block to the first block in the new region.
	doubleList->nextFreeBlock = currentBlock;

	nextBlock = MEMDOUBLELIST_BLOCK_GET_NEXT_BLOCK(currentBlock, doubleList->blockSize);
	//Make every block in the linked list point to the next.
	while(nextBlock < (void *)region){
		//Set the block's next pointer to the next block in the array.
		MEMDOUBLELIST_BLOCK_FREE_GET_NEXT(currentBlock) = nextBlock;
		currentBlock = nextBlock;
		//Make the next block invalid.
		*MEMDOUBLELIST_BLOCK_FREE_NEXT_GET_FLAG(currentBlock) = flag;

		nextBlock = MEMDOUBLELIST_BLOCK_GET_NEXT_BLOCK(nextBlock, doubleList->blockSize);
	}

	//We set the last block's pointer to the old next free block.
	MEMDOUBLELIST_BLOCK_FREE_GET_NEXT(currentBlock) = oldFree;
}

void memDoubleListClear(memoryDoubleList *doubleList){
	memoryRegion *memoryEnd = doubleList->region;
	void *currentBlock = MEMDOUBLELIST_BLOCK_FREE_FLAG_GET_NEXT(memoryEnd->start);

	//The next free block should be the beginning of the first memory region.
	doubleList->nextFreeBlock = currentBlock;

	//Clear every block in every region.
	do {
		void *nextBlock;

		//This will make the last block of the previous region
		//point to the first block of the current one. This
		//value will be overwritten with the first region.
		MEMDOUBLELIST_BLOCK_FREE_GET_NEXT(currentBlock) = memoryEnd->start;
		MEMDOUBLELIST_BLOCK_FREE_GET_FLAG(memoryEnd->start) = MEMDOUBLELIST_FLAG_INVALID;
		currentBlock = MEMDOUBLELIST_BLOCK_FREE_FLAG_GET_NEXT(memoryEnd->start);

		nextBlock = MEMDOUBLELIST_BLOCK_GET_NEXT_BLOCK(currentBlock, doubleList->blockSize);
		//Make every block in the linked list point to the next.
		while(nextBlock < (void *)memoryEnd){
			//Set the block's next pointer to the next block in the array.
			MEMDOUBLELIST_BLOCK_FREE_GET_NEXT(currentBlock) = nextBlock;
			currentBlock = nextBlock;
			//Make the next block invalid.
			*MEMDOUBLELIST_BLOCK_FREE_NEXT_GET_FLAG(currentBlock) = MEMDOUBLELIST_FLAG_INVALID;

			nextBlock = MEMDOUBLELIST_BLOCK_GET_NEXT_BLOCK(nextBlock, doubleList->blockSize);
		}

		//Move to the next region.
		memoryEnd = memoryEnd->next;
	} while(memoryEnd != NULL);

	//We set the last block's pointer to NULL as nothing comes after it.
	MEMDOUBLELIST_BLOCK_FREE_GET_NEXT(currentBlock) = NULL;
}


void memDoubleListDelete(memoryDoubleList *doubleList){
	memoryAllocatorDelete(doubleList->region);
}