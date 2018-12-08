#ifndef memoryShared_h
#define memoryShared_h


#include <stdlib.h>
#include <stdint.h>

#include "utilTypes.h"


//We'll assume the system is 8 byte aligned.
#define MEMORY_ALIGNMENT 8
#define MEMORY_FLAG_MASK (MEMORY_ALIGNMENT - 1)
#define MEMORY_DATA_MASK (~MEMORY_FLAG_MASK)
//Assuming "ALIGNMENT" is a power of two,
//this will round "block" up to "ALIGNMENT".
#define memoryAlign(block) ((void *)((((uintptr_t)(block)) + MEMORY_FLAG_MASK) & MEMORY_DATA_MASK))

//Return the size of a memory region. All this really
//does is subtracts the size of the object from its size.
#define memoryGetRegionSize(size) ((size) - sizeof(memoryRegion))
//Return a pointer to a memory region object using the total size of the region.
#define memoryGetRegionFromSize(start, size)           ((memoryRegion *)(((byte_t *)(start)) + (size)))
//Return a pointer to a memory region object using the number of blocks in the region.
#define memoryGetRegionFromBlocks(start, blocks, size) ((memoryRegion *)(((byte_t *)(start)) + (blocks) * (size)))
//Return the amount of memory required for a region of "size" bytes.
#define memoryGetRequiredSize(size) ((size) + sizeof(memoryRegion))

//The main reason for using these flags
//are for when we want to enable or
//disable concurrent heap allocations.
#define MEMORY_WIN32_HEAP_FLAG_SAFE   0x00
#define MEMORY_WIN32_HEAP_FLAG_UNSAFE 0x01
#define MEMORY_WIN32_HEAP_FLAGS MEMORY_WIN32_HEAP_FLAG_UNSAFE

#ifndef MEMORY_LOW_LEVEL
#define memoryAlloc(size)          malloc(size)
#define memoryRealloc(block, size) realloc(block, size)
#define memoryFree(block)          free(block)
#else
void *memoryLowLevelAlloc(const size_t memorySize);
void *memoryLowLevelRealloc(void *block, const size_t memorySize);
return_t memoryLowLevelFree(void *block);

#define memoryAlloc(size)          memoryLowLevelAlloc(size)
#define memoryRealloc(block, size) memoryLowLevelRealloc(block, size)
#define memoryFree(block)          memoryLowLevelFree(block)
#endif


//Stored at the end of certain allocators' memory regions,
//this information is used to extend an allocator's memory.
typedef struct memoryRegion memoryRegion;
typedef struct memoryRegion {
	void *start;
	memoryRegion *next;
} memoryRegion;


void memoryRegionAppend(memoryRegion **region, memoryRegion *newRegion, void *memory);
void memoryRegionInsertBefore(memoryRegion **region, memoryRegion *newRegion, void *memory);
void memoryRegionInsertAfter(memoryRegion *region, memoryRegion *newRegion, void *memory);

void memoryAllocatorDelete(memoryRegion *region);


#endif