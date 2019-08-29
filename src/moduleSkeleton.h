#ifndef moduleSkeleton_h
#define moduleSkeleton_h


#include "utilTypes.h"

#include "memoryManager.h"
#include "memoryPool.h"
#include "memorySingleList.h"

#include "skeleton.h"


#define MODULE_SKELETON
#define MODULE_SKELETON_SETUP_FAIL 3

#define MODULE_SKELETON_ELEMENT_SIZE       sizeof(skeleton)
#define MODULE_SKELEANIM_ELEMENT_SIZE      sizeof(skeletonAnim)
#define MODULE_SKELEANIMSTATE_ELEMENT_SIZE sizeof(skeletonAnimState)

#ifndef MEMORY_MODULE_NUM_SKELETONS
	#define MEMORY_MODULE_NUM_SKELETONS 1
#endif
#ifndef MEMORY_MODULE_NUM_SKELEANIMS
	#define MEMORY_MODULE_NUM_SKELEANIMS 1
#endif
#ifndef MEMORY_MODULE_NUM_SKELEANIMSTATES
	#define MEMORY_MODULE_NUM_SKELEANIMSTATES 1
#endif

#define MODULE_SKELETON_MANAGER_SIZE       memPoolMemoryForBlocks(MEMORY_MODULE_NUM_SKELETONS, MODULE_SKELETON_ELEMENT_SIZE)
#define MODULE_SKELEANIM_MANAGER_SIZE      memPoolMemoryForBlocks(MEMORY_MODULE_NUM_SKELEANIMS, MODULE_SKELEANIM_ELEMENT_SIZE)
#define MODULE_SKELEANIMSTATE_MANAGER_SIZE memSingleListMemoryForBlocks(MEMORY_MODULE_NUM_SKELEANIMSTATES, MODULE_SKELEANIMSTATE_ELEMENT_SIZE)


return_t moduleSkeletonSetup();
void moduleSkeletonCleanup();

skeleton *moduleSkeletonAlloc();
void moduleSkeletonFree(skeleton *skele);
void moduleSkeletonClear();

skeletonAnim *moduleSkeleAnimAlloc();
void moduleSkeleAnimFree(skeletonAnim *skeleAnim);
void moduleSkeleAnimClear();

skeletonAnimState *moduleSkeleAnimStateAlloc();
skeletonAnimState *moduleSkeleAnimStatePrepend(skeletonAnimState **start);
skeletonAnimState *moduleSkeleAnimStateAppend(skeletonAnimState **start);
skeletonAnimState *moduleSkeleAnimStateInsertBefore(skeletonAnimState **start, skeletonAnimState *prevData);
skeletonAnimState *moduleSkeleAnimStateInsertAfter(skeletonAnimState **start, skeletonAnimState *data);
void moduleSkeleAnimStateFree(skeletonAnimState **start, skeletonAnimState *animState, skeletonAnimState *prevData);
void moduleSkeleAnimStateFreeArray(skeletonAnimState **start);
void moduleSkeleAnimStateClear();


extern memoryPool skeletonManager;
extern memoryPool skeleAnimManager;
extern memorySingleList skeleAnimStateManager;


#endif