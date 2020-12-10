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

skeletonAnimDef *moduleSkeletonAnimDefAlloc();
void moduleSkeletonAnimDefFree(skeletonAnimDef *const restrict animDef);
void moduleSkeletonAnimDefClear();

skeletonAnim *moduleSkeletonAnimAlloc();
skeletonAnim *moduleSkeletonAnimPrepend(skeletonAnim **const restrict start);
skeletonAnim *moduleSkeletonAnimAppend(skeletonAnim **const restrict start);
skeletonAnim *moduleSkeletonAnimInsertBefore(skeletonAnim **const restrict start, skeletonAnim *const restrict prevData);
skeletonAnim *moduleSkeletonAnimInsertAfter(skeletonAnim **const restrict start, skeletonAnim *const restrict data);
skeletonAnim *moduleSkeletonAnimNext(const skeletonAnim *const restrict animInst);
void moduleSkeletonAnimFree(skeletonAnim **const restrict start, skeletonAnim *const restrict animInst, skeletonAnim *const restrict prevData);
void moduleSkeletonAnimFreeArray(skeletonAnim **const restrict start);
void moduleSkeletonAnimClear();


extern memoryPool g_skeletonManager;
extern memoryPool g_skeleAnimDefManager;
extern memorySingleList g_skeleAnimManager;


#endif