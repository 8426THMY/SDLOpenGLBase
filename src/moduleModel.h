#ifndef moduleModel_h
#define moduleModel_h


#include "utilTypes.h"

#include "memoryManager.h"
#include "memoryPool.h"

#include "model.h"


#define MODULEMODEL
#define MODULEMODEL_SETUP_FAIL 4

#ifndef MEMORY_MAX_MODELS
	#define MEMORY_MAX_MODELS 1
#endif


return_t moduleModelSetup();

model *moduleModelAlloc();
void moduleModelFree(model *mdl);

void moduleModelCleanup();


extern memoryPool modelManager;


#endif