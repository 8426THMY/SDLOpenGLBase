#ifndef moduleModel_h
#define moduleModel_h


#define MODULEMODEL

#define MODULEMODEL_SETUP_FAIL 4


#include "utilTypes.h"

#include "memoryManager.h"
#include "memoryPool.h"

#include "model.h"


return_t moduleModelSetup();

model *moduleModelAlloc();
void moduleModelFree(model *mdl);

void moduleModelCleanup();


extern memoryPool modelManager;


#endif