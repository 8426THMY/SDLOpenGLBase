#include "memoryStack.h"


#include "utilTypes.h"


#define MEMSTACK_BLOCK_FOOTER_SIZE sizeof(size_t)

// Get the footer of the next block in the stack.
#define memStackBlockGetNextBlockFooter(block, size) ((size_t *)memoryAddPointer(block, size))
// Get the footer of the last block in the stack.
#define memStackBlockGetPrevBlockFooter(top) ((size_t *)memorySubPointer(top, MEMSTACK_BLOCK_FOOTER_SIZE))


#warning "What if the beginning or end of a memory region is not aligned?"


void *memStackInit(memoryStack *const restrict stack, void *const restrict memory, const size_t stackSize){
	stack->top = memory;
	stack->size = stackSize;
	stack->bottom = memory;

	return(memory);
}


// Allocate a new block at the top of the stack!
void *memStackAlloc(memoryStack *const restrict stack, const size_t blockSize){
	void *const newBlock = stack->top;
	size_t actualBlockSize;

	// Check for 0 byte allocations.
	if(blockSize == 0){
		return(NULL);
	}

	actualBlockSize = blockSize + MEMSTACK_BLOCK_FOOTER_SIZE;
	if(stack->size >= actualBlockSize){
		// Store this block's size in its footer.
		*memStackBlockGetNextBlockFooter(newBlock, blockSize) = blockSize;

		// Advance the stack's top pointer and update its size.
		stack->top = memoryAddPointer(newBlock, actualBlockSize);
		stack->size -= actualBlockSize;


		return(newBlock);
	}


	return(NULL);
}

// Free the block at the top of the stack!
void memStackFreeLast(memoryStack *const restrict stack){
	// Get a pointer to the last block's footer and get its size.
	size_t *const lastBlockFooter = memStackBlockGetPrevBlockFooter(stack->top);
	const size_t lastBlockSize = *lastBlockFooter;

	// Move the top of the stack back and update its size.
	stack->top = lastBlockFooter - lastBlockSize;
	stack->size += lastBlockSize + MEMSTACK_BLOCK_FOOTER_SIZE;
}