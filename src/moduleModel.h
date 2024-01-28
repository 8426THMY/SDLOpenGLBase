#ifndef moduleModel_h
#define moduleModel_h


#include "model.h"

#include "memoryPool.h"
#include "memorySingleList.h"

#include "utilTypes.h"
#include "moduleShared.h"


#define MODULE_MODEL
#define MODULE_MODEL_SETUP_FAIL 6

#ifndef MEMORY_MODULE_NUM_MODELDEFS
	#define MEMORY_MODULE_NUM_MODELDEFS 1
#endif
#ifndef MEMORY_MODULE_NUM_MODELS
	#define MEMORY_MODULE_NUM_MODELS 1
#endif

#define MODULE_MODELDEF_MANAGER_SIZE \
	memPoolMemoryForBlocks(MEMORY_MODULE_NUM_MODELDEFS, sizeof(modelDef))
#define MODULE_MODEL_MANAGER_SIZE \
	memSingleListMemoryForBlocks(MEMORY_MODULE_NUM_MODELS, sizeof(model))


// modelDef
moduleDeclarePool(ModelDef, modelDef, g_modelDefManager)
moduleDeclarePoolFree(ModelDef, modelDef)
// model
moduleDeclareSingleList(Model, model, g_modelManager)
moduleDeclareSingleListFree(Model, model)

return_t moduleModelSetup();
void moduleModelCleanup();


#endif