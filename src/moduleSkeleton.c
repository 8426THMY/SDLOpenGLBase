#include "moduleSkeleton.h"


memoryPool skeletonManager;
memoryPool skeleAnimManager;
memorySingleList skeleAnimStateManager;


#warning "How are we going to store animations? How about bones?"

#warning "What if we aren't using the global memory manager?"


return_t moduleSkeletonSetup(){
	// The module's setup will be successful if we
	// can allocate enough memory for our manager.
	return(
		// skeleton
		memPoolInit(
			&skeletonManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_SKELETON_MANAGER_SIZE)),
			MODULE_SKELETON_MANAGER_SIZE, MODULE_SKELETON_ELEMENT_SIZE
		) != NULL &&
		// skeletonAnim
		memPoolInit(
			&skeleAnimManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_SKELEANIM_MANAGER_SIZE)),
			MODULE_SKELEANIM_MANAGER_SIZE, MODULE_SKELEANIM_ELEMENT_SIZE
		) != NULL &&
		// skeletonAnimState
		memSingleListInit(
			&skeleAnimStateManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_SKELEANIMSTATE_MANAGER_SIZE)),
			MODULE_SKELEANIMSTATE_MANAGER_SIZE, MODULE_SKELEANIMSTATE_ELEMENT_SIZE
		) != NULL
	);
}

void moduleSkeletonCleanup(){
	// skeletonAnimState
	moduleSkeleAnimStateClear();
	memoryManagerGlobalFree(memSingleListRegionStart(skeleAnimStateManager.region));
	// skeletonAnim
	moduleSkeleAnimClear();
	memoryManagerGlobalFree(memPoolRegionStart(skeleAnimManager.region));
	// skeleton
	moduleSkeletonClear();
	memoryManagerGlobalFree(memPoolRegionStart(skeletonManager.region));
}


// Allocate memory for a skeleton and return a handle to it.
skeleton *moduleSkeletonAlloc(){
	return(memPoolAlloc(&skeletonManager));
}

// Free a skeleton that has been allocated.
void moduleSkeletonFree(skeleton *skele){
	skeleDelete(skele);
	memPoolFree(&skeletonManager, skele);
}

// Delete every skeleton in the manager.
void moduleSkeletonClear(){
	MEMPOOL_LOOP_BEGIN(skeletonManager, i, skeleton *)
		moduleSkeletonFree(i);
	MEMPOOL_LOOP_END(skeletonManager, i, skeleton *, return)
}


// Allocate memory for a skeleton animation and return a handle to it.
skeletonAnim *moduleSkeleAnimAlloc(){
	return(memPoolAlloc(&skeleAnimManager));
}

// Free a skeleton animation that has been allocated.
void moduleSkeletonAnimFree(skeletonAnim *skeleAnim){
	skeleAnimDelete(skeleAnim);
	memPoolFree(&skeleAnimManager, skeleAnim);
}

// Delete every skeleton animation in the manager.
void moduleSkeleAnimClear(){
	MEMPOOL_LOOP_BEGIN(skeleAnimManager, i, skeletonAnim *)
		moduleSkeletonAnimFree(i);
	MEMPOOL_LOOP_END(skeleAnimManager, i, skeletonAnim *, return)
}


// Allocate a new skeleton animation state array.
skeletonAnimState *moduleSkeleAnimStateAlloc(){
	return(memSingleListAlloc(&skeleAnimStateManager));
}

// Insert a skeleton animation state at the beginning of an array.
skeletonAnimState *moduleSkeleAnimStatePrepend(skeletonAnimState **start){
	return(memSingleListPrepend(&skeleAnimStateManager, (void **)start));
}

// Insert a skeleton animation state at the end of an array.
skeletonAnimState *moduleSkeleAnimStateAppend(skeletonAnimState **start){
	return(memSingleListAppend(&skeleAnimStateManager, (void **)start));
}

// Insert a skeleton animation state after the element "prevData".
skeletonAnimState *moduleSkeleAnimStateInsertBefore(skeletonAnimState **start, skeletonAnimState *prevData){
	return(memSingleListInsertBefore(&skeleAnimStateManager, (void **)start, (void *)prevData));
}

// Insert a skeleton animation state after the element "data".
skeletonAnimState *moduleSkeleAnimStateInsertAfter(skeletonAnimState **start, skeletonAnimState *data){
	return(memSingleListInsertAfter(&skeleAnimStateManager, (void **)start, (void *)data));
}

// Free a skeleton animation state that has been allocated.
void moduleSkeleAnimStateFree(skeletonAnimState **start, skeletonAnimState *animState, skeletonAnimState *prevData){
	memSingleListFree(&skeleAnimStateManager, (void **)start, (void *)animState, (void *)prevData);
}

// Free an entire skeleton animation state array.
void moduleSkeleAnimStateFreeArray(skeletonAnimState **start){
	skeletonAnimState *animState = *start;
	while(animState != NULL){
		moduleSkeleAnimStateFree(start, animState, NULL);
		animState = *start;
	}
}

// Delete every skeleton animation state in the manager.
void moduleSkeleAnimStateClear(){
	MEMSINGLELIST_LOOP_BEGIN(skeleAnimStateManager, i, skeletonAnimState *)
		moduleSkeleAnimStateFree(NULL, i, NULL);
	MEMSINGLELIST_LOOP_END(skeleAnimStateManager, i, skeletonAnimState *, return)
}