#include "moduleRenderable.h"


memorySingleList g_renderableDefManager;
memorySingleList g_renderableManager;


return_t moduleRenderableSetup(){
	return(
		// renderableDef
		memSingleListInit(
			&g_renderableDefManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_RENDERABLEDEF_MANAGER_SIZE)),
			MODULE_RENDERABLEDEF_MANAGER_SIZE, MODULE_RENDERABLEDEF_ELEMENT_SIZE
		) != NULL &&
		// renderable
		memSingleListInit(
			&g_renderableManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_RENDERABLE_MANAGER_SIZE)),
			MODULE_RENDERABLE_MANAGER_SIZE, MODULE_RENDERABLE_ELEMENT_SIZE
		) != NULL
	);
}

void moduleRenderableCleanup(){
	// renderable
	MEMSINGLELIST_LOOP_BEGIN(g_renderableManager, i, renderable)
		moduleRenderableFree(NULL, i, NULL);
	MEMSINGLELIST_LOOP_END(g_renderableManager, i)
	memSingleListDelete(&g_renderableManager, memoryManagerGlobalFree);
	// renderableDef
	MEMSINGLELIST_LOOP_BEGIN(g_renderableDefManager, i, renderableDef)
		moduleRenderableDefFree(NULL, i, NULL);
	MEMSINGLELIST_LOOP_END(g_renderableDefManager, i)
	memSingleListDelete(&g_renderableDefManager, memoryManagerGlobalFree);
}


// Allocate a new renderable base array.
renderableDef *moduleRenderableDefAlloc(){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListAlloc(&g_renderableDefManager));
	#else
	renderableDef *newBlock = memSingleListAlloc(&g_renderableDefManager);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_renderableDefManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_RENDERABLEDEF_MANAGER_SIZE)),
			MODULE_RENDERABLEDEF_MANAGER_SIZE
		)){
			newBlock = memSingleListAlloc(&g_renderableDefManager);
		}
	}
	return(newBlock);
	#endif
}

// Insert an renderable base at the beginning of an array.
renderableDef *moduleRenderableDefPrepend(renderableDef **const restrict start){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListPrepend(&g_renderableDefManager, (void **)start));
	#else
	renderableDef *newBlock = memSingleListPrepend(&g_renderableDefManager, (void **)start);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_renderableDefManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_RENDERABLEDEF_MANAGER_SIZE)),
			MODULE_RENDERABLEDEF_MANAGER_SIZE
		)){
			newBlock = memSingleListPrepend(&g_renderableDefManager, (void **)start);
		}
	}
	return(newBlock);
	#endif
}

// Insert an renderable base at the end of an array.
renderableDef *moduleRenderableDefAppend(renderableDef **const restrict start){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListAppend(&g_renderableDefManager, (void **)start));
	#else
	renderableDef *newBlock = memSingleListAppend(&g_renderableDefManager, (void **)start);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_renderableDefManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_RENDERABLEDEF_MANAGER_SIZE)),
			MODULE_RENDERABLEDEF_MANAGER_SIZE
		)){
			newBlock = memSingleListAppend(&g_renderableDefManager, (void **)start);
		}
	}
	return(newBlock);
	#endif
}

// Insert an renderable base after the element "prevData".
renderableDef *moduleRenderableDefInsertBefore(renderableDef **const restrict start, renderableDef *const restrict prevData){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListInsertBefore(&g_renderableDefManager, (void **)start, (void *)prevData));
	#else
	renderableDef *newBlock = memSingleListInsertBefore(&g_renderableDefManager, (void **)start, (void *)prevData);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_renderableDefManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_RENDERABLEDEF_MANAGER_SIZE)),
			MODULE_RENDERABLEDEF_MANAGER_SIZE
		)){
			newBlock = memSingleListInsertBefore(&g_renderableDefManager, (void **)start, (void *)prevData);
		}
	}
	return(newBlock);
	#endif
}

// Insert an renderable base after the element "data".
renderableDef *moduleRenderableDefInsertAfter(renderableDef **const restrict start, renderableDef *const restrict data){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListInsertAfter(&g_renderableDefManager, (void **)start, (void *)data));
	#else
	renderableDef *newBlock = memSingleListInsertAfter(&g_renderableDefManager, (void **)start, (void *)data);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_renderableDefManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_RENDERABLEDEF_MANAGER_SIZE)),
			MODULE_RENDERABLEDEF_MANAGER_SIZE
		)){
			newBlock = memSingleListInsertAfter(&g_renderableDefManager, (void **)start, (void *)data);
		}
	}
	return(newBlock);
	#endif
}

renderableDef *moduleRenderableDefNext(const renderableDef *const restrict renderDef){
	return(memSingleListNext(renderDef));
}

