#ifndef settingsMemory_h
#define settingsMemory_h


#define MEMORY_KIBIBYTE 1024
#define MEMORY_MEBIBYTE (1024 * MEMORY_KIBIBYTE)

// Define the number of bytes to allocate for the memory manager.
#define MEMORY_HEAPSIZE (64 * MEMORY_MEBIBYTE)

#define MEMORY_USE_GLOBAL_MANAGER
#define MEMORY_USE_MODULE_MANAGER

#define MEMPOOL_COUNT_USED_BLOCKS
#define MEMFREELIST_COUNT_USED_BLOCKS
#define MEMSINGLELIST_COUNT_USED_BLOCKS
#define MEMDOUBLELIST_COUNT_USED_BLOCKS
#define MEMQUADLIST_COUNT_USED_BLOCKS


#warning "Modules should allocate new memory regions once they're full."
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
#define MEMORY_MODULE_NUM_OBJECTDEFS          2
#define MEMORY_MODULE_NUM_OBJECTS             2
#define MEMORY_MODULE_NUM_RENDERABLEDEFS      2
#define MEMORY_MODULE_NUM_RENDERABLES         2


#define MEMTREE_DEBUG


#endif