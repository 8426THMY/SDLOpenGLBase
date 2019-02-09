#ifndef settingsMemory_h
#define settingsMemory_h


#define MEMORY_KIBIBYTE 1024
#define MEMORY_MEBIBYTE (1024 * MEMORY_KIBIBYTE)

//Define the number of bytes to allocate for the memory manager.
#define MEMORY_HEAPSIZE (64 * MEMORY_MEBIBYTE)

#define MEMORY_USE_GLOBAL_MANAGER
#define MEMORY_USE_MODULE_MANAGER


//Define the base pool sizes for our object allocators!
#define MEMORY_MAX_TEXTURES   1
#define MEMORY_MAX_TEXGROUPS  1
#define MEMORY_MAX_SKELEANIMS 1
#define MEMORY_MAX_MODELS     1
#define MEMORY_MAX_RENDEROBJS 1


#define MEMTREE_DEBUG


#endif