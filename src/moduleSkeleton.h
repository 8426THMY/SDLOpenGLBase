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
void moduleSkeletonFree(skeleton *const restrict skele);
void moduleSkeletonClear();

skeletonAnimDef *moduleSkeleAnimDefAlloc();
void moduleSkeleAnimDefFree(skeletonAnimDef *const restrict animDef);
void moduleSkeleAnimDefClear();

skeletonAnim *moduleSkeleAnimAlloc();
skeletonAnim *moduleSkeleAnimPrepend(skeletonAnim **const restrict start);
skeletonAnim *moduleSkeleAnimAppend(skeletonAnim **const restrict start);
skeletonAnim *moduleSkeleAnimInsertBefore(skeletonAnim **const restrict start, skeletonAnim *const restrict prevData);
skeletonAnim *moduleSkeleAnimInsertAfter(skeletonAnim **const restrict start, skeletonAnim *const restrict data);
void moduleSkeleAnimFree(skeletonAnim **const restrict start, skeletonAnim *const restrict animInst, skeletonAnim *const restrict prevData);
void moduleSkeleAnimFreeArray(skeletonAnim **const restrict start);
void moduleSkeleAnimClear();


extern memoryPool g_skeletonManager;
extern memoryPool g_skeleAnimDefManager;
extern memorySingleList g_skeleAnimManager;


#endif