#include "moduleSkeleton.h"


memoryPool skeletonManager;
memoryPool skeleAnimDefManager;
memorySingleList skeleAnimManager;


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
		// skeletonAnimDef
		memPoolInit(
			&skeleAnimDefManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_SKELEANIMDEF_MANAGER_SIZE)),
			MODULE_SKELEANIMDEF_MANAGER_SIZE, MODULE_SKELEANIMDEF_ELEMENT_SIZE
		) != NULL &&
		// skeletonAnim
		memSingleListInit(
			&skeleAnimManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_SKELEANIM_MANAGER_SIZE)),
			MODULE_SKELEANIM_MANAGER_SIZE, MODULE_SKELEANIM_ELEMENT_SIZE
		) != NULL
	);
}

void moduleSkeletonCleanup(){
	// skeletonAnim
	moduleSkeleAnimClear();
	memoryManagerGlobalFree(memSingleListRegionStart(skeleAnimManager.region));
	// skeletonAnimDef
	moduleSkeleAnimDefClear();
	memoryManagerGlobalFree(memPoolRegionStart(skeleAnimDefManager.region));
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


// Allocate memory for a skeleton animation base and return a handle to it.
skeletonAnimDef *moduleSkeleAnimDefAlloc(){
	return(memPoolAlloc(&skeleAnimDefManager));
}

// Free a skeleton animation base that has been allocated.
void moduleSkeleAnimDefFree(skeletonAnimDef *animDef){
	skeleAnimDefDelete(animDef);
	memPoolFree(&skeleAnimDefManager, animDef);
}

// Delete every skeleton animation base in the manager.
void moduleSkeleAnimDefClear(){
	MEMPOOL_LOOP_BEGIN(skeleAnimDefManager, i, skeletonAnimDef *)
		moduleSkeleAnimDefFree(i);
	MEMPOOL_LOOP_END(skeleAnimDefManager, i, skeletonAnimDef *, return)
}


// Allocate a new skeleton animation state array.
skeletonAnim *moduleSkeleAnimAlloc(){
	return(memSingleListAlloc(&skeleAnimManager));
}

// Insert a skeleton animation state at the beginning of an array.
skeletonAnim *moduleSkeleAnimPrepend(skeletonAnim **start){
	return(memSingleListPrepend(&skeleAnimManager, (void **)start));
}

// Insert a skeleton animation state at the end of an array.
skeletonAnim *moduleSkeleAnimAppend(skeletonAnim **start){
	return(memSingleListAppend(&skeleAnimManager, (void **)start));
}

// Insert a skeleton animation state after the element "prevData".
skeletonAnim *moduleSkeleAnimInsertBefore(skeletonAnim **start, skeletonAnim *prevData){
	return(memSingleListInsertBefore(&skeleAnimManager, (void **)start, (void *)prevData));
}

// Insert a skeleton animation state after the element "data".
skeletonAnim *moduleSkeleAnimInsertAfter(skeletonAnim **start, skeletonAnim *data){
	return(memSingleListInsertAfter(&skeleAnimManager, (void **)start, (void *)data));
}

// Free a skeleton animation state that has been allocated.
void moduleSkeleAnimFree(skeletonAnim **start, skeletonAnim *anim, skeletonAnim *prevData){
	memSingleListFree(&skeleAnimManager, (void **)start, (void *)anim, (void *)prevData);
}

// Free an entire skeleton animation state array.
void moduleSkeleAnimFreeArray(skeletonAnim **start){
	skeletonAnim *anim = *start;
	while(anim != NULL){
		moduleSkeleAnimFree(start, anim, NULL);
		anim = *start;
	}
}

// Delete every skeleton animation state in the manager.
void moduleSkeleAnimClear(){
	MEMSINGLELIST_LOOP_BEGIN(skeleAnimManager, i, skeletonAnim *)
		moduleSkeleAnimFree(NULL, i, NULL);
	MEMSINGLELIST_LOOP_END(skeleAnimManager, i, skeletonAnim *, return)
}