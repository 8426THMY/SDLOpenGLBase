#ifndef memoryManager_h
#define memorymanager_h


#include "utilMemory.h"
#include "settingsMemory.h"

#include "memoryTree.h"


#ifndef MEMORY_HEAPSIZE
	#define MEMORY_HEAPSIZE (64 * MEMORY_MEBIBYTE)
#endif


typedef memoryTree memoryManager;


// This will define a memory manager with global scope
// and functions that are "more optimised" for it.
/** Would it be better to define these as macros that  **/
/** just call the regular versions of these functions? **/
#ifdef MEMORY_USE_GLOBAL_MANAGER
return_t memoryManagerGlobalInit(const size_t heapSize);

void *memoryManagerGlobalAlloc(const size_t heapSize);
void *memoryManagerGlobalResize(void *const restrict block, const size_t blockSize);
void *memoryManagerGlobalRealloc(void *const restrict block, const size_t blockSize);
void *memoryManagerGlobalExtend(const size_t memorySize);
void memoryManagerGlobalFree(void *const restrict block);

void memoryManagerGlobalDelete();


extern memoryManager g_memManager;
#endif

// This will define memory manager functions
// that can be used with a user-defined manager.
#ifdef MEMORY_USE_MODULE_MANAGER
return_t memoryManagerInit(memoryManager *const restrict memMngr, const size_t heapSize);

void *memoryManagerAlloc(memoryManager *const restrict memMngr, const size_t heapSize);
void *memoryManagerResize(memoryManager *const restrict memMngr, void *const restrict block, const size_t blockSize);
void *memoryManagerRealloc(memoryManager *const restrict memMngr, void *const restrict block, const size_t blockSize);
void *memoryManagerExtend(memoryManager *const restrict memMngr, const size_t memorySize);
void memoryManagerFree(memoryManager *const restrict memMngr, void *const restrict block);

void memoryManagerDelete(memoryManager *const restrict memMngr);
#endif


#endif