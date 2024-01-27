#ifndef moduleSkeleton_h
#define moduleSkeleton_h


#include "skeleton.h"

#include "utilTypes.h"
#include "moduleShared.h"


#define MODULE_SKELETON
#define MODULE_SKELETON_SETUP_FAIL 4

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

#define MODULE_SKELETON_MANAGER_SIZE \
	memPoolMemoryForBlocks(MEMORY_MODULE_NUM_SKELETONS, MODULE_SKELETON_ELEMENT_SIZE)
#define MODULE_SKELEANIMDEF_MANAGER_SIZE \
	memPoolMemoryForBlocks(MEMORY_MODULE_NUM_SKELEANIMDEFS, MODULE_SKELEANIMDEF_ELEMENT_SIZE)
#define MODULE_SKELEANIM_MANAGER_SIZE \
	memSingleListMemoryForBlocks(MEMORY_MODULE_NUM_SKELEANIMS, MODULE_SKELEANIM_ELEMENT_SIZE)


moduleDeclarePool(Skeleton, skeleton, g_skeletonManager)
moduleDeclarePool(SkeletonAnimDef, skeletonAnimDef, g_skeleAnimDefManager)
moduleDeclareSingleList(SkeletonAnim, skeletonAnim, g_skeleAnimManager)

return_t moduleSkeletonSetup();
void moduleSkeletonCleanup();


#endif