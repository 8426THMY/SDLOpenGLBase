#ifndef settingsMemory_h
#define settingsMemory_h


// Included for "UINTPTR_MAX", "UINT32_MAX" and "UINT64_MAX".
#include <stdint.h>


// Define a macro for printing memory addresses as unsigned integers.
// This lets us avoid warnings about printf format specifiers.
#if UINTPTR_MAX == UINT32_MAX
	#define PRINTF_SIZE_T "%u"
#else
	#define PRINTF_SIZE_T "%lu"
#endif


#define MEMORY_KIBIBYTE 1024
#define MEMORY_MEBIBYTE (1024 * MEMORY_KIBIBYTE)

// Define the number of bytes to allocate for the memory manager.
#define MEMORY_HEAPSIZE (64 * MEMORY_MEBIBYTE)

#define MEMORY_USE_GLOBAL_MANAGER
#define MEMORY_USE_MODULE_MANAGER


// Define the base pool sizes for our object allocators!
#define MEMORY_MODULE_NUM_TEXTURES            4
#define MEMORY_MODULE_NUM_TEXGROUPS           3
#define MEMORY_MODULE_NUM_SKELETONS           2
#define MEMORY_MODULE_NUM_SKELEANIMDEFS       2
#define MEMORY_MODULE_NUM_SKELEANIMS          2
#define MEMORY_MODULE_NUM_MODELS              2
#define MEMORY_MODULE_NUM_PHYSCONTACTPAIRS    1
#define MEMORY_MODULE_NUM_PHYSSEPARATIONPAIRS 1
#define MEMORY_MODULE_NUM_AABBNODES           1
#define MEMORY_MODULE_NUM_PHYSCOLLIDERS       1
#define MEMORY_MODULE_NUM_PHYSRIGIDBODYDEFS   1
#define MEMORY_MODULE_NUM_PHYSRIGIDBODIES     1
#define MEMORY_MODULE_NUM_OBJECTDEFS          1
#define MEMORY_MODULE_NUM_OBJECTS             1
#define MEMORY_MODULE_NUM_RENDERABLEDEFS      1
#define MEMORY_MODULE_NUM_RENDERABLES         1


#define MEMTREE_DEBUG


#endif