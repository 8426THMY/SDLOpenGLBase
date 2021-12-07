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


#ifdef _WIN32
void *memoryLowLevelAlloc(const size_t bytes){
	return(HeapAlloc(GetProcessHeap(), MEMORY_WIN32_HEAP_FLAGS, bytes));
}
#else
void *memoryLowLevelAlloc(const size_t bytes){
	// It's not a good idea to mix brk with malloc and friends.
	//return(sbrk(bytes));

    int fd = open("/dev/zero", O_RDWR);
    void *p = mmap(0, bytes, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    close(fd);

    return(p);
}
#endif

#ifdef _WIN32
void *memoryLowLevelRealloc(void *const restrict block, const size_t bytes){
	return(HeapReAlloc(GetProcessHeap(), MEMORY_WIN32_HEAP_FLAGS, block, bytes));
}
#else
void *memoryLowLevelRealloc(void *const restrict block, const size_t bytes_old, const size_t bytes_new){
	// It's not a good idea to mix brk with malloc and friends.
	//if(brk((byte_t *)block + bytes) == 0){
	//	return(block);
	//}
	//return(NULL);

    return(mremap(block, bytes_old, bytes_new, MREMAP_MAYMOVE | MREMAP_FIXED));
}
#endif
}

#ifdef _WIN32
int memoryLowLevelFree(void *const restrict block){
	return(HeapFree(GetProcessHeap(), MEMORY_WIN32_HEAP_FLAGS, block));
}
#else
int memoryLowLevelFree(void *const restrict block, const size_t bytes){
	// It's not a good idea to mix brk with malloc and friends.
	//return(brk(block));

    return(munmap(block, bytes));
}
#endif
#endif


void *memoryAlloc(const size_t size){
#ifndef MEMORY_LOW_LEVEL
	return(malloc(size));
#else
	return(memoryLowLevelAlloc(size));
#endif
}

#if defined(_WIN32) || !defined(MEMORY_LOW_LEVEL)
void *memoryRealloc(void *const restrict block, const size_t size){
	return(realloc(block, size));
}

void memoryFree(void *const restrict block){
	free(block);
}
#else
void *memoryRealloc(void *const restrict block, const size_t oldSize, const size_t newSize){
	return(memoryLowLevelRealloc(block, oldSize, newSize));
}

void memoryFree(void *const restrict block, const size_t size){
	memoryLowLevelFree(block, size);
}
#endif


void memoryRegionAppend(memoryRegion **region, memoryRegion *const newRegion, void *const memory){
	newRegion->start = memory;
	// Find the last memory region.
	while(*region != NULL){
		region = &((*region)->next);
	}
	// Make it point to the new one.
	*region = newRegion;
	newRegion->next = NULL;
}

void memoryRegionInsertBefore(memoryRegion **region, memoryRegion *const newRegion, void *const memory){
	newRegion->start = memory;
	newRegion->next = *region;
	*region = newRegion;
}

void memoryRegionInsertAfter(memoryRegion *region, memoryRegion *const newRegion, void *const memory){
	newRegion->start = memory;
	newRegion->next = region->next;
	region->next = newRegion;
}


/*
** Free a sequence of memory regions that were
** allocated using the regular memory functions.
*/
void memoryDeleteRegions(memoryRegion *region){
	// Free every memory region in the allocator.
	while(region != NULL){
		memoryRegion *const next = region->next;

		#if defined(_WIN32) || !defined(MEMORY_LOW_LEVEL)
		memoryFree(region->start);
		#else
		memoryFree(region->start, memoryRegionFullSize(region));
		#endif
		region = next;
	}
}