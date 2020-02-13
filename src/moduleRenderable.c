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
	moduleRenderableClear();
	memoryManagerGlobalFree(memSingleListRegionStart(g_renderableManager.region));
	// renderableDef
	moduleRenderableDefClear();
	memoryManagerGlobalFree(memSingleListRegionStart(g_renderableDefManager.region));
}


// Allocate a new renderable base array.
renderableDef *moduleRenderableDefAlloc(){
	return(memSingleListAlloc(&g_renderableDefManager));
}

// Insert an renderable base at the beginning of an array.
renderableDef *moduleRenderableDefPrepend(renderableDef **const restrict start){
	return(memSingleListPrepend(&g_renderableDefManager, (void **)start));
}

// Insert an renderable base at the end of an array.
renderableDef *moduleRenderableDefAppend(renderableDef **const restrict start){
	return(memSingleListAppend(&g_renderableDefManager, (void **)start));
}

// Insert an renderable base after the element "prevData".
renderableDef *moduleRenderableDefInsertBefore(renderableDef **const restrict start, renderableDef *const restrict prevData){
	return(memSingleListInsertBefore(&g_renderableDefManager, (void **)start, (void *)prevData));
}

// Insert an renderable base after the element "data".
renderableDef *moduleRenderableDefInsertAfter(renderableDef **const restrict start, renderableDef *const restrict data){
	return(memSingleListInsertAfter(&g_renderableDefManager, (void **)start, (void *)data));
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
	MEMSINGLELIST_LOOP_BEGIN(g_renderableDefManager, i, renderableDef *)
		moduleRenderableDefFree(NULL, i, NULL);
	MEMSINGLELIST_LOOP_END(g_renderableDefManager, i, renderableDef *, return)
}


// Allocate a new renderable array.
renderable *moduleRenderableAlloc(){
	return(memSingleListAlloc(&g_renderableManager));
}

// Insert an renderable at the beginning of an array.
renderable *moduleRenderablePrepend(renderable **const restrict start){
	return(memSingleListPrepend(&g_renderableManager, (void **)start));
}

// Insert an renderable at the end of an array.
renderable *moduleRenderableAppend(renderable **const restrict start){
	return(memSingleListAppend(&g_renderableManager, (void **)start));
}

// Insert an renderable after the element "prevData".
renderable *moduleRenderableInsertBefore(renderable **const restrict start, renderable *const restrict prevData){
	return(memSingleListInsertBefore(&g_renderableManager, (void **)start, (void *)prevData));
}

// Insert an renderable after the element "data".
renderable *moduleRenderableInsertAfter(renderable **const restrict start, renderable *const restrict data){
	return(memSingleListInsertAfter(&g_renderableManager, (void **)start, (void *)data));
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
	MEMSINGLELIST_LOOP_BEGIN(g_renderableManager, i, renderable *)
		moduleRenderableFree(NULL, i, NULL);
	MEMSINGLELIST_LOOP_END(g_renderableManager, i, renderable *, return)
}