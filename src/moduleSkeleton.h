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
#define MODULE_SKELEANIMDEF_ELEMENT_SIZE      sizeof(skeletonAnimDef)
#define MODULE_SKELEANIM_ELEMENT_SIZE sizeof(skeletonAnim)

#ifndef MEMORY_MODULE_NUM_SKELETONS
	#define MEMORY_MODULE_NUM_SKELETONS 1
#endif
#ifndef MEMORY_MODULE_NUM_SKELEANIMDEFS
	#define MEMORY_MODULE_NUM_SKELEANIMDEFS 1
#endif
#ifndef MEMORY_MODULE_NUM_SKELEANIMS
	#define MEMORY_MODULE_NUM_SKELEANIMS 1
#endif

#define MODULE_SKELETON_MANAGER_SIZE     memPoolMemoryForBlocks(MEMORY_MODULE_NUM_SKELETONS, MODULE_SKELETON_ELEMENT_SIZE)
#define MODULE_SKELEANIMDEF_MANAGER_SIZE memPoolMemoryForBlocks(MEMORY_MODULE_NUM_SKELEANIMDEFS, MODULE_SKELEANIMDEF_ELEMENT_SIZE)
#define MODULE_SKELEANIM_MANAGER_SIZE    memSingleListMemoryForBlocks(MEMORY_MODULE_NUM_SKELEANIMS, MODULE_SKELEANIM_ELEMENT_SIZE)


return_t moduleSkeletonSetup();
void moduleSkeletonCleanup();

skeleton *moduleSkeletonAlloc();
void moduleSkeletonFree(skeleton *skele);
void moduleSkeletonClear();

skeletonAnimDef *moduleSkeleAnimDefAlloc();
void moduleSkeleAnimDefFree(skeletonAnimDef *animDef);
void moduleSkeleAnimDefClear();

skeletonAnim *moduleSkeleAnimAlloc();
skeletonAnim *moduleSkeleAnimPrepend(skeletonAnim **start);
skeletonAnim *moduleSkeleAnimAppend(skeletonAnim **start);
skeletonAnim *moduleSkeleAnimInsertBefore(skeletonAnim **start, skeletonAnim *prevData);
skeletonAnim *moduleSkeleAnimInsertAfter(skeletonAnim **start, skeletonAnim *data);
void moduleSkeleAnimFree(skeletonAnim **start, skeletonAnim *animInst, skeletonAnim *prevData);
void moduleSkeleAnimFreeArray(skeletonAnim **start);
void moduleSkeleAnimClear();


extern memoryPool skeletonManager;
extern memoryPool skeleAnimDefManager;
extern memorySingleList skeleAnimManager;


#endif