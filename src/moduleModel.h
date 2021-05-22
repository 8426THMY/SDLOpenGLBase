#ifndef moduleModel_h
#define moduleModel_h


#include "utilTypes.h"

#include "memoryManager.h"
#include "memoryPool.h"
#include "memorySingleList.h"

#include "model.h"


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

#define MODULE_MODELDEF_MANAGER_SIZE memPoolMemoryForBlocks(MEMORY_MODULE_NUM_MODELDEFS, MODULE_MODELDEF_ELEMENT_SIZE)
#define MODULE_MODEL_MANAGER_SIZE    memSingleListMemoryForBlocks(MEMORY_MODULE_NUM_MODELS, MODULE_MODEL_ELEMENT_SIZE)


return_t moduleModelSetup();
void moduleModelCleanup();

modelDef *moduleModelDefAlloc();
void moduleModelDefFree(modelDef *const restrict mdlDef);
void moduleModelDefClear();

model *moduleModelAlloc();
model *moduleModelPrepend(model **const restrict start);
model *moduleModelAppend(model **const restrict start);
model *moduleModelInsertBefore(model **const restrict start, model *const restrict prevData);
model *moduleModelInsertAfter(model **const restrict start, model *const restrict data);
model *moduleModelNext(const model *const restrict render);
void moduleModelFree(model **const restrict start, model *const restrict render, model *const restrict prevData);
void moduleModelFreeArray(model **const restrict start);
void moduleModelClear();


extern memoryPool g_modelDefManager;
extern memorySingleList g_modelManager;


#endif