// Free an renderable base that has been allocated.
void moduleRenderableDefFree(renderableDef **const restrict start, renderableDef *const restrict renderDef, renderableDef *const restrict prevData){
	memSingleListFree(&g_renderableDefManager, (void **)start, (void *)renderDef, (void *)prevData);
}

// Free an entire renderable base array.
void moduleRenderableDefFreeArray(renderableDef **const restrict start){
	renderableDef *renderDef = *start;
	while(renderDef != NULL){
		moduleRenderableDefFree(start, renderDef, NULL);
		renderDef = *start;
	}
}

// Delete every renderable base in the manager.
void moduleRenderableDefClear(){
	MEMSINGLELIST_LOOP_BEGIN(g_renderableDefManager, i, renderableDef)
		moduleRenderableDefFree(NULL, i, NULL);
	MEMSINGLELIST_LOOP_END(g_renderableDefManager, i)
	memSingleListClear(&g_renderableDefManager);
}


// Allocate a new renderable array.
renderable *moduleRenderableAlloc(){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListAlloc(&g_renderableManager));
	#else
	renderable *newBlock = memSingleListAlloc(&g_renderableManager);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_renderableManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_RENDERABLE_MANAGER_SIZE)),
			MODULE_RENDERABLE_MANAGER_SIZE
		)){
			newBlock = memSingleListAlloc(&g_renderableManager);
		}
	}
	return(newBlock);
	#endif
}

// Insert an renderable at the beginning of an array.
renderable *moduleRenderablePrepend(renderable **const restrict start){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListPrepend(&g_renderableManager, (void **)start));
	#else
	renderable *newBlock = memSingleListPrepend(&g_renderableManager, (void **)start);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_renderableManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_RENDERABLE_MANAGER_SIZE)),
			MODULE_RENDERABLE_MANAGER_SIZE
		)){
			newBlock = memSingleListPrepend(&g_renderableManager, (void **)start);
		}
	}
	return(newBlock);
	#endif
}

// Insert an renderable at the end of an array.
renderable *moduleRenderableAppend(renderable **const restrict start){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListAppend(&g_renderableManager, (void **)start));
	#else
	renderable *newBlock = memSingleListAppend(&g_renderableManager, (void **)start);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_renderableManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_RENDERABLE_MANAGER_SIZE)),
			MODULE_RENDERABLE_MANAGER_SIZE
		)){
			newBlock = memSingleListAppend(&g_renderableManager, (void **)start);
		}
	}
	return(newBlock);
	#endif
}

// Insert an renderable after the element "prevData".
renderable *moduleRenderableInsertBefore(renderable **const restrict start, renderable *const restrict prevData){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListInsertBefore(&g_renderableManager, (void **)start, (void *)prevData));
	#else
	renderable *newBlock = memSingleListInsertBefore(&g_renderableManager, (void **)start, (void *)prevData);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_renderableManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_RENDERABLE_MANAGER_SIZE)),
			MODULE_RENDERABLE_MANAGER_SIZE
		)){
			newBlock = memSingleListInsertBefore(&g_renderableManager, (void **)start, (void *)prevData);
		}
	}
	return(newBlock);
	#endif
}

// Insert an renderable after the element "data".
renderable *moduleRenderableInsertAfter(renderable **const restrict start, renderable *const restrict data){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListInsertAfter(&g_renderableManager, (void **)start, (void *)data));
	#else
	renderable *newBlock = memSingleListInsertAfter(&g_renderableManager, (void **)start, (void *)data);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_renderableManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_RENDERABLE_MANAGER_SIZE)),
			MODULE_RENDERABLE_MANAGER_SIZE
		)){
			newBlock = memSingleListInsertAfter(&g_renderableManager, (void **)start, (void *)data);
		}
	}
	return(newBlock);
	#endif
}

renderable *moduleRenderableNext(const renderable *const restrict render){
	return(memSingleListNext(render));
}

// Free an renderable that has been allocated.
void moduleRenderableFree(renderable **const restrict start, renderable *const restrict render, renderable *const restrict prevData){
	memSingleListFree(&g_renderableManager, (void **)start, (void *)render, (void *)prevData);
}

// Free an entire renderable array.
void moduleRenderableFreeArray(renderable **const restrict start){
	renderable *render = *start;
	while(render != NULL){
		moduleRenderableFree(start, render, NULL);
		render = *start;
	}
}

// Delete every renderable in the manager.
void moduleRenderableClear(){
	MEMSINGLELIST_LOOP_BEGIN(g_renderableManager, i, renderable)
		moduleRenderableFree(NULL, i, NULL);
	MEMSINGLELIST_LOOP_END(g_renderableManager, i)
	memSingleListClear(&g_renderableManager);
}