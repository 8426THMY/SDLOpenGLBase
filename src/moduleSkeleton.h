#ifndef moduleSkeleton_h
#define moduleSkeleton_h


#define MODULESKELETON

#define MODULESKELETON_SETUP_FAIL 3


#include "utilTypes.h"

#include "memoryManager.h"
#include "memoryPool.h"

#include "skeleton.h"


return_t moduleSkeletonSetup();

skeletonAnim *moduleSkeletonAnimAlloc();
void moduleSkeletonAnimFree(skeletonAnim *skeleAnim);

void moduleSkeletonCleanup();


extern memoryPool *skeletonManager;


#endif