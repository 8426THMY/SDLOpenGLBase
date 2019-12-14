#include "moduleModel.h"


memoryPool modelManager;


#warning "What if we aren't using the global memory manager?"


return_t moduleModelSetup(){
	// The module's setup will be successful if we
	// can allocate enough memory for our manager
	// and the error object can be setup correctly.
	return(
		memPoolInit(
			&modelManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_MODEL_MANAGER_SIZE)),
			MODULE_MODEL_MANAGER_SIZE, MODULE_MODEL_ELEMENT_SIZE
		) != NULL &&
		modelSetupDefault()
	);
}

void moduleModelCleanup(){
	moduleModelClear();
	memoryManagerGlobalFree(memPoolRegionStart(modelManager.region));
}


// Allocate memory for a model and return a handle to it.
model *moduleModelAlloc(){
	return(memPoolAlloc(&modelManager));
}

// Free a model that has been allocated.
void moduleModelFree(model *mdl){
	modelDelete(mdl);
	memPoolFree(&modelManager, mdl);
}

// Delete every model in the manager.
void moduleModelClear(){
	MEMPOOL_LOOP_BEGIN(modelManager, i, model *)
		moduleModelFree(i);
	MEMPOOL_LOOP_END(modelManager, i, model *, return)
}