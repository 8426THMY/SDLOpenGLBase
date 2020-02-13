#include "utilMemory.h"


// These low level functions will almost
// certainly break things. The only time
// we can expect them to work is when we
// are the only ones operating on memory.
#ifdef MEMORY_LOW_LEVEL
#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#define VC_EXTRALEAN
	#include <windows.h>
#else
	#include <unistd.h>
#endif


void *memoryLowLevelAlloc(const size_t memorySize){
#ifdef _WIN32
	return(HeapAlloc(GetProcessHeap(), MEMORY_WIN32_HEAP_FLAGS, memorySize));
#else
	return(sbrk(memorySize));
#endif
}

void *memoryLowLevelRealloc(void *block, const size_t memorySize){
#ifdef _WIN32
	return(HeapReAlloc(GetProcessHeap(), MEMORY_WIN32_HEAP_FLAGS, block, memorySize));
#else
	if(brk((byte_t *)block + memorySize) == 0){
		return(block);
	}
	return(NULL);
#endif
}

return_t memoryLowLevelFree(void *block){
#ifdef _WIN32
	return(HeapFree(GetProcessHeap(), MEMORY_WIN32_HEAP_FLAGS, block));
#else
	return(brk(block));
#endif
}
#endif


void memoryRegionAppend(memoryRegion **region, memoryRegion *newRegion, void *memory){
	newRegion->start = memory;
	// Find the last memory region.
	while(*region != NULL){
		region = &((*region)->next);
	}
	// Make it point to the new one.
	*region = newRegion;
	newRegion->next = NULL;
}

void memoryRegionInsertBefore(memoryRegion **region, memoryRegion *newRegion, void *memory){
	newRegion->start = memory;
	newRegion->next = *region;
	*region = newRegion;
}

void memoryRegionInsertAfter(memoryRegion *region, memoryRegion *newRegion, void *memory){
	newRegion->start = memory;
	newRegion->next = region->next;
	region->next = newRegion;
}


/*
** This assumes that the region's start points
** to the beginning of the allocated block,
** which is not the case for many allocators.
*/
void memoryAllocatorDelete(memoryRegion *region){
	// Free every memory region in the allocator.
	while(region != NULL){
		memoryRegion *next = region->next;

		memoryFree(region->start);
		region = next;
	}
}