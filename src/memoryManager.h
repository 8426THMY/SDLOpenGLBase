#ifndef memoryManager_h
#define memorymanager_h


#include "memoryShared.h"
#include "settingsMemory.h"

#include "memoryTree.h"


#ifndef MEMORY_HEAPSIZE
	#define MEMORY_HEAPSIZE (64 * MEMORY_MEBIBYTE)
#endif


typedef memoryTree memoryManager;


//This will define a memory manager with global scope
//and functions that are "more optimised" for it.
/** Would it be better to define these as macros that  **/
/** just call the regular versions of these functions? **/
#ifdef MEMORY_USE_GLOBAL_MANAGER
return_t memoryManagerGlobalInit(const size_t heapSize);

void *memoryManagerGlobalAlloc(const size_t heapSize);
void *memoryManagerGlobalResize(void *block, const size_t blockSize);
void *memoryManagerGlobalRealloc(void *block, const size_t blockSize);
void *memoryManagerGlobalExtend(const size_t memorySize);
void memoryManagerGlobalFree(void *block);

void memoryManagerGlobalDelete();


extern memoryManager memManager;
#endif

//This will define memory manager functions
//that can be used with a user-defined manager.
#ifdef MEMORY_USE_MODULE_MANAGER
return_t memoryManagerInit(memoryManager *memMngr, const size_t heapSize);

void *memoryManagerAlloc(memoryManager *memMngr, const size_t heapSize);
void *memoryManagerResize(memoryManager *memMngr, void *block, const size_t blockSize);
void *memoryManagerRealloc(memoryManager *memMngr, void *block, const size_t blockSize);
void *memoryManagerExtend(memoryManager *memMngr, const size_t memorySize);
void memoryManagerFree(memoryManager *memMngr, void *block);

void memoryManagerDelete(memoryManager *memMngr);
#endif


#endif