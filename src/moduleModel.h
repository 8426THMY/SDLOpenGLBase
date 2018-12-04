#ifndef moduleModel_h
#define moduleModel_h


#define MODULEMODEL

#define MODULEMODEL_SETUP_FAIL     2
#define MODULEMODEL_CONFIGURE_FAIL 2


#include "utilTypes.h"

#include "memoryManager.h"
#include "memoryPool.h"

#include "model.h"


return_t moduleModelSetup();

model *moduleModelAlloc();
void moduleModelFree(model *mdl);

void moduleModelCleanup();


extern memoryPool *modelManager;


#endif