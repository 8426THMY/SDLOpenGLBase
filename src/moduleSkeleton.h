#ifndef moduleSkeleton_h
#define moduleSkeleton_h


#include "utilTypes.h"

#include "memoryManager.h"
#include "memoryPool.h"

#include "skeleton.h"


#define MODULESKELETON
#define MODULESKELETON_SETUP_FAIL 3

#ifndef MEMORY_MAX_SKELEANIMS
	#define MEMORY_MAX_SKELEANIMS 1
#endif


return_t moduleSkeletonSetup();

skeletonAnim *moduleSkeletonAnimAlloc();
void moduleSkeletonAnimFree(skeletonAnim *skeleAnim);

void moduleSkeletonCleanup();


extern memoryPool skeletonManager;


#endif