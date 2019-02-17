#include "moduleSkeleton.h"


memoryPool skeleAnimManager;


#warning "How are we going to store animations? How about bones?"

#warning "What if we aren't using the global memory manager?"


return_t moduleSkeletonSetup(){
	//The module's setup will be successful if we
	//can allocate enough memory for our manager.
	return(
		memPoolInit(
			&skeleAnimManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_SKELEANIM_MANAGER_SIZE)),
			MODULE_SKELEANIM_MANAGER_SIZE, MODULE_SKELEANIM_ELEMENT_SIZE
		) != NULL
	);
}

void moduleSkeletonCleanup(){
	moduleSkeletonAnimClear();
	memoryManagerGlobalFree(memPoolRegionStart(skeleAnimManager.region));
}


//Allocate memory for a skeleton
//and return a handle to it.
skeletonAnim *moduleSkeletonAnimAlloc(){
	return(memPoolAlloc(&skeleAnimManager));
}

//Free a skeleton that has been allocated.
void moduleSkeletonAnimFree(skeletonAnim *skeleAnim){
	skeleAnimDelete(skeleAnim);
	memPoolFree(&skeleAnimManager, skeleAnim);
}

//Delete every skeleton animation in the manager.
void moduleSkeletonAnimClear(){
	MEMPOOL_LOOP_BEGIN(skeleAnimManager, i, skeletonAnim *)
		moduleSkeletonAnimFree(i);
	MEMPOOL_LOOP_END(skeleAnimManager, i, skeletonAnim *, return)
}