#ifndef moduleModel_h
#define moduleModel_h


#include "model.h"

#include "utilTypes.h"
#include "moduleShared.h"


#define MODULE_MODEL
#define MODULE_MODEL_SETUP_FAIL 6

#define MODULE_MODELDEF_ELEMENT_SIZE sizeof(modelDef)
#define MODULE_MODEL_ELEMENT_SIZE    sizeof(model)

#ifndef MEMORY_MODULE_NUM_MODELDEFS
	#define MEMORY_MODULE_NUM_MODELDEFS 1
#endif
#ifndef MEMORY_MODULE_NUM_MODELS
	#define MEMORY_MODULE_NUM_MODELS 1
#endif

#define MODULE_MODELDEF_MANAGER_SIZE \
	memPoolMemoryForBlocks(MEMORY_MODULE_NUM_MODELDEFS, MODULE_MODELDEF_ELEMENT_SIZE)
#define MODULE_MODEL_MANAGER_SIZE \
	memSingleListMemoryForBlocks(MEMORY_MODULE_NUM_MODELS, MODULE_MODEL_ELEMENT_SIZE)


moduleDeclarePool(ModelDef, modelDef, g_modelDefManager)
moduleDeclareSingleList(Model, model, g_modelManager)

return_t moduleModelSetup();
void moduleModelCleanup();


#endif