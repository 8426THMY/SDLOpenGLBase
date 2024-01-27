#include "moduleModel.h"


#include "memoryManager.h"


moduleDefinePool(ModelDef, modelDef, g_modelDefManager, modelDefDelete, MODULE_MODELDEF_MANAGER_SIZE)
moduleDefineSingleList(Model, model, g_modelManager, modelDelete, MODULE_MODEL_MANAGER_SIZE)


return_t moduleModelSetup(){
	// The module's setup will be successful if we
	// can allocate enough memory for our manager
	// and the error object can be setup correctly.
	return(
		// modelDef
		memPoolInit(
			&g_modelDefManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_MODELDEF_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_MODELDEF_MANAGER_SIZE), MODULE_MODELDEF_ELEMENT_SIZE
		) != NULL &&
		// model
		memSingleListInit(
			&g_modelManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_MODEL_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_MODEL_MANAGER_SIZE), MODULE_MODEL_ELEMENT_SIZE
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