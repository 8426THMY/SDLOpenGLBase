#ifndef moduleModel_h
#define moduleModel_h


#include "utilTypes.h"

#include "memoryManager.h"
#include "memoryPool.h"

#include "model.h"


#define MODULEMODEL
#define MODULE_MODEL_SETUP_FAIL 5

#define MODULE_MODEL_ELEMENT_SIZE sizeof(model)

#ifndef MEMORY_MODULE_NUM_MODELS
	#define MEMORY_MODULE_NUM_MODELS 1
#endif

#define MODULE_MODEL_MANAGER_SIZE memPoolMemoryForBlocks(MEMORY_MODULE_NUM_MODELS, MODULE_MODEL_ELEMENT_SIZE)


return_t moduleModelSetup();
void moduleModelCleanup();

model *moduleModelAlloc();
void moduleModelFree(model *mdl);
void moduleModelClear();


extern memoryPool modelManager;


#endif