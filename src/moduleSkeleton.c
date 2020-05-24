#include "moduleSkeleton.h"


memoryPool g_skeletonManager;
memoryPool g_skeleAnimDefManager;
memorySingleList g_skeleAnimManager;


#warning "How are we going to store animations? How about bones?"

#warning "What if we aren't using the global memory manager?"


return_t moduleSkeletonSetup(){
	// The module's setup will be successful if we
	// can allocate enough memory for our manager.
	return(
		// skeleton
		memPoolInit(
			&g_skeletonManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_SKELETON_MANAGER_SIZE)),
			MODULE_SKELETON_MANAGER_SIZE, MODULE_SKELETON_ELEMENT_SIZE
		) != NULL &&
		// skeletonAnimDef
		memPoolInit(
			&g_skeleAnimDefManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_SKELEANIMDEF_MANAGER_SIZE)),
			MODULE_SKELEANIMDEF_MANAGER_SIZE, MODULE_SKELEANIMDEF_ELEMENT_SIZE
		) != NULL &&
		// skeletonAnim
		memSingleListInit(
			&g_skeleAnimManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_SKELEANIM_MANAGER_SIZE)),
			MODULE_SKELEANIM_MANAGER_SIZE, MODULE_SKELEANIM_ELEMENT_SIZE
		) != NULL
	);
}

void moduleSkeletonCleanup(){
	// skeletonAnim
	MEMSINGLELIST_LOOP_BEGIN(g_skeleAnimManager, i, skeletonAnim)
		moduleSkeleAnimFree(NULL, i, NULL);
	MEMSINGLELIST_LOOP_END(g_skeleAnimManager, i, break)
	memSingleListDelete(&g_skeleAnimManager, memoryManagerGlobalFree);
	// skeletonAnimDef
	MEMPOOL_LOOP_BEGIN(g_skeleAnimDefManager, i, skeletonAnimDef)
		moduleSkeleAnimDefFree(i);
	MEMPOOL_LOOP_END(g_skeleAnimDefManager, i, break)
	memPoolDelete(&g_skeleAnimDefManager, memoryManagerGlobalFree);
	// skeleton
	MEMPOOL_LOOP_BEGIN(g_skeletonManager, i, skeleton)
		moduleSkeletonFree(i);
	MEMPOOL_LOOP_END(g_skeletonManager, i, break)
	memPoolDelete(&g_skeletonManager, memoryManagerGlobalFree);
}


// Allocate memory for a skeleton and return a handle to it.
skeleton *moduleSkeletonAlloc(){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memPoolAlloc(&g_skeletonManager));
	#else
	skeleton *newBlock = memPoolAlloc(&g_skeletonManager);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memPoolExtend(
			&g_skeletonManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_SKELETON_MANAGER_SIZE)),
			MODULE_SKELETON_MANAGER_SIZE
		)){
			newBlock = memPoolAlloc(&g_skeletonManager);
		}
	}
	return(newBlock);
	#endif
}

// Free a skeleton that has been allocated.
void moduleSkeletonFree(skeleton *skele){
	skeleDelete(skele);
	memPoolFree(&g_skeletonManager, skele);
}

// Delete every skeleton in the manager.
void moduleSkeletonClear(){
	MEMPOOL_LOOP_BEGIN(g_skeletonManager, i, skeleton)
		moduleSkeletonFree(i);
	MEMPOOL_LOOP_END(g_skeletonManager, i, break)
	memPoolClear(&g_skeletonManager);
}


// Allocate memory for a skeleton animation base and return a handle to it.
skeletonAnimDef *moduleSkeleAnimDefAlloc(){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memPoolAlloc(&g_skeleAnimDefManager));
	#else
	skeletonAnimDef *newBlock = memPoolAlloc(&g_skeleAnimDefManager);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memPoolExtend(
			&g_skeleAnimDefManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_SKELEANIMDEF_MANAGER_SIZE)),
			MODULE_SKELEANIMDEF_MANAGER_SIZE
		)){
			newBlock = memPoolAlloc(&g_skeleAnimDefManager);
		}
	}
	return(newBlock);
	#endif
}

// Free a skeleton animation base that has been allocated.
void moduleSkeleAnimDefFree(skeletonAnimDef *const restrict animDef){
	skeleAnimDefDelete(animDef);
	memPoolFree(&g_skeleAnimDefManager, animDef);
}

