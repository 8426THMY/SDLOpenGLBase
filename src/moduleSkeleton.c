#include "moduleSkeleton.h"


#include "memoryManager.h"


moduleDefinePool(
	Skeleton, skeleton, g_skeletonManager,
	skeleDelete, MODULE_SKELETON_MANAGER_SIZE
)
moduleDefinePool(
	SkeletonAnimDef, skeletonAnimDef, g_skeleAnimDefManager,
	skeleAnimDefDelete, MODULE_SKELEANIMDEF_MANAGER_SIZE
)
moduleDefineSingleList(
	SkeletonAnim, skeletonAnim, g_skeleAnimManager,
	{}, MODULE_SKELEANIM_MANAGER_SIZE
)


return_t moduleSkeletonSetup(){
	// The module's setup will be successful if we
	// can allocate enough memory for our managers.
	return(
		// skeleton
		memPoolInit(
			&g_skeletonManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_SKELETON_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_SKELETON_MANAGER_SIZE), MODULE_SKELETON_ELEMENT_SIZE
		) != NULL &&
		// skeletonAnimDef
		memPoolInit(
			&g_skeleAnimDefManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_SKELEANIMDEF_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_SKELEANIMDEF_MANAGER_SIZE), MODULE_SKELEANIMDEF_ELEMENT_SIZE
		) != NULL &&
		// skeletonAnim
		memSingleListInit(
			&g_skeleAnimManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_SKELEANIM_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_SKELEANIM_MANAGER_SIZE), MODULE_SKELEANIM_ELEMENT_SIZE
		) != NULL
	);
}

void moduleSkeletonCleanup(){
	// skeletonAnim
	MEMSINGLELIST_LOOP_BEGIN(g_skeleAnimManager, i, skeletonAnim)
		moduleSkeletonAnimFree(NULL, i, NULL);
	MEMSINGLELIST_LOOP_END(g_skeleAnimManager, i)
	memoryManagerGlobalDeleteRegions(g_skeleAnimManager.region);
	// skeletonAnimDef
	MEMPOOL_LOOP_BEGIN(g_skeleAnimDefManager, i, skeletonAnimDef)
		moduleSkeletonAnimDefFree(i);
	MEMPOOL_LOOP_END(g_skeleAnimDefManager, i)
	memoryManagerGlobalDeleteRegions(g_skeleAnimDefManager.region);
	// skeleton
	MEMPOOL_LOOP_BEGIN(g_skeletonManager, i, skeleton)
		moduleSkeletonFree(i);
	MEMPOOL_LOOP_END(g_skeletonManager, i)
	memoryManagerGlobalDeleteRegions(g_skeletonManager.region);
}