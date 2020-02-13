#include "memoryManager.h"


#ifdef MEMORY_USE_GLOBAL_MANAGER
memoryManager g_memManager;


// Allocate memory for the global memory manager.
#warning "Check if memoryAlloc failed in here instead of the allocators."
return_t memoryManagerGlobalInit(const size_t heapSize){
	const size_t regionSize = memTreeMemoryForSize(heapSize);

	return(memTreeInit(&g_memManager, memoryAlloc(regionSize), regionSize) != NULL);
}


void *memoryManagerGlobalAlloc(const size_t blockSize){
	return(memTreeAlloc(&g_memManager, blockSize));
}

void *memoryManagerGlobalResize(void *const restrict block, const size_t blockSize){
	return(memTreeResize(&g_memManager, block, blockSize));
}

void *memoryManagerGlobalRealloc(void *const restrict block, const size_t blockSize){
	return(memTreeRealloc(&g_memManager, block, blockSize));
}

void *memoryManagerGlobalExtend(const size_t heapSize){
	const size_t regionSize = memTreeMemoryForSize(heapSize);

	return(memTreeExtend(&g_memManager, memoryAlloc(regionSize), regionSize));
}

void memoryManagerGlobalFree(void *const restrict block){
	memTreeFree(&g_memManager, block);
}


// Free memory used by the global memory manager.
void memoryManagerGlobalDelete(){
    memTreeDelete(&g_memManager);
}
#endif

#ifdef MEMORY_USE_MODULE_MANAGER
// Allocate memory for the memory manager.
return_t memoryManagerInit(memoryManager *const restrict memMngr, const size_t heapSize){
	const size_t regionSize = memTreeMemoryForSize(heapSize);

	return(memTreeInit(memMngr, memoryAlloc(regionSize), regionSize) != NULL);
}


void *memoryManagerAlloc(memoryManager *const restrict memMngr, const size_t blockSize){
	return(memTreeAlloc(memMngr, blockSize));
}

void *memoryManagerResize(memoryManager *const restrict memMngr, void *const restrict block, const size_t blockSize){
	return(memTreeResize(memMngr, block, blockSize));
}

void *memoryManagerRealloc(memoryManager *const restrict memMngr, void *const restrict block, const size_t blockSize){
	return(memTreeRealloc(memMngr, block, blockSize));
}

void *memoryManagerExtend(memoryManager *const restrict memMngr, const size_t heapSize){
	const size_t regionSize = memTreeMemoryForSize(heapSize);

	return(memTreeExtend(memMngr, memoryAlloc(regionSize), regionSize));
}

void memoryManagerFree(memoryManager *const restrict memMngr, void *const restrict block){
	memTreeFree(memMngr, block);
}


// Free memory used by the memory manager.
void memoryManagerDelete(memoryManager *const restrict memMngr){
    memTreeDelete(memMngr);
}
#endif