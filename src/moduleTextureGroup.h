#ifndef moduleTextureGroup_h
#define moduleTextureGroup_h


#define MODULETEXGROUP

#define MODULETEXGROUP_SETUP_FAIL 2


#include "utilTypes.h"

#include "memoryManager.h"
#include "memoryPool.h"

#include "textureGroup.h"


return_t moduleTexGroupSetup();

textureGroup *moduleTexGroupAlloc();
void moduleTexGroupFree(textureGroup *texGroup);

void moduleTexGroupCleanup();


textureGroup *moduleTexGroupFind(const char *name);


extern memoryPool texGroupManager;


#endif