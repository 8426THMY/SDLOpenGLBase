#include "moduleModel.h"


memoryPool g_modelDefManager;
memorySingleList g_modelManager;


#warning "What if we aren't using the global memory manager?"


return_t moduleModelSetup(){
	// The module's setup will be successful if we
	// can allocate enough memory for our manager
	// and the error object can be setup correctly.
	return(
		// modelDef
		memPoolInit(
			&g_modelDefManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_MODELDEF_MANAGER_SIZE)),
			MODULE_MODELDEF_MANAGER_SIZE, MODULE_MODELDEF_ELEMENT_SIZE
		) != NULL &&
		// model
		memSingleListInit(
			&g_modelManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_MODEL_MANAGER_SIZE)),
			MODULE_MODEL_MANAGER_SIZE, MODULE_MODEL_ELEMENT_SIZE
		) != NULL &&
		modelSetup()
	);
}

void moduleModelCleanup(){
	// model
	MEMSINGLELIST_LOOP_BEGIN(g_modelManager, i, model)
		moduleModelFree(NULL, i, NULL);
	MEMSINGLELIST_LOOP_END(g_modelManager, i)
	memoryManagerGlobalDeleteRegions(g_modelManager.region);
	// modelDef
	MEMPOOL_LOOP_BEGIN(g_modelDefManager, i, modelDef)
		moduleModelDefFree(i);
	MEMPOOL_LOOP_END(g_modelDefManager, i)
	modelCleanup();
	memoryManagerGlobalDeleteRegions(g_modelDefManager.region);
}


// Allocate memory for a model base and return a handle to it.
modelDef *moduleModelDefAlloc(){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memPoolAlloc(&g_modelDefManager));
	#else
	modelDef *newBlock = memPoolAlloc(&g_modelDefManager);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memPoolExtend(
			&g_modelDefManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_MODELDEF_MANAGER_SIZE)),
			MODULE_MODELDEF_MANAGER_SIZE
		)){
			newBlock = memPoolAlloc(&g_modelDefManager);
		}
	}
	return(newBlock);
	#endif
}

// Free a model base that has been allocated.
void moduleModelDefFree(modelDef *const restrict mdl){
	modelDefDelete(mdl);
	memPoolFree(&g_modelDefManager, mdl);
}

// Delete every model base in the manager.
void moduleModelDefClear(){
	MEMPOOL_LOOP_BEGIN(g_modelDefManager, i, modelDef)
		moduleModelDefFree(i);
	MEMPOOL_LOOP_END(g_modelDefManager, i)
	memPoolClear(&g_modelDefManager);
}


// Allocate a new model array.
model *moduleModelAlloc(){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListAlloc(&g_modelManager));
	#else
	model *newBlock = memSingleListAlloc(&g_modelManager);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_modelManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_MODEL_MANAGER_SIZE)),
			MODULE_MODEL_MANAGER_SIZE
		)){
			newBlock = memSingleListAlloc(&g_modelManager);
		}
	}
	return(newBlock);
	#endif
}

// Insert an model at the beginning of an array.
model *moduleModelPrepend(model **const restrict start){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListPrepend(&g_modelManager, (void **)start));
	#else
	model *newBlock = memSingleListPrepend(&g_modelManager, (void **)start);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_modelManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_MODEL_MANAGER_SIZE)),
			MODULE_MODEL_MANAGER_SIZE
		)){
			newBlock = memSingleListPrepend(&g_modelManager, (void **)start);
		}
	}
	return(newBlock);
	#endif
}

// Insert an model at the end of an array.
model *moduleModelAppend(model **const restrict start){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListAppend(&g_modelManager, (void **)start));
	#else
	model *newBlock = memSingleListAppend(&g_modelManager, (void **)start);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_modelManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_MODEL_MANAGER_SIZE)),
			MODULE_MODEL_MANAGER_SIZE
		)){
			newBlock = memSingleListAppend(&g_modelManager, (void **)start);
		}
	}
	return(newBlock);
	#endif
}

// Insert an model after the element "prevData".
model *moduleModelInsertBefore(model **const restrict start, model *const restrict prevData){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListInsertBefore(&g_modelManager, (void **)start, (void *)prevData));
	#else
	model *newBlock = memSingleListInsertBefore(&g_modelManager, (void **)start, (void *)prevData);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_modelManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_MODEL_MANAGER_SIZE)),
			MODULE_MODEL_MANAGER_SIZE
		)){
			newBlock = memSingleListInsertBefore(&g_modelManager, (void **)start, (void *)prevData);
		}
	}
	return(newBlock);
	#endif
}

// Insert an model after the element "data".
model *moduleModelInsertAfter(model **const restrict start, model *const restrict data){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListInsertAfter(&g_modelManager, (void **)start, (void *)data));
	#else
	model *newBlock = memSingleListInsertAfter(&g_modelManager, (void **)start, (void *)data);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_modelManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_MODEL_MANAGER_SIZE)),
			MODULE_MODEL_MANAGER_SIZE
		)){
			newBlock = memSingleListInsertAfter(&g_modelManager, (void **)start, (void *)data);
		}
	}
	return(newBlock);
	#endif
}

model *moduleModelNext(const model *const restrict mdl){
	return(memSingleListNext(mdl));
}

// Free an model that has been allocated.
void moduleModelFree(model **const restrict start, model *const restrict mdl, model *const restrict prevData){
	modelDelete(mdl);
	memSingleListFree(&g_modelManager, (void **)start, (void *)mdl, (void *)prevData);
}

// Free an entire model array.
void moduleModelFreeArray(model **const restrict start){
	model *mdl = *start;
	while(mdl != NULL){
		moduleModelFree(start, mdl, NULL);
		mdl = *start;
	}
}

// Delete every model in the manager.
void moduleModelClear(){
	MEMSINGLELIST_LOOP_BEGIN(g_modelManager, i, model)
		moduleModelFree(NULL, i, NULL);
	MEMSINGLELIST_LOOP_END(g_modelManager, i)
	memSingleListClear(&g_modelManager);
}