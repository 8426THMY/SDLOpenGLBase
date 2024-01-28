#ifndef moduleSkeleton_h
#define moduleSkeleton_h


#include "skeleton.h"

#include "memoryPool.h"
#include "memorySingleList.h"

#include "utilTypes.h"
#include "moduleShared.h"


#define MODULE_SKELETON
#define MODULE_SKELETON_SETUP_FAIL 4

#ifndef MEMORY_MODULE_NUM_SKELETONS
	#define MEMORY_MODULE_NUM_SKELETONS 1
#endif
#ifndef MEMORY_MODULE_NUM_SKELEANIMDEFS
	#define MEMORY_MODULE_NUM_SKELEANIMDEFS 1
#endif
#ifndef MEMORY_MODULE_NUM_SKELEANIMS
	#define MEMORY_MODULE_NUM_SKELEANIMS 1
#endif

#define MODULE_SKELETON_MANAGER_SIZE \
	memPoolMemoryForBlocks(MEMORY_MODULE_NUM_SKELETONS, sizeof(skeleton))
#define MODULE_SKELEANIMDEF_MANAGER_SIZE \
	memPoolMemoryForBlocks(MEMORY_MODULE_NUM_SKELEANIMDEFS, sizeof(skeletonAnimDef))
#define MODULE_SKELEANIM_MANAGER_SIZE \
	memSingleListMemoryForBlocks(MEMORY_MODULE_NUM_SKELEANIMS, sizeof(skeletonAnim))


// skeleton
moduleDeclarePool(Skeleton, skeleton, g_skeletonManager)
moduleDeclarePoolFree(Skeleton, skeleton)
// skeletonAnimDef
moduleDeclarePool(SkeletonAnimDef, skeletonAnimDef, g_skeleAnimDefManager)
moduleDeclarePoolFree(SkeletonAnimDef, skeletonAnimDef)
// skeletonAnim
moduleDeclareSingleList(SkeletonAnim, skeletonAnim, g_skeleAnimManager)
moduleDeclareSingleListFree(SkeletonAnim, skeletonAnim)

return_t moduleSkeletonSetup();
void moduleSkeletonCleanup();


#endif