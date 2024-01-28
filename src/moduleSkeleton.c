#include "moduleSkeleton.h"


#include "memoryManager.h"


// skeleton
moduleDefinePool(Skeleton, skeleton, g_skeletonManager, MODULE_SKELETON_MANAGER_SIZE)
moduleDefinePoolFreeFlexible(Skeleton, skeleton, g_skeletonManager, skeleDelete)
// skeletonAnimDef
moduleDefinePool(SkeletonAnimDef, skeletonAnimDef, g_skeleAnimDefManager, MODULE_SKELEANIMDEF_MANAGER_SIZE)
moduleDefinePoolFreeFlexible(SkeletonAnimDef, skeletonAnimDef, g_skeleAnimDefManager, skeleAnimDefDelete)
// skeletonAnim
moduleDefineSingleList(SkeletonAnim, skeletonAnim, g_skeleAnimManager, MODULE_SKELEANIM_MANAGER_SIZE)
moduleDefineSingleListFree(SkeletonAnim, skeletonAnim, g_skeleAnimManager)


return_t moduleSkeletonSetup(){
	// The module's setup will be successful if we
	// can allocate enough memory for our managers.
	return(moduleSkeletonInit() && moduleSkeletonAnimDefInit() && moduleSkeletonAnimInit());
}

void moduleSkeletonCleanup(){
	moduleSkeletonAnimDelete();
	moduleSkeletonAnimDefDelete();
	moduleSkeletonDelete();
}