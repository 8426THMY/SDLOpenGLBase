#include "memoryStack.h"


#include "utilTypes.h"


#define MEMSTACK_BLOCK_FOOTER_SIZE sizeof(size_t)

//Get the footer of the next block in the stack.
#define MEMSTACK_BLOCK_GET_NEXT_MEMSTACK_BLOCK_FOOTER(block, size) ((size_t *)(((byte_t *)block) + size))
//Get the footer of the last block in the stack.
#define MEMSTACK_BLOCK_GET_PREV_MEMSTACK_BLOCK_FOOTER(top) ((size_t *)(((byte_t *)top) - MEMSTACK_BLOCK_FOOTER_SIZE))


void *memStackInit(memoryStack *stack, void *memory, const size_t stackSize){
	stack->top = memory;
	stack->size = stackSize;
	stack->bottom = memory;

	return(memory);
}


//Allocate a new block at the top of the stack!
void *memStackAlloc(memoryStack *stack, const size_t blockSize){
	void *newBlock = stack->top;

	const size_t actualBlockSize = blockSize + MEMSTACK_BLOCK_FOOTER_SIZE;
	if(stack->size >= actualBlockSize){
		//Store this block's size in its footer.
		*MEMSTACK_BLOCK_GET_NEXT_MEMSTACK_BLOCK_FOOTER(newBlock, blockSize) = blockSize;

		//Advance the stack's top pointer and update its size.
		stack->top = newBlock + actualBlockSize;
		stack->size -= actualBlockSize;


		return(newBlock);
	}


	return(NULL);
}

//Free the block at the top of the stack!
void memStackFreeLast(memoryStack *stack){
	//Get a pointer to the last block's footer and get its size.
	size_t *lastBlockFooter = MEMSTACK_BLOCK_GET_PREV_MEMSTACK_BLOCK_FOOTER(stack->top);
	const size_t lastBlockSize = *lastBlockFooter;

	//Move the top of the stack back and update its size.
	stack->top = lastBlockFooter - lastBlockSize;
	stack->size += lastBlockSize + MEMSTACK_BLOCK_FOOTER_SIZE;
}


//Free the entire stack's memory.
void memStackDelete(memoryStack *stack){
	memoryFree(stack->bottom);
}