#ifndef moduleSkeleton_h
#define moduleSkeleton_h


#include "utilTypes.h"

#include "memoryManager.h"
#include "memoryPool.h"

#include "skeleton.h"


#define MODULE_SKELETON
#define MODULE_SKELETON_SETUP_FAIL 3

#define MODULE_SKELEANIM_ELEMENT_SIZE sizeof(skeletonAnim)

#ifndef MEMORY_MODULE_NUM_SKELEANIMS
	#define MEMORY_MODULE_NUM_SKELEANIMS 1
#endif

#define MODULE_SKELEANIM_MANAGER_SIZE memPoolMemoryForBlocks(MEMORY_MODULE_NUM_SKELEANIMS, MODULE_SKELEANIM_ELEMENT_SIZE)


return_t moduleSkeletonSetup();
void moduleSkeletonCleanup();

skeletonAnim *moduleSkeletonAnimAlloc();
void moduleSkeletonAnimFree(skeletonAnim *skeleAnim);
void moduleSkeletonAnimClear();


extern memoryPool skeleAnimManager;


#endif