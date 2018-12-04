#include "model.h"


memoryPool *modelManager;


#warning "What if we aren't using the global memory manager?"


return_t moduleModelSetup(){
	//The module's setup will be successful if we
	//can allocate enough memory for our manager
	//and the error object can be setup correctly.
	return(
		modelManager = memoryManagerGlobalAlloc(MODEL_BASE_MEMORY + sizeof(modelManager)) &&
		modelSetupError()
	);
}


//Allocate memory for a model
//and return a handle to it.
model *moduleModelAlloc(){
	return(memPoolAlloc(modelManager));
}

//Free a model that has been allocated.
void moduleModelFree(model *mdl){
	modelDelete(mdl);
	memPoolFree(modelManager, mdl);
}


void moduleModelCleanup(){
	/*size_t i = loadedModels.size;
	while(i > 0){
		--i;
		modelDelete((model *)vectorGet(&loadedModels, i));
		vectorRemove(&loadedModels, i);
	}*/
	memoryManagerGlobalFree(modelManager);
}