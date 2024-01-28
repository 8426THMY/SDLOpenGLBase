#include "moduleModel.h"


#include "memoryManager.h"


// modelDef
moduleDefinePool(ModelDef, modelDef, g_modelDefManager, MODULE_MODELDEF_MANAGER_SIZE)
moduleDefinePoolFreeFlexible(ModelDef, modelDef, g_modelDefManager, modelDefDelete)
// model
moduleDefineSingleList(Model, model, g_modelManager, MODULE_MODEL_MANAGER_SIZE)
moduleDefineSingleListFreeFlexible(Model, model, g_modelManager, modelDelete)


return_t moduleModelSetup(){
	return(moduleModelDefInit() && moduleModelInit() && modelSetup());
}

void moduleModelCleanup(){
	modelCleanup();
	moduleModelDelete();
	moduleModelDefDelete();
}