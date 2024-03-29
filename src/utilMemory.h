#ifndef utilMemory_h
#define utilMemory_h


#include <stdlib.h>
#include <stdint.h>

#include "utilTypes.h"


// This lets us print memory addresses as unsigned integers without having to
// worry about changing format specifiers between 32-bit and 64-bit systems.
#if UINTPTR_MAX == UINT32_MAX
	#define PRINTF_SIZE_T "%u"
#else
	#include <inttypes.h>
	#define PRINTF_SIZE_T "%"PRIu64
#endif


// Add "num" to the address "pointer". We need to cast the pointer
// to a character pointer, as otherwise we will be adding "num"
// multiplied by the size of the type that "pointer" points to.
#define memoryAddPointer(pointer, num) ((void *)(((uintptr_t)(pointer)) + ((uintptr_t)(num))))
// Same as above, but subtracts "num" from "pointer" instead of adding it.
#define memorySubPointer(pointer, num) ((void *)(((uintptr_t)(pointer)) - ((uintptr_t)(num))))

// We'll assume the system is 8 byte aligned.
#define MEMORY_ALIGNMENT 8
#define MEMORY_FLAG_MASK (MEMORY_ALIGNMENT - 1)
#define MEMORY_DATA_MASK (~MEMORY_FLAG_MASK)
// Assuming "ALIGNMENT" is a power of two,
// this will round "block" up to "ALIGNMENT".
#define memoryAlign(block) ((void *)(((uintptr_t)memoryAddPointer(block, MEMORY_FLAG_MASK)) & MEMORY_DATA_MASK))

// Return the size of a memory region. All this really
// does is subtracts the size of the object from its size.
#define memoryGetRegionSize(size) ((size) - sizeof(memoryRegion))
// Return a pointer to a memory region object using the total size of the region.
// Note the assumption that the size of the region includes the size of the memoryRegion object.
#define memoryGetRegionFromSize(start, size) ((memoryRegion *)memoryAddPointer(start, memoryGetRegionSize(size)))
// Return a pointer to a memory region object using the number of blocks in the region.
#define memoryGetRegionFromBlocks(start, blocks, size) ((memoryRegion *)memoryAddPointer(start, (blocks) * (size)))
// Return the amount of memory required for a region of "size" bytes.
#define memoryGetRequiredSize(size) ((size) + sizeof(memoryRegion))

// Get the size of the data controlled by a memory region.
#define memoryRegionDataSize(region) ((uintptr_t)memorySubPointer((region)->start, (region)))
// Get the total size of a memory region.
#define memoryRegionFullSize(region) (((uintptr_t)memorySubPointer((region)->start, (region))) + sizeof(memoryRegion))

// This can be used to exit from memory allocator loops early.
#define memoryLoopExit(allocator, node) goto allocator##_EXIT_LOOP_##node

// The main reason for using these flags
// are for when we want to enable or
// disable concurrent heap allocations.
#ifdef _WIN32
	#define MEMORY_WIN32_HEAP_FLAG_SAFE   0x00
	#define MEMORY_WIN32_HEAP_FLAG_UNSAFE 0x01
	#define MEMORY_WIN32_HEAP_FLAGS       MEMORY_WIN32_HEAP_FLAG_UNSAFE
#endif

/*#ifndef MEMORY_LOW_LEVEL
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
#endif*/


// Stored at the end of certain allocators' memory regions,
// this information is used to extend an allocator's memory.
typedef struct memoryRegion memoryRegion;
typedef struct memoryRegion {
	void *start;
	memoryRegion *next;
} memoryRegion;


void *memoryAlloc(const size_t size);
#if defined(_WIN32) || !defined(MEMORY_LOW_LEVEL)
void *memoryRealloc(void *const restrict block, const size_t size);
void memoryFree(void *const restrict block);
#else
void *memoryRealloc(void *const restrict block, const size_t oldSize, const size_t newSize);
void memoryFree(void *const restrict block, const size_t size);
#endif

void memoryRegionAppend(memoryRegion **region, memoryRegion *const newRegion, void *const memory);
void memoryRegionInsertBefore(memoryRegion **region, memoryRegion *const newRegion, void *const memory);
void memoryRegionInsertAfter(memoryRegion *region, memoryRegion *const newRegion, void *const memory);

void memoryDeleteRegions(memoryRegion *region);


#endif