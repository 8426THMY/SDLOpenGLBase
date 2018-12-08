#include "memoryManager.h"


#ifdef MEMORY_USE_GLOBAL_MANAGER
memoryManager memManager;


//Allocate memory for the global memory manager.
#warning "Check if memoryAlloc failed in here instead of the allocators."
return_t memoryManagerGlobalInit(const size_t heapSize){
	const size_t regionSize = memTreeMemoryForSize(heapSize);

	return(memTreeInit(&memManager, memoryAlloc(regionSize), regionSize) != NULL);
}


void *memoryManagerGlobalAlloc(const size_t blockSize){
	return(memTreeAlloc(&memManager, blockSize));
}

void *memoryManagerGlobalRealloc(void *block, const size_t blockSize){
	return(memTreeRealloc(&memManager, block, blockSize));
}

void *memoryManagerGlobalExtend(const size_t heapSize){
	const size_t regionSize = memTreeMemoryForSize(heapSize);

	return(memTreeExtend(&memManager, memoryAlloc(regionSize), regionSize));
}

void memoryManagerGlobalFree(void *block){
	memTreeFree(&memManager, block);
}


//Free memory used by the global memory manager.
void memoryManagerGlobalDelete(){
    memTreeDelete(&memManager);
}
#endif

#ifdef MEMORY_USE_MODULE_MANAGER
//Allocate memory for the memory manager.
return_t memoryManagerInit(memoryManager *memMngr, const size_t heapSize){
	const size_t regionSize = memTreeMemoryForSize(heapSize);

	return(memTreeInit(memMngr, memoryAlloc(regionSize), regionSize) != NULL);
}


void *memoryManagerAlloc(memoryManager *memMngr, const size_t blockSize){
	return(memTreeAlloc(memMngr, blockSize));
}

void *memoryManagerRealloc(memoryManager *memMngr, void *block, const size_t blockSize){
	return(memTreeRealloc(memMngr, block, blockSize));
}

void *memoryManagerExtend(memoryManager *memMngr, const size_t heapSize){
	const size_t regionSize = memTreeMemoryForSize(heapSize);

	return(memTreeExtend(memMngr, memoryAlloc(regionSize), regionSize));
}

void memoryManagerFree(memoryManager *memMngr, void *block){
	memTreeFree(memMngr, block);
}


//Free memory used by the memory manager.
void memoryManagerDelete(memoryManager *memMngr){
    memTreeDelete(memMngr);
}
#endif