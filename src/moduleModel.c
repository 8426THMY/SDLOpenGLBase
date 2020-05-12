#include "moduleModel.h"


memoryPool g_modelManager;


#warning "What if we aren't using the global memory manager?"


return_t moduleModelSetup(){
	// The module's setup will be successful if we
	// can allocate enough memory for our manager
	// and the error object can be setup correctly.
	return(
		memPoolInit(
			&g_modelManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_MODEL_MANAGER_SIZE)),
			MODULE_MODEL_MANAGER_SIZE, MODULE_MODEL_ELEMENT_SIZE
		) != NULL &&
		meshSetupDefault() && modelSetupDefault()
	);
}

void moduleModelCleanup(){
	moduleModelClear();
	memoryManagerGlobalFree(memPoolRegionStart(g_modelManager.region));
}


// Allocate memory for a model and return a handle to it.
model *moduleModelAlloc(){
	return(memPoolAlloc(&g_modelManager));
}

// Free a model that has been allocated.
void moduleModelFree(model *const restrict mdl){
	modelDelete(mdl);
	memPoolFree(&g_modelManager, mdl);
}

// Delete every model in the manager.
void moduleModelClear(){
	MEMPOOL_LOOP_BEGIN(g_modelManager, i, model)
		moduleModelFree(i);
	MEMPOOL_LOOP_END(g_modelManager, i, return)
}