// Delete every skeleton animation base in the manager.
void moduleSkeleAnimDefClear(){
	MEMPOOL_LOOP_BEGIN(g_skeleAnimDefManager, i, skeletonAnimDef)
		moduleSkeleAnimDefFree(i);
	MEMPOOL_LOOP_END(g_skeleAnimDefManager, i, break)
	memPoolClear(&g_skeleAnimDefManager);
}


// Allocate a new skeleton animation state array.
skeletonAnim *moduleSkeleAnimAlloc(){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListAlloc(&g_skeleAnimManager));
	#else
	skeletonAnim *newBlock = memSingleListAlloc(&g_skeleAnimManager);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_skeleAnimManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_SKELEANIM_MANAGER_SIZE)),
			MODULE_SKELEANIM_MANAGER_SIZE
		)){
			newBlock = memSingleListAlloc(&g_skeleAnimManager);
		}
	}
	return(newBlock);
	#endif
}

// Insert a skeleton animation state at the beginning of an array.
skeletonAnim *moduleSkeleAnimPrepend(skeletonAnim **const restrict start){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListPrepend(&g_skeleAnimManager, (void **)start));
	#else
	skeletonAnim *newBlock = memSingleListPrepend(&g_skeleAnimManager, (void **)start);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_skeleAnimManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_SKELEANIM_MANAGER_SIZE)),
			MODULE_SKELEANIM_MANAGER_SIZE
		)){
			newBlock = memSingleListPrepend(&g_skeleAnimManager, (void **)start);
		}
	}
	return(newBlock);
	#endif
}

// Insert a skeleton animation state at the end of an array.
skeletonAnim *moduleSkeleAnimAppend(skeletonAnim **const restrict start){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListAppend(&g_skeleAnimManager, (void **)start));
	#else
	skeletonAnim *newBlock = memSingleListAppend(&g_skeleAnimManager, (void **)start);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_skeleAnimManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_SKELEANIM_MANAGER_SIZE)),
			MODULE_SKELEANIM_MANAGER_SIZE
		)){
			newBlock = memSingleListAppend(&g_skeleAnimManager, (void **)start);
		}
	}
	return(newBlock);
	#endif
}

// Insert a skeleton animation state after the element "prevData".
skeletonAnim *moduleSkeleAnimInsertBefore(skeletonAnim **const restrict start, skeletonAnim *const restrict prevData){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListInsertBefore(&g_skeleAnimManager, (void **)start, (void *)prevData));
	#else
	skeletonAnim *newBlock = memSingleListInsertBefore(&g_skeleAnimManager, (void **)start, (void *)prevData);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_skeleAnimManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_SKELEANIM_MANAGER_SIZE)),
			MODULE_SKELEANIM_MANAGER_SIZE
		)){
			newBlock = memSingleListInsertBefore(&g_skeleAnimManager, (void **)start, (void *)prevData);
		}
	}
	return(newBlock);
	#endif
}

// Insert a skeleton animation state after the element "data".
skeletonAnim *moduleSkeleAnimInsertAfter(skeletonAnim **const restrict start, skeletonAnim *const restrict data){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListInsertAfter(&g_skeleAnimManager, (void **)start, (void *)data));
	#else
	skeletonAnim *newBlock = memSingleListInsertAfter(&g_skeleAnimManager, (void **)start, (void *)data);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_skeleAnimManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_SKELEANIM_MANAGER_SIZE)),
			MODULE_SKELEANIM_MANAGER_SIZE
		)){
			newBlock = memSingleListInsertAfter(&g_skeleAnimManager, (void **)start, (void *)data);
		}
	}
	return(newBlock);
	#endif
}

// Free a skeleton animation state that has been allocated.
void moduleSkeleAnimFree(skeletonAnim **const restrict start, skeletonAnim *const restrict anim, skeletonAnim *const restrict prevData){
	memSingleListFree(&g_skeleAnimManager, (void **)start, (void *)anim, (void *)prevData);
}

// Free an entire skeleton animation state array.
void moduleSkeleAnimFreeArray(skeletonAnim **const restrict start){
	skeletonAnim *anim = *start;
	while(anim != NULL){
		moduleSkeleAnimFree(start, anim, NULL);
		anim = *start;
	}
}

// Delete every skeleton animation state in the manager.
void moduleSkeleAnimClear(){
	MEMSINGLELIST_LOOP_BEGIN(g_skeleAnimManager, i, skeletonAnim)
		moduleSkeleAnimFree(NULL, i, NULL);
	MEMSINGLELIST_LOOP_END(g_skeleAnimManager, i, break)
	memSingleListClear(&g_skeleAnimManager);
}